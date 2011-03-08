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

#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES

#include "TextureBinders/X11TextureFromPixmap.h"
#include "X11/extensions/Xcomposite.h"
#include "Log.h"

const int X11TextureFromPixmap::pixmapAttribs[] = {
		GLX_TEXTURE_TARGET_EXT, GLX_TEXTURE_2D_EXT,
		GLX_TEXTURE_FORMAT_EXT, GLX_TEXTURE_FORMAT_RGBA_EXT, None
};

	X11TextureFromPixmap::X11TextureFromPixmap(Display* display, GLXFBConfig pixmapConfig) : dpy(display), pixmapConfig(pixmapConfig){
		glXBindTexImageEXT_func = (PFNGLXBINDTEXIMAGEEXTPROC) glXGetProcAddress((GLubyte *) "glXBindTexImageEXT");
		glXReleaseTexImageEXT_func = (PFNGLXRELEASETEXIMAGEEXTPROC)	glXGetProcAddress((GLubyte*) "glXReleaseTexImageEXT");

		if (!glXBindTexImageEXT_func || !glXReleaseTexImageEXT_func) {
			LOG_ERROR("X11TextureFromPixmap", "glXGetProcAddress failed");
		}
	};

	void X11TextureFromPixmap::bindSurfaceTexture(Surface*s){
		GLXPlatformSurface* nativeSurface = (GLXPlatformSurface*)s->platform;
		glXBindTexImageEXT_func(dpy, nativeSurface->glxPixmap, GLX_FRONT_LEFT_EXT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	void X11TextureFromPixmap::unbindSurfaceTexture(Surface*s){
		GLXPlatformSurface* nativeSurface = (GLXPlatformSurface*)s->platform;
		glXReleaseTexImageEXT_func(dpy, nativeSurface->glxPixmap, GLX_FRONT_LEFT_EXT);
	}

	void X11TextureFromPixmap::createClientBuffer(Surface* surface){
		LOG_DEBUG("X11TextureFromPixmap", "creating clientBuffer for window: " << surface->nativeHandle);
		GLXPlatformSurface* platformSurface = (GLXPlatformSurface*)surface->platform;
		Pixmap p = 0;
		LOG_DEBUG("X11TextureFromPixmap", "get X Pixmap");
		p= XCompositeNameWindowPixmap (dpy, surface->nativeHandle);
		if (p==0)
			LOG_ERROR("X11TextureFromPixmap", "didnt create pixmap!");
		XSync(dpy, 0);

		GLXPixmap pm = 0;
		LOG_DEBUG("X11TextureFromPixmap", "creating glx pixmap from xpixmap");
		pm = glXCreatePixmap(dpy, pixmapConfig, p, pixmapAttribs);
		if (pm ==0)
			LOG_DEBUG("X11TextureFromPixmap", "could not allocate glxpixmap for window");
		platformSurface->glxPixmap = pm;
		platformSurface->pixmap = p;
	}

	PlatformSurface* X11TextureFromPixmap::createPlatformSurface(Surface* s){
		return new GLXPlatformSurface(s);
	}

	void X11TextureFromPixmap::destroyClientBuffer(Surface* surface){
		LOG_ERROR("X11TextureFromPixmap", "destroying clientBuffer");
		GLXPlatformSurface* platformSurface = (GLXPlatformSurface*)surface->platform;
		LOG_DEBUG("X11TextureFromPixmap", "removing GLX Pixmap");
		if (platformSurface->glxPixmap){
			//glXReleaseTexImageEXT_func(dpy, x11surf->glxPixmap, GLX_FRONT_LEFT_EXT);
			glXDestroyPixmap(dpy,platformSurface->glxPixmap);
			platformSurface->glxPixmap = 0;
		}
		XSync(dpy, 0);

		LOG_DEBUG("X11TextureFromPixmap", "removing X Pixmap");
		if (platformSurface->pixmap){
			XFreePixmap(dpy,platformSurface->pixmap);
			platformSurface->pixmap = 0;
		}
	}
