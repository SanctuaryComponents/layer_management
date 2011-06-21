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

#include "TextureBinders/X11CopyGLES.h"
#include "Surface.h"
#include "X11/Xlib.h"
#include "Log.h"

#include "GLES2/gl2.h"

bool X11CopyGLES::bindSurfaceTexture(Surface* surface)
{   
    XPlatformSurface* nativeSurface = NULL;
    Pixmap pixmap = 0;
    GLenum targetType = GL_RGBA;
    GLenum sourceType = GL_RGBA;
	unsigned char* swapedData = NULL;
	bool swaprgb = false;
    if (surface != NULL ) 
    {
        nativeSurface = (XPlatformSurface*)surface->platform;
    } 
    else if( nativeSurface != NULL && surface->nativeHandle != 0 ) 
    {
	    pixmap = XCompositeNameWindowPixmap (dpy, surface->nativeHandle);
	    if (!pixmap)
	    {
		    LOG_ERROR("X11CopyGLES", "didnt create pixmap!");
		    return false;
	    }
	    nativeSurface->pixmap = pixmap;
	    XImage * xim = XGetImage(dpy, nativeSurface->pixmap, 0, 0, surface->OriginalSourceWidth, surface->OriginalSourceHeight, AllPlanes, ZPixmap);
	    if ( xim != NULL )
	    {
		    glBindTexture(GL_TEXTURE_2D, nativeSurface->texture);
		    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    if ( xim->depth == 24 )
		    {
			    targetType = GL_RGB;
			    sourceType = GL_RGB;
			    swaprgb = true;
			    swapedData = new unsigned char[surface->OriginalSourceWidth*surface->OriginalSourceHeight*3];
		    } else {
			    swapedData = new unsigned char[surface->OriginalSourceWidth*surface->OriginalSourceHeight*4];
		    }
		    swapPixmap((unsigned char*)xim->data, swapedData, surface->OriginalSourceWidth, surface->OriginalSourceHeight,swaprgb);
		    glTexImage2D(GL_TEXTURE_2D, 0, sourceType, surface->OriginalSourceWidth, surface->OriginalSourceHeight, 0, targetType, GL_UNSIGNED_BYTE, swapedData);
		    XDestroyImage(xim);
		    delete[] swapedData;
            return true;
	    }
    }
    return false;
}
void X11CopyGLES::swapPixmap(unsigned char* src,unsigned char* dest, unsigned int width,unsigned int height,bool swaprgb) 
{
	unsigned int count = 0; 
	if (swaprgb == false)
	{	
		count = width*height;
		for (int j=0;j<count; j++) {
			dest[j*4]=src[j*4+2];
			dest[j*4+1]=src[j*4+1];
			dest[j*4+2]=src[j*4];
			dest[j*4+3]=src[j*4+3];
		}
	} else {
		count = width*height;
		for (int j=0;j<count; j++) 
		{
			dest[j*3]=src[j*3+2];
			dest[j*3+1]=src[j*3+1];
			dest[j*3+2]=src[j*3];
		}
	}
}

void X11CopyGLES::createClientBuffer(Surface* surface)
{
    XPlatformSurface* nativeSurface = (XPlatformSurface*)surface->platform;
    glGenTextures(1,&nativeSurface->texture);
}

void X11CopyGLES::destroyClientBuffer(Surface* surface)
{
    XPlatformSurface* nativeSurface = (XPlatformSurface*) surface->platform;
    if (nativeSurface && nativeSurface->pixmap)
    {
          glDeleteTextures(1,&nativeSurface->texture);
          XFreePixmap(dpy, nativeSurface->pixmap);
    }
}
