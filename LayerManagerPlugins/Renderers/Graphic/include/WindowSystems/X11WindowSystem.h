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

#ifndef _X11WINDOWSYSTEM_H_
#define _X11WINDOWSYSTEM_H_

#include "WindowSystems/BaseWindowSystem.h"
#include "X11/Xlib.h"
#include "Surface.h"
#include "PlatformSurfaces/XPlatformSurface.h"
#include <X11/Xutil.h>
#include "Log.h"

typedef XVisualInfo* (*GetVisualInfoFunction)(Display *dpy);

class X11WindowSystem : public BaseWindowSystem {
public:
	X11WindowSystem(LayerList* layerlist, BaseGraphicSystem* graphicSystem,GetVisualInfoFunction func=X11WindowSystem::getDefaultVisual );
	bool start(int, int, const char*);
	void stop(){};

	static XVisualInfo *
	getDefaultVisual(Display *dpy);
private:
	static void* EventLoop(void * ptr);
	static int error (Display *dpy, XErrorEvent *ev);
	GetVisualInfoFunction getVisualFunc;

protected:
	Display* x11Display;
	bool initXServer();
	//void setDisplayMode();
	int windowWidth;
	int windowHeight;
	//	Window background;
	Window CompositorWindow;
	XVisualInfo* windowVis;
	static bool m_success;
	static bool m_initialized;

private:
	void Redraw();
	void OpenDisplayConnection();
	void checkForCompositeExtension();
	void createSurfaceForWindow(Window w);
	void configureSurfaceWindow(Window w);
	Surface* getSurfaceForWindow(Window w);
	void destroy_surface(Window w);
	void updateSurface(Surface* s, Window w, XPlatformSurface* x11surf);
	void MapWindow(Window w);
	void UnMapWindow(Window w);
	void NewWindow(Surface* s, Window w);
	void DestroyWindow(Window w);
	bool isWindowValid(Window w);
	bool CreatePixmapsForAllWindows();
	bool CreateCompositorWindow();
	void UnredirectSpecialWIndows(Window w);
	void printDebug(int posx,int posy);
	bool debugMode;
	int resolutionWidth;
	int resolutionHeight;
	static int composite_opcode;
	int composite_event, composite_error;
	int composite_major, composite_minor;
	static const char CompositorWindowTitle[];
};

#endif /* _X11WINDOWSYSTEM_H_ */
