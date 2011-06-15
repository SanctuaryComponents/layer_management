/***************************************************************************
*
* Copyright 2010,2011 BMW Car IT GmbH
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

#include "TextureBinders/X11CopyGLX.h"
#include "Surface.h"
#include "X11/Xlib.h"
#include "Log.h"

#include "GL/glx.h"

void X11CopyGLX::bindSurfaceTexture(Surface* surface)
{
    XPlatformSurface* nativeSurface = (XPlatformSurface*)surface->platform;
    Pixmap pixmap = 0;
    GLenum targetType = GL_BGRA;
    GLenum sourceType = GL_RGBA;

    pixmap = XCompositeNameWindowPixmap (dpy, surface->nativeHandle);
    if (!pixmap)
    {
        LOG_ERROR("X11CopyGLX", "didnt create pixmap!");
        return;
    }

    nativeSurface->pixmap = pixmap;
    XImage * xim = XGetImage(dpy, nativeSurface->pixmap, 0, 0, surface->OriginalSourceWidth, surface->OriginalSourceHeight, AllPlanes, ZPixmap);
    if (xim)
    {
        glBindTexture(GL_TEXTURE_2D, nativeSurface->texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (xim->depth == 24)
        {
            targetType = GL_BGR;
            sourceType = GL_RGB;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, sourceType, surface->OriginalSourceWidth, surface->OriginalSourceHeight, 0, targetType, GL_UNSIGNED_BYTE, xim->data);
        XDestroyImage(xim);
    }
}

void X11CopyGLX::createClientBuffer(Surface* surface)
{
    XPlatformSurface* nativeSurface = (XPlatformSurface*)surface->platform;
    glGenTextures(1,&nativeSurface->texture);
}
