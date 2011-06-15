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

#include "GraphicSystems/GLXGraphicsystem.h"
#include <string.h>
#include "TextureBinders/X11CopyGLX.h"
#include "TextureBinders/X11TextureFromPixmap.h"

#include "Bitmap.h"

GLXGraphicsystem::GLXGraphicsystem(int WindowWidth, int WindowHeight)
{
    LOG_DEBUG("GLXGraphicsystem", "creating GLXGraphicsystem");
    this->windowHeight = WindowHeight;
    this->windowWidth = WindowWidth;
#ifdef GLX_GRAPHICSYSTEM_FORCE_COPY
    m_forcecopy = true;
#else
    m_forcecopy = false;
#endif
}

GLXGraphicsystem::~GLXGraphicsystem()
{
    if (m_binder)
    {
        delete m_binder;
    }
}

XVisualInfo* GLXGraphicsystem::GetMatchingVisual(Display *dpy)
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
    if (!visinfo)
    {
        LOG_ERROR("GLXGraphicsystem", "Unable to find RGB, double-buffered visual");
        exit(1);
    }
    return visinfo;
}
bool GLXGraphicsystem::CheckConfigMask(Display *curDisplay,GLXFBConfig currentConfig, int attribute, int expectedValue)
{
     bool result = true;
    int returnedValue = 0;

    glXGetFBConfigAttrib(curDisplay,currentConfig,attribute,&returnedValue);
    if (!(returnedValue & expectedValue))
    {
        result = false;
    }
    return result;
}

bool GLXGraphicsystem::CheckConfigValue(Display *curDisplay,GLXFBConfig currentConfig, int attribute, int expectedValue)
{
     bool result = true;
    int returnedValue = 0;

    glXGetFBConfigAttrib(curDisplay,currentConfig,attribute,&returnedValue);
    if ((returnedValue != expectedValue))
    {
        result = false;
    }
    return result;
}


GLXFBConfig GLXGraphicsystem::GetMatchingPixmapConfig(Display *curDisplay)
{
    int neededMaskAttribute[] =
    {
        GLX_DRAWABLE_TYPE,GLX_PIXMAP_BIT,
        GLX_DRAWABLE_TYPE,GLX_WINDOW_BIT,
        GLX_BIND_TO_TEXTURE_TARGETS_EXT,GLX_TEXTURE_2D_BIT_EXT,
        None
    };
    int neededValueAttribute[] =
    {
        GLX_BUFFER_SIZE,32,
        GLX_ALPHA_SIZE,8,
        GLX_BIND_TO_TEXTURE_RGBA_EXT,True,
        None
    };
    LOG_DEBUG("GLXGraphicsystem", "Choose pixmap GL configuration");
    int screen = DefaultScreen(curDisplay);
    GLXFBConfig *currentFBconfigs;
    int i = 0;
    int j = 0;
    int nConfigs = 0;

    currentFBconfigs = glXGetFBConfigs(curDisplay, screen, &nConfigs);
    for (i = 0; i < nConfigs; i++)
    {
        GLXFBConfig config = currentFBconfigs[i];
        bool result = true;
        /* check first all mask values */
        j = 0;
        while ( neededMaskAttribute[j] != None && result == true )
        {
           result = CheckConfigMask(curDisplay,config, neededMaskAttribute[j], neededMaskAttribute[j+1]);
           j += 2;
        }
        /* no matching found in needed mask attribute, skip config take next */
        if (result == false )
        {
            continue;
        }
        /* check all fixed values */

        /* reset attribute counter */
        j = 0;
        /* check all fixed values */
        while ( neededValueAttribute[j] != None && result == true )
        {
           result = CheckConfigValue(curDisplay,config, neededValueAttribute[j], neededValueAttribute[j+1]);
           j += 2;
        }
        /* no matching found in needed fixed value attribute, skip config take next */

        if (result == false )
        {
            continue;
        }
        break;
    }

    if (i == nConfigs)
    {
        LOG_ERROR("GLXGraphicsystem", "Unable to find FBconfig for texturing");
        exit(1);
    }

    LOG_DEBUG("GLXGraphicsystem", "Done choosing GL Pixmap configuration");
    return currentFBconfigs[i];
}

