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

#include "X11GLESRenderer.h"
#include "ShaderProgramGLES.h"
#include "X11/Xlib.h"
#include "TextureBinders/X11CopyGLES.h"
#include "TextureBinders/X11EglImage.h"

X11GLESRenderer::X11GLESRenderer(LayerList* layerlist) : BaseRenderer(layerlist)
{
	LOG_DEBUG("X11GLESRenderer", "Creating Renderer");

};

bool X11GLESRenderer::start(int width, int height, const char* displayname){
	Display* nativeDisplayHandle = NULL;
	EGLDisplay eglDisplayhandle = NULL;
	ITextureBinder* binder = NULL;
	m_width = width;
	m_height = height;
	// create X11 windows, register as composite manager etc
	m_windowSystem = new X11WindowSystem(displayname, width, height, m_layerlist);
	m_graphicSystem = new GLESGraphicsystem(width,height, ShaderProgramGLES::createProgram);

	if ( !m_windowSystem->init(m_graphicSystem) ) goto fail;

	m_graphicSystem->setBaseWindowSystem(m_windowSystem);

	// create graphic context from window, init egl etc
	nativeDisplayHandle = m_windowSystem->getNativeDisplayHandle();

	LOG_DEBUG("X11GLESRenderer", "Got nativedisplay handle: " << nativeDisplayHandle << " from windowsystem");

	eglDisplayhandle = m_graphicSystem->getEGLDisplay();

#ifdef GLES_FORCE_COPY
	binder = new X11CopyGLES(eglDisplayhandle, nativeDisplayHandle);
#else
#ifdef EGL_NATIVE_PIXMAP_KHR
	binder = new X11EglImage(eglDisplayhandle, nativeDisplayHandle);
#else
	binder = new X11CopyGLES(eglDisplayhandle, nativeDisplayHandle);
#endif
#endif
	if ( (binder != NULL) && (nativeDisplayHandle != NULL) && (eglDisplayhandle!= NULL) )
	{
		m_graphicSystem->setTextureBinder(binder);

		if ( !m_windowSystem->start() )
		{
			goto fail;
		}
	} else {
		goto fail;
	}
	return true;
	fail:
	LOG_ERROR("X11GLESRenderer", "Initialization failed !");
	return false;
}

void X11GLESRenderer::stop(){
	m_windowSystem->stop();
}

void X11GLESRenderer::doScreenShot(std::string fileToSave){
	m_windowSystem->doScreenShot(fileToSave);
}

void X11GLESRenderer::doScreenShotOfLayer(std::string fileToSave,uint id){
	m_windowSystem->doScreenShotOfLayer(fileToSave,id);
}

void X11GLESRenderer::doScreenShotOfSurface(std::string fileToSave, uint id){
	m_windowSystem->doScreenShotOfSurface(fileToSave,id);
}

uint X11GLESRenderer::getNumberOfHardwareLayers(uint screenID){
	return 0;
	// TODO
}

uint* X11GLESRenderer::getScreenResolution(uint screenID){
	uint * resolution = new uint[2];
	resolution[0] = m_width;
	resolution[1] = m_height;
	return resolution;
}

uint* X11GLESRenderer::getScreenIDs(uint* length){
	Display* x11Display = m_windowSystem->getNativeDisplayHandle();
	if (!x11Display)
		return NULL;
	// Screens in X11 can be addresses/accessed by just the number - we must only know how many there are
	uint numberOfScreens = ScreenCount(x11Display);
	uint* screenIDS = new uint[numberOfScreens];
	for (int i = 0;i<numberOfScreens;i++){
		screenIDS[i] = i;
	}
	*length = numberOfScreens;
	return screenIDS;
}

void X11GLESRenderer::signalWindowSystemRedraw()
{
	m_windowSystem->signalRedrawEvent();
}

extern "C" BaseRenderer* createX11GLESRenderer(LayerList* layerlist){
    return new X11GLESRenderer(layerlist);
}

extern "C" void destroyX11GLESRenderer(X11GLESRenderer* p){
    delete p;
}
