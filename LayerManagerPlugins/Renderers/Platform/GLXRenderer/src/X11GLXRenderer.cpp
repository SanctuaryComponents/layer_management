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

X11GLXRenderer::X11GLXRenderer(LayerList* layerlist) : BaseRenderer(layerlist), graphicSys(NULL){
	LOG_DEBUG("X11GLXRenderer", "creating X11GLXRenderer");
	graphicSys = new GLXGraphicsystem();
	m_windowSystem = new X11WindowSystem(layerlist, graphicSys, GLXGraphicsystem::ChooseWindowVisual);
};

X11GLXRenderer::~X11GLXRenderer() {
	delete m_windowSystem;
};

void X11GLXRenderer::doScreenShot(std::string fileToSave){
	graphicSys->doScreenShot(fileToSave);
}

extern "C" IRenderer* createX11Renderer(LayerList* layerlist) {
	return new X11GLXRenderer(layerlist);
}

extern "C" void destroyX11Renderer(X11GLXRenderer* p) {
	delete p;
}
