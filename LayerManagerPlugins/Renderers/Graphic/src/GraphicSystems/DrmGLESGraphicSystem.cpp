/***************************************************************************
 *
 * Copyright 2010,2011 BMW Car IT GmbH
 * Copyright (C) 2011 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

#include "GraphicSystems/DrmGLESGraphicSystem.h"
#include "IlmMatrix.h"
#include "string.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES2/gl2.h"
#include "Bitmap.h"
#include "Transformation/ViewportTransform.h"
#include "PlatformSurfaces/WaylandPlatformSurface.h"
#include "WindowSystems/WaylandBaseWindowSystem.h"

#include "wayland-server.h"

#include <errno.h>
#include <drm_fourcc.h>
#include <drm_mode.h>
#include <gbm.h>
#include <libudev.h>

static drmModeModeInfo builtin_800x480_for_Crossville = {
	31746,
	800, 859, 999, 999, 0,
	480, 507, 538, 524, 0,
	0,
	DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_PVSYNC,
	0,
	"800x480"
};

DrmGLESGraphicSystem::DrmGLESGraphicSystem(int windowWidth, int windowHeight,
                                     PfnShaderProgramCreator shaderProgram)
: GLESGraphicsystem(windowWidth, windowHeight, shaderProgram)
, m_gbm(NULL)
, m_fdDev(-1)
, m_crtcs(NULL)
, m_crtcsNum(0)
, m_crtcAllocator(0)
, m_connectorAllocator(0)
{
    LOG_DEBUG("DrmGLESGraphicSystem", "creating DrmGLESGraphicSystem");

	m_pfGLEglImageTargetRenderbufferStorageOES =
		(PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC)eglGetProcAddress("glEGLImageTargetRenderbufferStorageOES");
	m_pfEglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
	m_pfEglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");
	m_pfEglBindWaylandDisplayWL = (PFNEGLBINDWAYLANDDISPLAYWL)eglGetProcAddress("eglBindWaylandDisplayWL");
	m_pfEglUnbindWaylandDisplayWL = (PFNEGLUNBINDWAYLANDDISPLAYWL)eglGetProcAddress("eglUnbindWaylandDisplayWL");

	if (!m_pfGLEglImageTargetRenderbufferStorageOES ||
		!m_pfEglCreateImageKHR ||
		!m_pfEglDestroyImageKHR ||
		!m_pfEglBindWaylandDisplayWL ||
		!m_pfEglUnbindWaylandDisplayWL)
	{
		LOG_ERROR("DrmGLESGraphicSystem", "Query EGL extensions failed.");
	}
}

DrmGLESGraphicSystem::~DrmGLESGraphicSystem()
{
	WaylandBaseWindowSystem* windowSystem = dynamic_cast<WaylandBaseWindowSystem*>(m_baseWindowSystem);
	struct wl_display* wlDisplay = windowSystem->getNativeDisplayHandle();

	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
							  GL_COLOR_ATTACHMENT0,
							  GL_RENDERBUFFER,
							  0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	struct DrmOutput* output = NULL;
	wl_list_for_each(output, &m_outputList, link) {
		glDeleteRenderbuffers(2, output->rbo);
		for (int i = 0; i < 2; ++i){
			drmModeRmFB(m_fdDev, output->fbID[i]);
			gbm_bo_destroy(output->bo[i]);
		}
		free(output);
	}

	if (wlDisplay)
		m_pfEglUnbindWaylandDisplayWL(m_eglDisplay, wlDisplay);
}

bool DrmGLESGraphicSystem::init(EGLNativeDisplayType display, EGLNativeWindowType NativeWindow)
{
	bool ret = true;
    LOG_DEBUG("DrmGLESGraphicSystem", "init..display:"  << display <<
									  ", NativeWindow:" << NativeWindow);

	m_nativeDisplay = display;
	m_nativeWindow  = NativeWindow;

	wl_list_init(&m_outputList);

	m_gbm = (gbm_device*)display;
	if (m_gbm == NULL)
	{
		LOG_ERROR("DrmGLESGraphicSystem", "gbm device is NULL.");
		return false;
	}

	m_fdDev = gbm_device_get_fd(m_gbm);
	if (m_fdDev < 0)
	{
		LOG_ERROR("DrmGLESGraphicSystem", "failed to get device fd.");
		return false;
	}

	m_eglDisplay = eglGetDisplay((EGLNativeDisplayType)m_gbm);
	if (m_eglDisplay == EGL_NO_DISPLAY)
	{
		LOG_ERROR("DrmGLESGraphicSystem", "failed to get EGL display.");
		return false;
	}

	EGLint major, minor;
	if (!eglInitialize(m_eglDisplay, &major, &minor))
	{
		LOG_ERROR("DrmGLESGraphicSystem", "failed to initialising EGL.");
		return false;
	}

	const char* ext = eglQueryString(m_eglDisplay, EGL_EXTENSIONS);
	if (!strstr(ext, "EGL_KHR_surfaceless_gles2"))
	{
		LOG_ERROR("DrmGLESGraphicSystem", "EGL_KHR_surfaceless_gles2 not avaiable.");
		return false;
	}

	eglBindAPI(EGL_OPENGL_ES_API);

	EGLint contextAttrs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	m_eglContext = eglCreateContext(m_eglDisplay, NULL, EGL_NO_CONTEXT, contextAttrs);
	if (!m_eglContext)
	{
		LOG_ERROR("DrmGLESGraphicSystem", "failed to create EGL context.");
		return false;
	}

	if (!eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, m_eglContext))
	{
		LOG_ERROR("DrmGLESGraphicSystem", "failed to make context current.");
		return false;
	}
	LOG_INFO("DrmGLESGraphicSystem", "make context current.");

	eglSwapInterval(m_eglDisplay, 1); // TODO: does not seem to work

	if (!initializeSystem())
	{
		LOG_ERROR("DrmGLESGraphicSystem", "failed to initialize system.");
		return false;
	}

	if (!GLESGraphicsystem::initOpenGLES(m_windowWidth, m_windowHeight))
	{
		LOG_ERROR("DrmGLESGraphicSystem", "failed to initialize gles.");
		return false;
	}

    return ret;
}

void DrmGLESGraphicSystem::activateGraphicContext()
{
	struct DrmOutput* output = NULL;
	wl_list_for_each(output, &m_outputList, link)
	{
		drmOutputPrepareRender(output);
	}
}

void DrmGLESGraphicSystem::releaseGraphicContext()
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
							  GL_COLOR_ATTACHMENT0,
							  GL_RENDERBUFFER,
							  NULL);
}

bool DrmGLESGraphicSystem::initializeSystem()
{
    LOG_DEBUG("DrmGLESGraphicSystem", "initializeSystem IN");

	WaylandBaseWindowSystem* windowSystem = dynamic_cast<WaylandBaseWindowSystem*>(m_baseWindowSystem);
	struct wl_display* wlDisplay = windowSystem->getNativeDisplayHandle();

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// bind display
	m_pfEglBindWaylandDisplayWL(m_eglDisplay, wlDisplay);

	if (!createOutputs())
		return false;

    LOG_DEBUG("DrmGLESGraphicSystem", "initializeSystem OUT");

	return true;
}

bool DrmGLESGraphicSystem::createOutputs()
{
    LOG_DEBUG("DrmGLESGraphicSystem", "createOutputs IN");

	drmModeConnector* connector;
	drmModeRes*       resources;
	int x = 0, y = 0;

	resources = drmModeGetResources(m_fdDev);
	if (!resources)
	{
		LOG_DEBUG("DrmGLESGraphicSystem", "drmModeGetResources failed.");
		return false;
	}

	m_crtcs = (uint32_t*)calloc(resources->count_crtcs, sizeof(uint32_t));
	if (!m_crtcs)
	{
		return false;
	}
	m_crtcsNum = resources->count_crtcs;

	memcpy(m_crtcs, resources->crtcs, sizeof(uint32_t) * m_crtcsNum);

	for (int ii = 0; ii < resources->count_connectors; ++ii)
	{
		connector = drmModeGetConnector(m_fdDev, resources->connectors[ii]);
		if (connector == NULL)
			continue;

		if (connector->connection == DRM_MODE_CONNECTED)
		{
			if (createOutputForConnector(resources, connector, x, y) < 0)
			{
				drmModeFreeConnector(connector);
				continue;
			}

			//x += container_of(m_outputList.prev, struct DrmOutput, link)->currentMode->width;
		}

		drmModeFreeConnector(connector);
		break;
	}

	if (wl_list_empty(&m_outputList))
	{
		LOG_ERROR("DrmGLESGraphicSystem", "DrmOutput list is empty.");
		return false;
	}

	drmModeFreeResources(resources);

    LOG_DEBUG("DrmGLESGraphicSystem", "createOutputs OUT");
	return true;
}

int DrmGLESGraphicSystem::createOutputForConnector(drmModeRes* resources,
												   drmModeConnector* connector,
												   int, int)
{
	LOG_DEBUG("DrmGLESGraphicSystem", "createOutputForConnector IN");
	int ret;
	int ii;
	drmModeEncoder* encoder;
	unsigned handle, stride;
	struct DrmMode*	drmMode = NULL;

	encoder = drmModeGetEncoder(m_fdDev, connector->encoders[0]);
	if (encoder == NULL){
		LOG_ERROR("DrmGLESGraphicSystem", "No encoder for connector.");
		return -1;
	}

	for (ii = 0; ii < resources->count_crtcs; ++ii)
	{
		if (encoder->possible_crtcs & (1 << ii) &&
			!(m_crtcAllocator & (1 << resources->crtcs[ii])))
		{
			break;
		}
	}
	if (ii == resources->count_crtcs)
	{
		LOG_ERROR("DrmGLESGraphicSystem", "No usable crtc for encoder.");
		return -1;
	}

	DrmOutput* output = (DrmOutput*)malloc(sizeof *output);
	if (output == NULL)
	{
		drmModeFreeEncoder(encoder);
		return -1;
	}
	memset(output, 0x00, sizeof *output);

	output->fdDev       = m_fdDev;
	output->crtcID      = resources->crtcs[ii];
	output->connectorID = connector->connector_id;

	m_crtcAllocator      |= (1 << output->crtcID);
	m_connectorAllocator |= (1 << output->connectorID);

	wl_list_init(&output->modeList);

	output->orgCrtc = drmModeGetCrtc(m_fdDev, output->crtcID);
	drmModeFreeEncoder(encoder);

	for (ii = 0; ii < connector->count_modes; ++ii)
	{
		if (drmOutputAddMode(output, &connector->modes[ii]))
			goto err_free;
	}

	if (connector->count_modes == 0)
	{
		if (drmOutputAddMode(output, &builtin_800x480_for_Crossville))
			goto err_free;
	}

	drmMode = container_of(output->modeList.next, struct DrmMode, link);
	output->currentMode = drmMode;
	drmMode->flags = WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED;

	glGenRenderbuffers(2, output->rbo);
	for (ii = 0; ii < 2; ++ii)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, output->rbo[ii]);

		output->bo[ii] = gbm_bo_create(m_gbm,
									   output->currentMode->width,
									   output->currentMode->height,
									   GBM_BO_FORMAT_XRGB8888,
									   GBM_BO_USE_SCANOUT |
									   GBM_BO_USE_RENDERING);
		if (!output->bo[ii])
		{
			LOG_ERROR("DrmGLESGraphicSystem", "failed to create bo.");
			goto err_bufs;
		}

		output->image[ii] = m_pfEglCreateImageKHR(m_eglDisplay,
												  NULL,
												  EGL_NATIVE_PIXMAP_KHR,
												  output->bo[ii],
												  NULL);
		if (!output->image[ii])
			goto err_bufs;

		m_pfGLEglImageTargetRenderbufferStorageOES(GL_RENDERBUFFER,
												   output->image[ii]);

		stride = gbm_bo_get_pitch(output->bo[ii]);
		handle = gbm_bo_get_handle(output->bo[ii]).u32;

		ret = drmModeAddFB(m_fdDev, drmMode->width, drmMode->height,
			24, 32, stride, handle, &output->fbID[ii]);
		if (ret)
		{
			LOG_ERROR("DrmGLESGraphicSystem", "failed to add fb(" << ii << ")");
			goto err_bufs;
		}
	}

	output->current = 0;
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
							  GL_COLOR_ATTACHMENT0,
							  GL_RENDERBUFFER,
							  output->rbo[output->current]);

	ret = drmModeSetCrtc(m_fdDev, output->crtcID,
						 output->fbID[output->current ^ 1], 0, 0,
						 &output->connectorID, 1,
						 &drmMode->modeInfo);
	if (ret)
	{
		LOG_ERROR("DrmGLESGraphicSystem", "failed to set mode.");
		goto err_fb;
	}

	wl_list_insert(m_outputList.prev, &output->link);

	LOG_DEBUG("DrmGLESGraphicSystem", "createOutputForConnector OUT (NORMAL)");
	return 0;

err_fb:
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
							  GL_COLOR_ATTACHMENT0,
							  GL_RENDERBUFFER,
							  0);

err_bufs:
	for (ii = 0; ii < 2; ++ii){
		if (output->fbID[ii] != 0)
			drmModeRmFB(m_fdDev, output->fbID[ii]);
		if (output->image[ii])
			m_pfEglDestroyImageKHR(m_eglDisplay, output->image[ii]);
		if (output->bo[ii])
			gbm_bo_destroy(output->bo[ii]);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glDeleteRenderbuffers(2, output->rbo);

err_free:
	drmModeFreeCrtc(output->orgCrtc);
	m_crtcAllocator &= ~(1 << output->crtcID);
	m_connectorAllocator &= ~(1 << output->connectorID);

	free(output);
	LOG_DEBUG("DrmGLESGraphicSystem", "createOutputForConnector OUT (ERROR)");
	return -1;
}

int DrmGLESGraphicSystem::drmOutputAddMode(struct DrmOutput* output, drmModeModeInfo* info)
{
	struct DrmMode* mode;

	mode = (struct DrmMode*)malloc(sizeof *mode);
	if (mode == NULL)
		return -1;

	mode->flags    = 0;
	mode->width    = info->hdisplay;
	mode->height   = info->vdisplay;
	mode->refresh  = info->vrefresh;
	mode->modeInfo = *info;
	wl_list_insert(output->modeList.prev, &mode->link);

	return 0;
}

int DrmGLESGraphicSystem::drmOutputPrepareRender(struct DrmOutput* output)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
							  GL_COLOR_ATTACHMENT0,
							  GL_RENDERBUFFER,
							  output->rbo[output->current]);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("DrmGLESGraphicSystem", "glCheckFramebufferStatus() failed.");
		return -1;
	}
	return 0;
}

void DrmGLESGraphicSystem::swapBuffers()
{
    LOG_DEBUG("DrmGLESGraphicSystem", "swapBuffers IN");

	struct DrmOutput* output = NULL;

	wl_list_for_each(output, &m_outputList, link)
	{
		glFinish();

		output->current ^= 1;
		uint32_t fbID = output->fbID[output->current ^ 1];
#if 0
		LOG_INFO("DrmGLESGraphicSystem", "fbID = " << fbID << ", current = " << output->current);
#endif
		if (drmModePageFlip(m_fdDev, output->crtcID, fbID, 0, NULL) < 0)
		{
			LOG_ERROR("DrmGLESGraphicSystem", "queueing pageflip failed");
		}
	}

    LOG_DEBUG("DrmGLESGraphicSystem", "swapBuffers OUT");
}

void DrmGLESGraphicSystem::applyLayerMatrix(IlmMatrix& matrix)
{
	GLESGraphicsystem::applyLayerMatrix(matrix);
	// reverse Y axis
	matrix.f[5] *= -1.0f;
}
