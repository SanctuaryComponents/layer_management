/***************************************************************************
*
* Copyright 2010, 2011 BMW Car IT GmbH  
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

#include "TextureBinders/WaylandCopyGLES.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2ext.h>
#include "PlatformSurfaces/EglWaylandPlatformSurface.h"
#include "Log.h"
#include "wayland-server.h"

WaylandCopyGLES::WaylandCopyGLES(EGLDisplay eglDisplay, struct wl_display* wlDisplay)
: m_eglDisplay(eglDisplay)
, m_wlDisplay(wlDisplay)
{
}

WaylandCopyGLES::~WaylandCopyGLES()
{
}

bool WaylandCopyGLES::bindSurfaceTexture(Surface* surface)
{
    LOG_DEBUG("WaylandCopyGLES", "bindSurfaceTexture IN");
    EglWaylandPlatformSurface* nativeSurface = (EglWaylandPlatformSurface*)surface->platform;
    if (nativeSurface && nativeSurface->isReadyForRendering())
    {
        struct wl_buffer* buffer = (struct wl_buffer*)surface->getNativeContent();
        glBindTexture(GL_TEXTURE_2D, nativeSurface->texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        LOG_DEBUG("WaylandEglImage", "width:"<<surface->OriginalSourceWidth<<", height:"<<surface->OriginalSourceHeight);
        unsigned int* pBuf = (unsigned int*)wl_shm_buffer_get_data(buffer);
        LOG_DEBUG("WaylandEglImage", "buffer:"<<pBuf);
        {
            unsigned int* pTmp = (unsigned int*)pBuf;
            unsigned int col = 0;
            int cnt = 0;
            unsigned int* pCnvImg = (unsigned int*)malloc(sizeof(unsigned int) * surface->OriginalSourceWidth * surface->OriginalSourceHeight);
            unsigned int* pCnvImgTmp = pCnvImg;
            for (cnt = 0; cnt < surface->OriginalSourceWidth * surface->OriginalSourceHeight; cnt++, pTmp++, pCnvImgTmp++)
            {
                col = (unsigned int)*pTmp;
                // TODO:endian
                *pCnvImgTmp = (col&0xFF00FF00) | ((col&0x00FF0000)>>16) | ((col&0x000000FF)<<16);
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->OriginalSourceWidth, surface->OriginalSourceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pCnvImg);
            free(pCnvImg);
        }
        LOG_DEBUG("WaylandEglImage", "bindSurfaceTexture OUT");
        return true;
    }
    LOG_DEBUG("WaylandCopyGLES", "bindSurfaceTexture ERROR OUT");
    return false;
}

bool WaylandCopyGLES::unbindSurfaceTexture(Surface* surface)
{
    LOG_DEBUG("WaylandCopyGLES", "unbindSurfaceTexture:surface" << surface);
    return true;
}

void WaylandCopyGLES::createClientBuffer(Surface* surface)
{
    LOG_DEBUG("WaylandCopyGLES", "creating client buffer with native display: " << m_wlDisplay << " for native handle: " << surface->getNativeContent());
    EglWaylandPlatformSurface* nativeSurface = (EglWaylandPlatformSurface*)surface->platform;
    if (NULL!=nativeSurface)
    {
        if (nativeSurface->texture)
	{
            glDeleteTextures(1,&nativeSurface->texture);
            nativeSurface->texture = 0;
        }
        glGenTextures(1,&nativeSurface->texture);
    }
}

PlatformSurface* WaylandCopyGLES::createPlatformSurface(Surface* surface)
{
    return new EglWaylandPlatformSurface(surface);
}

void WaylandCopyGLES::destroyClientBuffer(Surface* surface)
{
    EglWaylandPlatformSurface* nativeSurface = (EglWaylandPlatformSurface*)surface->platform;
    if (nativeSurface && nativeSurface->texture)
    {
        glDeleteTextures(1,&nativeSurface->texture);
        nativeSurface->texture = 0;
    }
}
