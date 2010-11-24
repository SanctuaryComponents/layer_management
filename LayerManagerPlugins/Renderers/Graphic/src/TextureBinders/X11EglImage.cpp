/***************************************************************************
*
* Copyright 2010 BMW Car IT GmbH
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*		http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
****************************************************************************/

#include "TextureBinders/X11EglImage.h"
#include "X11/extensions/Xcomposite.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2ext.h>
#include "PlatformSurfaces/EglXPlatformSurface.h"
#include "Log.h"

	X11EglImage::X11EglImage(Display* x11display) : m_x11display(x11display){
		LOG_INFO("X11EglImage", "Query EGL Extensions ...");

		m_pfGLEglImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)
			eglGetProcAddress("glEGLImageTargetTexture2DOES");
		m_pfEglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)
			eglGetProcAddress("eglCreateImageKHR");
		m_pfEglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)
			eglGetProcAddress("eglDestroyImageKHR");

		if (!m_pfEglCreateImageKHR || !m_pfEglDestroyImageKHR || !m_pfGLEglImageTargetTexture2DOES)
		{
			LOG_ERROR("X11EglImage", "Query EGL Extensions failed");
		}

		m_eglDisplay = eglGetDisplay((EGLNativeDisplayType)m_x11display);
	}
	void X11EglImage::bindSurfaceTexture(Surface*s){
		EglXPlatformSurface* nativeSurface = (EglXPlatformSurface*)s->platform;
		glBindTexture(GL_TEXTURE_2D, nativeSurface->texture);
		if (nativeSurface->eglImage != NULL){
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			m_pfGLEglImageTargetTexture2DOES(GL_TEXTURE_2D, nativeSurface->eglImage);
		}
	}

	void X11EglImage::unbindSurfaceTexture(Surface*s){

	}

	void X11EglImage::createClientBuffer(Surface* surface){
		LOG_DEBUG("X11EglImage", "creating client buffer");
		EglXPlatformSurface* nativeSurface = (EglXPlatformSurface*)surface->platform;
		Pixmap windowPixmap = 0;
		windowPixmap= XCompositeNameWindowPixmap (m_x11display, surface->nativeHandle);
		if (windowPixmap==0)
			LOG_ERROR("X11EglImage", "didnt create pixmap!");

		EGLImageKHR eglImage = 0;
		LOG_DEBUG("X11EglImage", "creating EGL Image from client buffer");
		if (nativeSurface->eglImage != NULL)
		{
			m_pfEglDestroyImageKHR(m_eglDisplay, nativeSurface->eglImage);
			glDeleteTextures(1,&nativeSurface->texture);
		}
		eglImage = m_pfEglCreateImageKHR(m_eglDisplay,
									 EGL_NO_CONTEXT,
									 EGL_NATIVE_PIXMAP_KHR,
									 (EGLClientBuffer)windowPixmap,
									 NULL);
		if (eglImage == 0)
		{
			LOG_DEBUG("X11EglImage", "could not allocate EGL Image for window");
		} else {
			nativeSurface->eglImage = eglImage;
			glGenTextures(1,&nativeSurface->texture);
		}
	}

	PlatformSurface* X11EglImage::createPlatformSurface(Surface* s){
		return new EglXPlatformSurface(s);
	}

	void X11EglImage::destroyClientBuffer(Surface* surface){
		EglXPlatformSurface* nativeSurface = (EglXPlatformSurface*)surface->platform;
		if (nativeSurface != NULL && nativeSurface->eglImage != NULL )
		{
		      m_pfEglDestroyImageKHR(m_eglDisplay, nativeSurface->eglImage);
	              glDeleteTextures(1,&nativeSurface->texture);
		}
		glDeleteTextures(1,&nativeSurface->texture);
	}
