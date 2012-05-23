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

#include "TextureBinders/WaylandEglImage.h"
#include "config.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2ext.h>
#include "PlatformSurfaces/EglWaylandPlatformSurface.h"
#include "Log.h"
#include "wayland-server.h"

WaylandEglImage::WaylandEglImage(EGLDisplay eglDisplay, struct wl_display* wlDisplay)
: m_eglDisplay(eglDisplay)
, m_wlDisplay(wlDisplay)
{
    // pseudo require EGL to have been initialised
    // we dont really need the EGL handle as such

    m_pfGLEglImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");
    m_pfEglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
    m_pfEglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");

    if (!m_pfEglCreateImageKHR || !m_pfEglDestroyImageKHR || !m_pfGLEglImageTargetTexture2DOES)
    {
        LOG_ERROR("WaylandEglImage", "Query EGL Extensions failed");
    }
}

WaylandEglImage::~WaylandEglImage()
{
}

bool WaylandEglImage::bindSurfaceTexture(Surface* surface)
{
    EglWaylandPlatformSurface* nativeSurface = (EglWaylandPlatformSurface*)surface->platform;
    if (nativeSurface && nativeSurface->isReadyForRendering())
    {
        glBindTexture(GL_TEXTURE_2D, nativeSurface->texture);
        if (nativeSurface->eglImage)
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            m_pfGLEglImageTargetTexture2DOES(GL_TEXTURE_2D, nativeSurface->eglImage);
            return true;
        }
    }
    return false;
}

bool WaylandEglImage::unbindSurfaceTexture(Surface* surface)
{
    (void)surface; // TODO: remove, only prevents warning

    // TODO
    return true;
}

void WaylandEglImage::createClientBuffer(Surface* surface)
{
    LOG_DEBUG("WaylandEglImage", "creating client buffer with native display: " << m_wlDisplay << " for native handle: " << surface->getNativeContent());
    EglWaylandPlatformSurface* nativeSurface = (EglWaylandPlatformSurface*)surface->platform;
    if (NULL!=nativeSurface)
    {
        struct wl_buffer* buffer = (struct wl_buffer*)surface->getNativeContent();
        EGLImageKHR eglImage = 0;
        LOG_DEBUG("WaylandEglImage", "creating EGL Image from client buffer");
        if (nativeSurface->eglImage)
        {
#ifdef WITH_WAYLAND_DRM
            if (nativeSurface->texture){
                glBindTexture(GL_TEXTURE_2D, 0);
                glDeleteTextures(1,&nativeSurface->texture);
            }
#else
            m_pfEglDestroyImageKHR(m_eglDisplay, nativeSurface->eglImage);
            glDeleteTextures(1,&nativeSurface->texture);
#endif // WITH_WAYLAND_DRM
            nativeSurface->eglImage = 0;
            nativeSurface->texture = 0;
        }
        eglImage = m_pfEglCreateImageKHR(m_eglDisplay,
                                     EGL_NO_CONTEXT,
                                     EGL_WAYLAND_BUFFER_WL,
                                     (EGLClientBuffer)buffer,
                                     NULL);
        if (!eglImage)
        {
            LOG_DEBUG("WaylandEglImage", "could not allocate EGL Image for window");
        }
        else
        {
            nativeSurface->eglImage = eglImage;
            glGenTextures(1,&nativeSurface->texture);
        }
    } else {
        LOG_DEBUG("WaylandEglImage", "native surface is NULL-----------------------------------------------------");
    }
}

PlatformSurface* WaylandEglImage::createPlatformSurface(Surface* surface)
{
    return new EglWaylandPlatformSurface(surface);
}

void WaylandEglImage::destroyClientBuffer(Surface* surface)
{
    EglWaylandPlatformSurface* nativeSurface = (EglWaylandPlatformSurface*)surface->platform;
    if (nativeSurface && nativeSurface->eglImage)
    {
        m_pfEglDestroyImageKHR(m_eglDisplay, nativeSurface->eglImage);
        glDeleteTextures(1,&nativeSurface->texture);
        nativeSurface->eglImage = 0;
        nativeSurface->texture = 0;
    }
}
