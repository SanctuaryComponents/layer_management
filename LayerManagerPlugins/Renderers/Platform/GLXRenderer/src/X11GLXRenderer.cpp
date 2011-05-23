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

#include "X11GLXRenderer.h"
#include "GraphicSystems/GLXGraphicsystem.h"
#include "WindowSystems/X11WindowSystem.h"
#include "Log.h"
#include <string.h>

X11GLXRenderer::X11GLXRenderer(LayerList* layerlist) : BaseRenderer(layerlist){
};

bool X11GLXRenderer::start(int width, int height, const char* displayname){
	LOG_DEBUG("X11GLXRenderer", "creating X11GLXRenderer");
	m_width = width;
	m_height = height;

	// create X11 windows, register as composite manager etc
	m_windowSystem  = new X11WindowSystem(displayname, width, height, m_layerlist, GLXGraphicsystem::GetMatchingVisual);
	m_graphicSystem = new GLXGraphicsystem(width, height);
	m_graphicSystem->setBaseWindowSystem(m_windowSystem);

	LOG_DEBUG("X11GLXRenderer", "init windowsystem");
	m_windowSystem->init(m_graphicSystem );

	Display* x11Display = m_windowSystem->getNativeDisplayHandle();
	if (NULL==x11Display)
		LOG_ERROR("GLXGraphicsystem", "Display from windowsystem is null");

	m_windowSystem->start();
}

void X11GLXRenderer::stop(){
	m_windowSystem->stop();
}

X11GLXRenderer::~X11GLXRenderer() {
	delete m_windowSystem;
};

void X11GLXRenderer::doScreenShot(std::string fileToSave){
	m_windowSystem->doScreenShot(fileToSave);
}

void X11GLXRenderer::doScreenShotOfLayer(std::string fileToSave,uint id){
	m_windowSystem->doScreenShotOfLayer(fileToSave,id);
}

void X11GLXRenderer::doScreenShotOfSurface(std::string fileToSave, uint id){
	m_windowSystem->doScreenShotOfSurface(fileToSave,id);
}

uint X11GLXRenderer::getNumberOfHardwareLayers(uint screenID){
	return 0;
	// TODO
}

uint* X11GLXRenderer::getScreenResolution(uint screenID){
	uint * resolution = new uint[2];
	resolution[0] = m_width;
	resolution[1] = m_height;
	return resolution;
}

uint* X11GLXRenderer::getScreenIDs(uint* length){
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

void X11GLXRenderer::signalWindowSystemRedraw()
{
	m_windowSystem->signalRedrawEvent();
}

extern "C" IRenderer* createX11GLXRenderer(LayerList* layerlist) {
	return new X11GLXRenderer(layerlist);
}

extern "C" void destroyX11GLXRenderer(X11GLXRenderer* p) {
	delete p;
}
