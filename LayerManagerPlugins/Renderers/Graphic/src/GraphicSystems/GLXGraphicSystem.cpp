/***************************************************************************
 *
 * Copyright 2010,2011 BMW Car IT GmbH
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

#include "GraphicSystems/GLXGraphicsystem.h"
#include <string.h>
#include "TextureBinders/X11CopyGLX.h"
#include "TextureBinders/X11TextureFromPixmap.h"

#include "Bitmap.h"

GLXGraphicsystem::GLXGraphicsystem( int WindowWidth,int WindowHeight)
{
	LOG_DEBUG("GLXGraphicsystem", "creating GLXGraphicsystem");
	this->windowHeight = WindowHeight;
	this->windowWidth = WindowWidth;
}

GLXGraphicsystem::~GLXGraphicsystem()
{
	if (NULL!=m_binder)
		delete m_binder;
}

XVisualInfo* GLXGraphicsystem::ChooseWindowVisual(Display *dpy)
{
	int screen = DefaultScreen(dpy);
	XVisualInfo *visinfo;
	int attribs[] = {
			GLX_RGBA,
			GLX_ALPHA_SIZE,8,
			GLX_RED_SIZE, 1,
			GLX_GREEN_SIZE, 1,
			GLX_BLUE_SIZE, 1,
			GLX_DEPTH_SIZE,8,
			GLX_BUFFER_SIZE,32,
			GLX_DOUBLEBUFFER,
			None
	};

	visinfo = glXChooseVisual(dpy, screen, attribs);
	if (!visinfo) {
		LOG_ERROR("GLXGraphicsystem", "Unable to find RGB, double-buffered visual");
		exit(1);
	}
	return visinfo;
}

GLXFBConfig GLXGraphicsystem::ChoosePixmapFBConfig(Display *display)
{
	LOG_DEBUG("GLXGraphicsystem", "Choose pixmap GL configuration");
	int screen = DefaultScreen(display);
	GLXFBConfig *fbconfigs;
	int i, nfbconfigs = 0, value;

	fbconfigs = glXGetFBConfigs(display, screen, &nfbconfigs);
	for (i = 0; i < nfbconfigs; i++) {

		glXGetFBConfigAttrib(display, fbconfigs[i], GLX_DRAWABLE_TYPE, &value);
		if (!(value & GLX_PIXMAP_BIT))
			continue;

		glXGetFBConfigAttrib(display, fbconfigs[i], GLX_DRAWABLE_TYPE, &value);
		if (!(value & GLX_WINDOW_BIT))
			continue;

		glXGetFBConfigAttrib(display, fbconfigs[i], GLX_BIND_TO_TEXTURE_TARGETS_EXT, &value);
		if (!(value & GLX_TEXTURE_2D_BIT_EXT))
			continue;

		glXGetFBConfigAttrib(display, fbconfigs[i], GLX_BUFFER_SIZE, &value);
		if (value != 32)
			continue;

		glXGetFBConfigAttrib(display, fbconfigs[i], GLX_ALPHA_SIZE, &value);
		if (value <8)
			continue;

		glXGetFBConfigAttrib(display, fbconfigs[i],  GLX_BIND_TO_TEXTURE_RGBA_EXT, &value);
		if (value == False) {
			continue;
		}

		break;
	}

	if (i == nfbconfigs) {
		LOG_ERROR("GLXGraphicsystem", "Unable to find FBconfig for texturing");
		exit(1);
	}

	LOG_DEBUG("GLXGraphicsystem", "Done choosing GL Pixmap configuration");
	return fbconfigs[i];
}

bool GLXGraphicsystem::init(Display* x11Display, Window x11Window)
{
	LOG_DEBUG("GLXGraphicsystem", "init");
	x11disp = x11Display;
	window = x11Window;

	if (!x11disp)
		LOG_ERROR("GLXGraphicsystem", "given display is null");

	if (!window)
		LOG_ERROR("GLXGraphicsystem", "given windowid is 0");
	XVisualInfo* windowVis = ChooseWindowVisual(x11disp);

	LOG_DEBUG("X11GLXRenderer", "Initialising opengl");
	GLXContext ctx = glXCreateContext(x11disp, windowVis, 0, GL_TRUE);
	if (!ctx) {
		LOG_ERROR("GLXGraphicsystem", "Couldn't create GLX context!");
		exit(1);
	}
	LOG_DEBUG("GLXGraphicsystem", "Make GLX Context current");
	glXMakeCurrent(x11disp, window, ctx);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_TEXTURE_2D);

	ITextureBinder * binder;
	const char *ext;
	ext = glXQueryExtensionsString(x11disp, 0);
	if (!strstr(ext, "GLX_EXT_texture_from_pixmap"))
	{
		LOG_WARNING("GLXGraphicsystem", "GLX_EXT_texture_from_pixmap not supported! Fallback to copy!");
		binder = new X11CopyGLX(x11disp);
	}
	else
	{
		GLXFBConfig pixmapConfig = GLXGraphicsystem::ChoosePixmapFBConfig(x11disp);
		binder = new X11TextureFromPixmap(x11disp,pixmapConfig);
	}

	setTextureBinder(binder);

	LOG_DEBUG("GLXGraphicsystem", "Initialised");
}


void GLXGraphicsystem::clearBackground()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void GLXGraphicsystem::swapBuffers()
{
	glXSwapBuffers(x11disp, window);
}

void GLXGraphicsystem::beginLayer(Layer* currentLayer)
{
	m_currentLayer = currentLayer;
	// TODO layer destination / source
}

void GLXGraphicsystem::renderLayer()
{
	if ((m_currentLayer)->visibility && (m_currentLayer)->opacity > 0.0)
	{
		std::list<Surface*> surfaces = m_currentLayer->surfaces;
		for(std::list<Surface*>::const_iterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++)
		{
			if ((*currentS)->visibility && (*currentS)->opacity>0.0f)
			{
				Surface* currentSurface = (Surface*)*currentS;
				m_baseWindowSystem->allocatePlatformSurface(currentSurface);
				renderSurface(currentSurface);
			}
		}
	}
}

void GLXGraphicsystem::endLayer()
{
	m_currentLayer = NULL;
}

void GLXGraphicsystem::renderSurface(Surface* currentSurface)
{
	m_binder->bindSurfaceTexture(currentSurface);
	glPushMatrix();
	glColor4f(1.0f,1.0f,1.0f,currentSurface->opacity*(m_currentLayer)->opacity);
	glBegin(GL_QUADS);
	const Rectangle& src = currentSurface->getSourceRegion();
	const Rectangle& dest = currentSurface->getDestinationRegion();

//	LOG_DEBUG("GLXGraphicsystem","rendersurface: src" << src.x << " " << src.y << " " << src.width << " " << src.height );
//	LOG_DEBUG("GLXGraphicsystem","rendersurface: dest" << dest.x << " " << dest.y << " " << dest.width << " " << dest.height );
//	LOG_DEBUG("GLXGraphicsystem","orig: " << currentSurface->OriginalSourceWidth << " " << currentSurface->OriginalSourceHeight  );
//	LOG_DEBUG("GLXGraphicsystem","window" << windowWidth << " " << windowHeight  );

	//bottom left
	glTexCoord2d((float)src.x/currentSurface->OriginalSourceWidth, (float)(src.y+src.height)/currentSurface->OriginalSourceHeight);
	glVertex2d((float)dest.x/windowWidth*2-1,  1-(float)(dest.y+dest.height)/windowHeight*2);

	// bottom right
	glTexCoord2f((float)(src.x + src.width)/currentSurface->OriginalSourceWidth, (float)(src.y+src.height)/currentSurface->OriginalSourceHeight);
	glVertex2d( (float)(dest.x+dest.width)/windowWidth*2-1, 1-(float)(dest.y+dest.height)/windowHeight*2);

	// top right
	glTexCoord2f((float)(src.x + src.width)/currentSurface->OriginalSourceWidth, (float)src.y/currentSurface->OriginalSourceHeight);
	glVertex2d((float)(dest.x+dest.width)/windowWidth*2-1, 1-(float)dest.y/windowHeight*2);

	// top left
	glTexCoord2f((float)src.x/currentSurface->OriginalSourceWidth, (float)src.y/currentSurface->OriginalSourceHeight);
	glVertex2d((float)dest.x/windowWidth*2-1 ,  1-(float)dest.y/windowHeight*2);
	glEnd();

	m_binder->unbindSurfaceTexture(currentSurface);
	glPopMatrix();
}

void GLXGraphicsystem::saveScreenShotOfFramebuffer(std::string fileToSave)
{
	{
		LOG_DEBUG("GLXGraphicsystem","taking screenshot and saving it to:" << fileToSave);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT,viewport); // x,y,width,height

		int WINDOW_WIDTH= viewport[2];
		int WINDOW_HEIGHT= viewport[3];
		LOG_DEBUG("GLXGraphicsystem","Screenshot: " << WINDOW_WIDTH << " * " << WINDOW_HEIGHT);
		char *buffer = (char *)malloc(WINDOW_WIDTH * WINDOW_HEIGHT * 3 * sizeof(unsigned char));
		glReadPixels(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,GL_BGR,GL_UNSIGNED_BYTE, buffer);

		writeBitmap(fileToSave,buffer,WINDOW_WIDTH,WINDOW_HEIGHT);
		free(buffer);
		LOG_DEBUG("GLXGraphicsystem","done taking screenshot");
	}
}

