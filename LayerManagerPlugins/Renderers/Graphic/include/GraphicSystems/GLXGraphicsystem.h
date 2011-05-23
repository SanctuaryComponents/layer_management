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

#ifndef _GLXGRAPHICSYSTEM_H_
#define _GLXGRAPHICSYSTEM_H_

#include "BaseGraphicSystem.h"
#include "PlatformSurfaces/GLXPlatformSurface.h"
#include "Log.h"

class GLXGraphicsystem : public BaseGraphicSystem<Display*, Window> {
public:
	GLXGraphicsystem( int windowWidth, int windowHeight);
	virtual ~GLXGraphicsystem();
	static XVisualInfo* GetMatchingVisual(Display *dpy);
	bool init(Display* x11Display, Window x11Window);

	void beginLayer(Layer* layer);
	void checkRenderLayer();
	void renderLayer();
	void endLayer();

	void clearBackground();
	void swapBuffers();
	void saveScreenShotOfFramebuffer(std::string fileToSave);
	GLXFBConfig GetMatchingPixmapConfig(Display *curDisplay);
	bool CheckConfigValue(Display *curDisplay,GLXFBConfig currentConfig, int attribute, int expectedValue);
	bool CheckConfigMask(Display *curDisplay,GLXFBConfig currentConfig, int attribute, int expectedValue);
	void renderSurface(Surface* currentSurface);

private:
	int windowWidth;
	int windowHeight;
	Display* x11disp;
	Window 	window;
	Layer*	m_currentLayer;
	bool 	m_forcecopy;
};

#endif /* _GLXGRAPHICSYSTEM_H_ */