bool GLXGraphicsystem::init(Display* x11Display, Window x11Window)
{
    LOG_DEBUG("GLXGraphicsystem", "init");
    x11disp = x11Display;
    window = x11Window;

    if (!x11disp)
    {
        LOG_ERROR("GLXGraphicsystem", "given display is null");
    }

    if (!window)
    {
        LOG_ERROR("GLXGraphicsystem", "given windowid is 0");
    }

    XVisualInfo* windowVis = GetMatchingVisual(x11disp);

    LOG_DEBUG("X11GLXRenderer", "Initialising opengl");
    GLXContext ctx = glXCreateContext(x11disp, windowVis, 0, GL_TRUE);
    if (!ctx)
    {
        LOG_ERROR("GLXGraphicsystem", "Couldn't create GLX context!");
        exit(1);
    }
    LOG_DEBUG("GLXGraphicsystem", "Make GLX Context current");
    glXMakeCurrent(x11disp, window, ctx);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);
    ITextureBinder * binder;
    const char *ext;
    ext = glXQueryExtensionsString(x11disp, 0);
    if (!strstr(ext, "GLX_EXT_texture_from_pixmap") || true == m_forcecopy )
    {
        LOG_DEBUG("GLXGraphicsystem", "GLX_EXT_texture_from_pixmap not supported or copy is forced! Fallback to copy!");
        binder = new X11CopyGLX(x11disp);
    }
    else
    {
        GLXFBConfig pixmapConfig = GetMatchingPixmapConfig(x11disp);
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

    /* Load Identity Matrix for each Layer */
    glLoadIdentity();

    /* set layer Transformations */
    const Rectangle& layerDestination = m_currentLayer->getDestinationRegion();
    // TODO: unused? const Rectangle& layerSource = m_currentLayer->getSourceRegion();

    glTranslatef(layerDestination.x, layerDestination.y, 0.0);
}

void GLXGraphicsystem::checkRenderLayer()
{
	SurfaceList surfaces = m_currentLayer->getAllSurfaces();

	m_currentLayer->damaged = false;

	if (!m_baseWindowSystem->m_damaged)
	{
		if (m_currentLayer->renderPropertyChanged)
		{
			m_currentLayer->damaged = true;
		}
		else if ((m_currentLayer)->visibility && (m_currentLayer)->opacity > 0.0)
		{
			for(std::list<Surface*>::const_iterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++)
			{
				if ((*currentS)->renderPropertyChanged)
				{
					m_currentLayer->damaged = true;
					break;
				}
				else if ((*currentS)->damaged && (*currentS)->visibility && (*currentS)->opacity>0.0f)
				{
					m_currentLayer->damaged = true;
					break;
				}
			}
		}
	}

	for(std::list<Surface*>::const_iterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++)
	{
		(*currentS)->damaged = false;
		(*currentS)->renderPropertyChanged = false;
	}

	m_currentLayer->renderPropertyChanged = false;

	if (m_currentLayer->damaged)
	{
		m_baseWindowSystem->m_damaged = true;
	}
}

void GLXGraphicsystem::renderLayer()
{
	SurfaceList surfaces = m_currentLayer->getAllSurfaces();
	for(std::list<Surface*>::const_iterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++)
	{
		if ((*currentS)->visibility && (*currentS)->opacity>0.0f)
		{
			Surface* currentSurface = (Surface*)*currentS;
			renderSurface(currentSurface);
		}
	}
}

void GLXGraphicsystem::endLayer()
{
    m_currentLayer = NULL;
}

void GLXGraphicsystem::renderSurface(Surface* currentSurface)
{
//    LOG_DEBUG("GLXGraphicsystem", "renderSurface " << currentSurface->getID() );
    glPushMatrix();
    m_binder->bindSurfaceTexture(currentSurface);
//    glPushMatrix();
    glColor4f(1.0f,1.0f,1.0f,currentSurface->opacity*(m_currentLayer)->opacity);
    glBegin(GL_QUADS);
    const Rectangle& src = currentSurface->getSourceRegion();
    const Rectangle& dest = currentSurface->getDestinationRegion();

//    LOG_DEBUG("GLXGraphicsystem","rendersurface: src" << src.x << " " << src.y << " " << src.width << " " << src.height );
//    LOG_DEBUG("GLXGraphicsystem","rendersurface: dest" << dest.x << " " << dest.y << " " << dest.width << " " << dest.height );
//    LOG_DEBUG("GLXGraphicsystem","orig: " << currentSurface->OriginalSourceWidth << " " << currentSurface->OriginalSourceHeight  );
//    LOG_DEBUG("GLXGraphicsystem","window: " << windowWidth << " " << windowHeight  );

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

