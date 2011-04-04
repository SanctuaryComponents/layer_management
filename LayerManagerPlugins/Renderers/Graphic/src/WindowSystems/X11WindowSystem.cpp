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

#include "WindowSystems/X11WindowSystem.h"
#include "Log.h"
#include "Layer.h"
#include <time.h>
#include <sys/time.h>
#include <X11/extensions/Xcomposite.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iomanip>

int		X11WindowSystem::composite_opcode;
const char X11WindowSystem::CompositorWindowTitle[] = "LayerManager";

X11WindowSystem::X11WindowSystem(const char* displayname, int width, int height, LayerList* layerlist,GetVisualInfoFunction func) : BaseWindowSystem(layerlist), displayname(displayname), getVisualFunc(func), windowWidth(width), windowHeight(height),resolutionWidth(width),resolutionHeight(height),debugMode(false), m_initialized(false), m_success(false), takeScreenshot(ScreenShotNone){
		LOG_DEBUG("X11WindowSystem", "creating X11WindowSystem");

		// init and take mutex, so windowsystem only does init phase until mutex is released again
		pthread_mutex_init(&run_lock, NULL);
		pthread_mutex_lock(&run_lock);
	};

X11WindowSystem::~X11WindowSystem(){
}

XVisualInfo* X11WindowSystem::getDefaultVisual(Display *dpy)
	{
		XVisualInfo* windowVis = new XVisualInfo();
		windowVis->depth = DefaultDepth(dpy, 0);;
		XMatchVisualInfo( dpy, 0, windowVis->depth, TrueColor, windowVis);
		if (!windowVis)
		{
			LOG_ERROR("X11WindowSystem", "Error: Unable to acquire visual\n");
		}
		return windowVis;
	};

bool X11WindowSystem::OpenDisplayConnection(){
	x11Display = XOpenDisplay(":0");
	if (!x11Display)
	{
		LOG_ERROR("X11WindowSystem", "Couldn't open default display!");
		return false;
	}
	LOG_DEBUG("X11WindowSystem", "Display connection: " << x11Display);
	return true;
}

bool X11WindowSystem::checkForCompositeExtension(){
	if (!XQueryExtension (x11Display, COMPOSITE_NAME, &composite_opcode,
				&composite_event, &composite_error))
	{
		LOG_ERROR("X11WindowSystem", "No composite extension");
		return false;
	}
	XCompositeQueryVersion (x11Display, &composite_major, &composite_minor);
	LOG_DEBUG("X11WindowSystem", "Found composite extension: composite opcode: " << composite_opcode);
	LOG_DEBUG("X11WindowSystem", "composite_major: " << composite_major);
	LOG_DEBUG("X11WindowSystem", "composite_minor: " << composite_minor);
	return true;
}

void X11WindowSystem::printDebug(int posx,int posy){
	// print stuff about layerlist
	std::stringstream debugmessage;
	debugmessage << "Layer:  ID |   X  |   Y  |   W  |   H  | Al. \n";
	// loop the layers
	std::list<Layer*> list = layerlist->getCurrentRenderOrder();
	for(std::list<Layer*>::const_iterator currentLayer = list.begin();
	currentLayer != list.end(); currentLayer++)
	{
		Rectangle dest = (*currentLayer)->getDestinationRegion();
		debugmessage << "            " << std::setw(4) << (*currentLayer)->getID() << " " << std::setw(3) << dest.x << " " << std::setw(3) << dest.y << " " << std::setw(3) << dest.width << " " << std::setw(3) << dest.height << " " << std::setw(3) << (*currentLayer)->opacity << "\n";

		debugmessage << "    Surface:  ID |Al.|  SVP: X |  Y |  W |  H     DVP:  X |  Y |  W |  H \n";
		// loop the surfaces of within each layer
		for(std::list<Surface*>::iterator current = (*currentLayer)->surfaces.begin();
		current != (*currentLayer)->surfaces.end(); current++)
		{
			Rectangle src = (*current)->getSourceRegion();
			Rectangle dest = (*current)->getDestinationRegion();
			debugmessage << "                        " << std::setw(4) << (*current)->getID() << " " << std::setprecision(3) << (*current)->opacity<< " " << std::setw(3) << src.x << " " << std::setw(3) << src.y << " " << std::setw(3) << src.width << " " << std::setw(3) << src.height << " " << std::setw(3) << dest.x << " " << std::setw(3) << dest.y << " " << std::setw(3) << dest.width << " " << std::setw(3) << dest.height  << "\n";
		}
	}
	LOG_INFO("X11WindowSystem",debugmessage.str());
}

Window * getListOfAllTopLevelWindows (Display *disp, unsigned int *len) {
	LOG_DEBUG("X11WindowSystem", "Getting list of all windows");
	Window *children;
	Window	root_return, parent_return;
	Window  root = XDefaultRootWindow(disp);
	XQueryTree (disp, root, &root_return, &parent_return, &children, len);
	return children;
}

bool X11WindowSystem::isWindowValid(Window w){
	// skip our own two windows
	if (w != CompositorWindow)
		return true;
	else
		return false;
}

Surface* X11WindowSystem::getSurfaceForWindow(Window w){
	LOG_DEBUG("X11WindowSystem", "finding surface for window " << w);
	// go though all surfaces
	const std::map<unsigned int,Surface*> surfaces = layerlist->getAllSurfaces();
	for(std::map<unsigned int, Surface*>::const_iterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++){
		Surface* currentSurface = (*currentS).second;
		LOG_DEBUG("X11WindowSystem", "CurrentSurface surface for window " << currentSurface->getID());
                LOG_DEBUG("X11WindowSystem", "CurrentSurface nativeHandle " << currentSurface->nativeHandle);
                if (currentSurface->nativeHandle == w){
                        LOG_DEBUG("X11WindowSystem", "surface " << currentSurface->getID() << " corresponds to window" << w);
                        return currentSurface;
                }
	}
	LOG_DEBUG("X11WindowSystem", "could not find surface for window " << w);
	return NULL;
}


void X11WindowSystem::configureSurfaceWindow(Window w){
	int status = 0;
	if (isWindowValid(w)){
		LOG_DEBUG("X11WindowSystem", "Updating window " << w);
		UnMapWindow(w);
		MapWindow(w);
		XWindowAttributes att;
		status = XGetWindowAttributes (x11Display, w, &att);
		int winWidth = att.width;
		int winHeight = att.height;

		Surface*s = getSurfaceForWindow(w);
		if (s==NULL){
			LOG_ERROR("X11WindowSystem", "surface emtpy");
			return;
		}
		if (s->platform==NULL){
			LOG_ERROR("X11WindowSystem", "platform surface empty");
			return;
		}

		LOG_DEBUG("X11WindowSystem", "Updating surface " << s->getID());

		s->OriginalSourceHeight =winHeight;
		s->OriginalSourceWidth = winWidth;
		s->setDestinationRegion(Rectangle(0,0,winWidth,winHeight));
		s->setSourceRegion(Rectangle(0,0,winWidth,winHeight));

		LOG_DEBUG("X11WindowSystem", "Done Updating window " << w);
	}
}

void X11WindowSystem::MapWindow(Window window){
	int status = 0;
	if (isWindowValid(window))
	{
		XWindowAttributes att;
		status = XGetWindowAttributes (x11Display, window, &att);
		if (att.map_state == IsViewable && att.override_redirect==0){
			LOG_DEBUG("X11WindowSystem", "Mapping window " << window);
			Surface* surface = getSurfaceForWindow(window);
			if (surface==NULL){
				LOG_ERROR("X11WindowSystem", "surface emtpy");
				return;
			}
			if (surface->platform==NULL){
				LOG_ERROR("X11WindowSystem", "platform surface empty");
				return;
			}

			XPlatformSurface* x11surf = (XPlatformSurface*)surface->platform;
			if (x11surf->isMapped){
				LOG_ERROR("X11WindowSystem", "already mapped!");
				return;
			}
			x11surf->isMapped = true;


			LOG_DEBUG("X11WindowSystem", "getting pixmap for window");
			LOG_DEBUG("X11WindowSystem", "window width: " << att.width);
			LOG_DEBUG("X11WindowSystem", "window height: " << att.height);
			LOG_DEBUG("X11WindowSystem", "map state: " << att.map_state);
			LOG_DEBUG("X11WindowSystem", "window x: " << att.x);
			LOG_DEBUG("X11WindowSystem", "window backing: " << att.backing_pixel);
			LOG_DEBUG("X11WindowSystem", "window save under: " << att.save_under);
			LOG_DEBUG("X11WindowSystem", "window orride: " << att.override_redirect);
			LOG_DEBUG("X11WindowSystem", "parent/root: " << att.root);
			LOG_DEBUG("X11WindowSystem", "root window: " << DefaultRootWindow(x11Display));

			int winWidth = att.width;
			int winHeight = att.height;

			surface->OriginalSourceHeight = winHeight;
			surface->OriginalSourceWidth = winWidth;
			surface->setDestinationRegion(Rectangle(0,0,winWidth,winHeight));
			surface->setSourceRegion(Rectangle(0,0,winWidth,winHeight));

			graphicSystem->m_binder->createClientBuffer(surface);
			XSync(x11Display, 0);

			surface->visibility = true;
			surface->setOpacity(1.0);
			LOG_DEBUG("X11WindowSystem", "Mapping Surface " << surface->getID());
			LOG_DEBUG("X11WindowSystem", "Done mapping");
		}
	}
	LOG_INFO("X11WindowSystem","mapping end");
}

void X11WindowSystem::UnMapWindow(Window window){
	if (isWindowValid(window)){
		LOG_DEBUG("X11WindowSystem", "Unmapping window " << window);
		Surface* surface = getSurfaceForWindow(window);
		if (surface==NULL){
			LOG_ERROR("X11WindowSystem", "surface emtpy");
			return;
		}
		// set invisible first, so it wont be used for rendering anymore
		surface->visibility = false;
		if (surface->platform==NULL){
			LOG_ERROR("X11WindowSystem", "platform surface empty");
			return;
		}
		XPlatformSurface* x11surf = (XPlatformSurface*)surface->platform;
		LOG_DEBUG("X11WindowSystem", "Unmapping surface " << surface->getID());
		x11surf->isMapped = false;


		LOG_DEBUG("X11WindowSystem", "Destroying ClientBuffer");
		graphicSystem->m_binder->destroyClientBuffer(surface);
		XSync(x11Display, 0);

		LOG_DEBUG("X11WindowSystem", "Removing X Pixmap");

		XSync(x11Display, 0);
	}
	LOG_DEBUG("X11WindowSystem", "Unmap finished");
}

void X11WindowSystem::NewWindow(Surface* surface, Window window)
{
	if (isWindowValid(window)){
		LOG_DEBUG("X11WindowSystem", "Creating Surface for new window " << window);
		// get the windows attributes
		XWindowAttributes att;
		int status = XGetWindowAttributes (x11Display, window, &att);
		LOG_DEBUG("X11WindowSystem", "Got window attrbutes");
		char* name;
		status = XFetchName(x11Display, window, &name);
		LOG_DEBUG("X11WindowSystem", "Got window name");
		if (status >= Success)
		{
			LOG_DEBUG("X11WindowSystem", "Found window: " << window << "  " << name);
			char GuiTitle[]  = "Layermanager Remote GUI\0";
			if (name != NULL && strcmp(name,GuiTitle)==0){
				LOG_DEBUG("X11WindowSystem", "Found gui window: repositioning it");
				XCompositeUnredirectWindow(x11Display,window,CompositeRedirectManual);
//XLowerWindow(x11Display,window);
				XMoveWindow(x11Display, window, 50,	500);
				XMapRaised(x11Display, window);
			}
		}else {
			LOG_DEBUG("X11WindowSystem", "Error fetching window name");
		}
		XFree(name);
XLowerWindow(x11Display,window);
//XMoveWindow(x11Display,window,-1000,-1000);
		//		Surface* s = layerlist->createSurface();

		surface->nativeHandle = window;
		XPlatformSurface * platformSurface = (XPlatformSurface*)graphicSystem->m_binder->createPlatformSurface(surface);
		platformSurface->isMapped = false;

		LOG_DEBUG("X11WindowSystem", "Created native Surface for X11 Window id " << window);

		surface->nativeHandle = window;
		surface->platform = platformSurface;
		surface->visibility = false;

		int winWidth = att.width;
		int winHeight = att.height;

		surface->OriginalSourceHeight =winHeight;
		surface->OriginalSourceWidth = winWidth;
		surface->setDestinationRegion(Rectangle(0,0,winWidth,winHeight));
		surface->setSourceRegion(Rectangle(0,0,winWidth,winHeight));
		LOG_DEBUG("X11WindowSystem", "orig width " << surface->OriginalSourceWidth);
		LOG_DEBUG("X11WindowSystem", "orig heigth " << surface->OriginalSourceHeight);
	}else{
		LOG_DEBUG("X11WindowSystem", "skipping window");
	}
	LOG_DEBUG("X11WindowSystem", "created the new surface");
}

void X11WindowSystem::DestroyWindow(Window window){
	if (isWindowValid(window))
	  {
		LOG_DEBUG("X11WindowSystem", "Destroying Surface for window " << window);
		Surface* surface = getSurfaceForWindow(window);
		if (surface==NULL){
			LOG_ERROR("X11WindowSystem", "surface empty");
			return;
		}
		graphicSystem->m_binder->destroyClientBuffer(surface);
		UnMapWindow(window);
                LOG_DEBUG("X11WindowSystem", "Removed Surface " << surface->getID());
		layerlist->lockList();
                layerlist->removeSurface(surface);
                layerlist->unlockList();
	  }
}

bool
X11WindowSystem::CreatePixmapsForAllWindows()
{
	bool result = true;
	LOG_DEBUG("X11WindowSystem", "redirecting all windows");
	Window root = RootWindow(x11Display, 0);
//	XGrabServer (x11Display);
//	unsigned int numberOfWindows = 0;
//	Window *children = getListOfAllTopLevelWindows(x11Display,&numberOfWindows);

//	LOG_DEBUG("X11WindowSystem", "Found " << numberOfWindows << " windows");
	XCompositeRedirectSubwindows(x11Display,root,CompositeRedirectManual);

/*	for (unsigned int i=0;i< (numberOfWindows-1);i++)
	{
		Window w = (Window) children[i];
		NewWindow(w);
		MapWindow(w);
	} */
//	XFree(children);
//	XUngrabServer (x11Display);
//	XSync(x11Display, 0);

	return result;
}

bool X11WindowSystem::CreateCompositorWindow()
{
	LOG_DEBUG("X11WindowSystem", "Get root window");
	bool result = true;
	Window root = XDefaultRootWindow(x11Display);

	LOG_DEBUG("X11WindowSystem", "Get default screen");
	// draw a black background the full size of the resolution
	long x11Screen = XDefaultScreen( x11Display );

	LOG_DEBUG("X11WindowSystem", "Creating Compositor Window");
	// create the actual content window
//	CompositorWindow = XCreateSimpleWindow(  x11Display, root, 0, 0, windowWidth, windowHeight,
//											 0, BlackPixel(x11Display, x11Screen), WhitePixel(x11Display, x11Screen));
		XSetWindowAttributes attr;
		// draw a black background the full size of the resolution
		attr.override_redirect = True;
		attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask;
		attr.background_pixel = 0;
		attr.border_pixel = 0;
		windowVis = getVisualFunc(x11Display);
		attr.colormap = XCreateColormap(x11Display, root, windowVis->visual, AllocNone);

		attr.override_redirect = True;

	//	Window overlaywindow = XCompositeGetOverlayWindow(x11Display,root);

		CompositorWindow = XCreateWindow(x11Display, root, 0, 0, windowWidth, windowHeight,
				0, windowVis->depth, InputOutput,
				windowVis->visual, CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect, &attr);

	//	Screen* s = ScreenOfDisplay( x11Display, 0 );
	//	CompositorWindow = XCreateSimpleWindow( x11Display,
	//	                                                         RootWindowOfScreen(s),
	//	                                                         0, 0, windowWidth, windowHeight, 0,
	//	                                                         BlackPixelOfScreen(s),
	//	                                                         WhitePixelOfScreen(s) );
	//
	if (!CompositorWindow)
	{
		LOG_ERROR("X11WindowSystem", "Could not create window");
		return false;
	}
	LOG_DEBUG("X11WindowSystem", "Created the Compositor Window");
	XSelectInput (x11Display, root,
			SubstructureNotifyMask|
			ExposureMask|
			StructureNotifyMask|
			PropertyChangeMask);

	LOG_DEBUG("X11WindowSystem", "Created the window");
	XSizeHints sizehints;
	sizehints.width  = windowWidth;
	sizehints.height = windowHeight;
	sizehints.flags = USSize;
	XSetNormalHints(x11Display, CompositorWindow, &sizehints);
	XSetStandardProperties(x11Display, CompositorWindow, CompositorWindowTitle, CompositorWindowTitle,
			None, (char **)NULL, 0, &sizehints);
	XMapRaised(x11Display, CompositorWindow);
	XFlush(x11Display);
	return result;
}

float			TimeCounter, LastFrameTimeCounter, DT, prevTime = 0.0, FPS;
struct timeval		tv, tv0;
int			Frame = 1, FramesPerFPS;

void UpdateTimeCounter() {
	LastFrameTimeCounter = TimeCounter;
	gettimeofday(&tv, NULL);
	TimeCounter = (float)(tv.tv_sec-tv0.tv_sec) + 0.000001*((float)(tv.tv_usec-tv0.tv_usec));
	DT = TimeCounter - LastFrameTimeCounter; }

void CalculateFPS() {
	Frame ++;

	if((Frame%FramesPerFPS) == 0) {
		FPS = ((float)(FramesPerFPS)) / (TimeCounter-prevTime);
		prevTime = TimeCounter; } }

void
X11WindowSystem::RedrawAllLayers()
{
	std::list<Layer*> layers = layerlist->getCurrentRenderOrder();
	for(std::list<Layer*>::const_iterator current = layers.begin(); current != layers.end(); current++)
	  {
		Layer* currentLayer = (Layer*)*current;
		graphicSystem->beginLayer(currentLayer);
		graphicSystem->renderLayer();
		graphicSystem->endLayer();
	  }
}

void
X11WindowSystem::Redraw()
{
	// draw all the layers
	graphicSystem->clearBackground();
	/*LOG_INFO("X11WindowSystem","Locking List");*/
	layerlist->lockList();

	// check if we are supposed to take screenshot
	if (takeScreenshot!=ScreenShotNone){
		if (takeScreenshot==ScreenshotOfDisplay){
		LOG_DEBUG("X11WindowSystem", "Taking screenshot");
			RedrawAllLayers();
		}else if(takeScreenshot==ScreenshotOfLayer){
			LOG_DEBUG("X11WindowSystem", "Taking screenshot of layer");
			Layer* currentLayer = layerlist->getLayer(screenShotID);
			if (currentLayer!=NULL){
				graphicSystem->beginLayer(currentLayer);
				graphicSystem->renderLayer();
				graphicSystem->endLayer();
			}
		}else if(takeScreenshot==ScreenshotOfSurface){
			LOG_DEBUG("X11WindowSystem", "Taking screenshot of surface");
			Surface* currentSurface = layerlist->getSurface(screenShotID);
			if (NULL!=currentSurface){
				Layer* l = layerlist->createLayer(GraphicalObject::INVALID_ID);
				l->setOpacity(1.0);
				l->setDestinationRegion(currentSurface->getDestinationRegion());
				l->setSourceRegion(currentSurface->getSourceRegion());
				graphicSystem->beginLayer(l);
				graphicSystem->renderSurface(currentSurface);
				graphicSystem->endLayer();
				layerlist->removeLayer(l);
				// layer is deleted in removeLayer.
			}else{
				LOG_ERROR("X11WindowSystem", "Could not take screenshot of non existing surface");
			}
		}

		graphicSystem->saveScreenShotOfFramebuffer(screenShotFile);
//		graphicSystem->swapBuffers();
		takeScreenshot = ScreenShotNone;
		LOG_DEBUG("X11WindowSystem", "Done taking screenshot");
	}

	RedrawAllLayers();

	if (debugMode)
	{
		printDebug(200,windowHeight-40);
	}
	UpdateTimeCounter();
	CalculateFPS();
	char floatStringBuffer[256];
	sprintf(floatStringBuffer, "FPS: %f", FPS);
	if ((Frame % 1000 ) == 0)
	{
		LOG_DEBUG("X11WindowSystem",floatStringBuffer);
	}
       layerlist->unlockList();
       /*LOG_INFO("X11WindowSystem","UnLocking List");*/
       graphicSystem->swapBuffers();
}

int
X11WindowSystem::error (Display *dpy, XErrorEvent *ev)
{
	const char    *name = NULL;
	static char buffer[256];

	if (ev->request_code == composite_opcode &&
			ev->minor_code == X_CompositeRedirectWindow)
	{
		LOG_ERROR("X11WindowSystem", "Maybe another composite manager is already running");
	}

	if (name == NULL)
	{
		buffer[0] = '\0';
		XGetErrorText (dpy, ev->error_code, buffer, sizeof (buffer));
		name = buffer;
	}
	name = (strlen (name) > 0) ? name : "unknown";
	LOG_ERROR("X11WindowSystem", "X Error: " << (int)ev->error_code << " " << name << " request : " << (int)ev->request_code << " minor: " <<  (int)ev->minor_code << " serial: " << (int)ev->serial);
	return 0;
}

bool X11WindowSystem::initXServer()
{
	LOG_DEBUG("X11WindowSystem", "Initialising XServer connection");
	bool result = true;

	//setDisplayMode();
	if ( !CreateCompositorWindow() )
	{
		LOG_ERROR("X11WindowSystem", "Compositor Window creation failed " );
		return false;
	}

	LOG_DEBUG("X11WindowSystem", "Compositor Window ID: " << CompositorWindow);

	CreatePixmapsForAllWindows();
	//unredirect our window
#ifdef FULLSCREEN
	XCompositeUnredirectWindow(x11Display,background,CompositeRedirectManual);
#endif
	XCompositeUnredirectWindow(x11Display,CompositorWindow,CompositeRedirectManual);

	LOG_DEBUG("X11WindowSystem", "Initialised XServer connection");
	return result;
}

void* X11WindowSystem::EventLoop(void * ptr)
{
	// INITALIZATION
	LOG_DEBUG("X11WindowSystem", "Enter thread");

	bool status = true;

	X11WindowSystem *windowsys = (X11WindowSystem *) ptr;
	XSetErrorHandler(error);
	// init own stuff
	LOG_DEBUG("X11WindowSystem", "open display connection");
	status &= windowsys->OpenDisplayConnection();

	LOG_DEBUG("X11WindowSystem", "check for composite extension");
	status &= windowsys->checkForCompositeExtension();

	LOG_DEBUG("X11WindowSystem", "init xserver");
	status &= windowsys->initXServer();

	status &= windowsys->graphicSystem->init(windowsys->x11Display,windowsys->CompositorWindow);

	windowsys->m_success = status;
	windowsys->m_initialized = true;

	// Done with init, wait for lock to actually run (ie start/stop method called)
	pthread_mutex_lock(&windowsys->run_lock);

	LOG_DEBUG("X11WindowSystem", "Starting Event loop");
	Layer* defaultLayer;

	// run the main event loop while rendering
	gettimeofday(&tv0, NULL);
	FramesPerFPS = 30;
	if (windowsys->debugMode)
	{
		defaultLayer = windowsys->layerlist->createLayer(0);
		defaultLayer->setOpacity(1.0);
		defaultLayer->setDestinationRegion(Rectangle(0,0,windowsys->resolutionWidth,windowsys->resolutionHeight));
		defaultLayer->setSourceRegion(Rectangle(0,0,windowsys->resolutionWidth,windowsys->resolutionHeight));
		windowsys->layerlist->getCurrentRenderOrder().push_back(defaultLayer);
	}
	LOG_DEBUG("X11WindowSystem", "Enter render loop");
	while (windowsys->m_running)
	{
		if ( XPending(windowsys->x11Display) > 0) {
			XEvent event;
			XNextEvent(windowsys->x11Display, &event);
			switch (event.type) {
			case CreateNotify:
			{
				if (windowsys->debugMode)
				{
					LOG_DEBUG("X11WindowSystem", "CreateNotify Event");
					Surface* s = windowsys->layerlist->createSurface(0);
					s->setOpacity(1.0);
					windowsys->NewWindow(s, event.xcreatewindow.window);
					defaultLayer->addSurface(s);
				}
				break;
			}
			case ConfigureNotify:
				LOG_DEBUG("X11WindowSystem", "Configure notify Event");
				windowsys->configureSurfaceWindow( event.xconfigure.window);
				break;

			case DestroyNotify:
				LOG_DEBUG("X11WindowSystem", "Destroy  Event");
				windowsys->DestroyWindow(event.xdestroywindow.window);
				break;
			case Expose:
				LOG_DEBUG("X11WindowSystem", "Expose Event");
				windowsys->Redraw();
				break;
			case ButtonPress:
				LOG_DEBUG("X11WindowSystem", "Button press Event");
				//running = False;
				break;
			case MapNotify:
				LOG_DEBUG("X11WindowSystem", "Map Event");
				windowsys->MapWindow(event.xmap.window);
				break;
			case UnmapNotify:
				LOG_DEBUG("X11WindowSystem", "Unmap Event");
				windowsys->UnMapWindow(event.xunmap.window);
				break;
			case ReparentNotify:
				LOG_DEBUG("X11WindowSystem", "Reparent Event");
				//			 if (event.xreparent.parent == root)
				//				 renderer->NewWindow(event.xreparent.window);
				//			 else
				//				 renderer->DestroyWindow(event.xreparent.window);
				break;

			default:
				; /*no-op*/
			}
		}
		windowsys->Redraw();
	}
	windowsys->cleanup();
	LOG_INFO("X11WindowSystem", "Renderer thread finished");
	return NULL;
}

void X11WindowSystem::cleanup(){
	LOG_INFO("X11WindowSystem", "Cleanup");
	Window root = RootWindow(x11Display, 0);
	XCompositeUnredirectSubwindows(x11Display,root,CompositeRedirectManual);
	XDestroyWindow(x11Display,CompositorWindow);
	XCloseDisplay(x11Display);
	m_running = false;
}

bool X11WindowSystem::init(BaseGraphicSystem<Display*,Window>* base){
	X11WindowSystem *renderer = this;
	graphicSystem = base;
	int status = pthread_create( &renderThread, NULL, X11WindowSystem::EventLoop, (void*) renderer);
	if (0 != status )
	{
		return false;
	}

	while ( m_initialized == false )
	{
		usleep(10000);
		LOG_INFO("X11WindowSystem","Waiting start complete " << m_initialized);
	}
	LOG_INFO("X11WindowSystem","Start complete " << m_initialized << " success " << m_success);
	return m_success;
}

bool X11WindowSystem::start(){
	LOG_INFO("X11WindowSystem", "Starting / Creating thread");
	// let thread actually run
	this->m_running = true;
	pthread_mutex_unlock(&run_lock);
	return true;
}

void X11WindowSystem::stop(){
	LOG_INFO("X11WindowSystem","Stopping..");
	this->m_running = false;
	// needed if start was never called, we wake up thread, so it can immediatly finish
	pthread_mutex_unlock(&run_lock);
	pthread_join(renderThread,NULL);
}

void X11WindowSystem::allocatePlatformSurface(Surface* surface)
{
	XPlatformSurface* nativeSurface = (XPlatformSurface*)surface->platform;
	if (NULL == nativeSurface)
	{
		/*LOG_INFO("X11WindowSystem","creating native surface for new window");*/
		// this surface does not have a native platform surface attached yet!
		NewWindow(surface,surface->nativeHandle);
		MapWindow(surface->nativeHandle);
	}
}

void X11WindowSystem::doScreenShot(std::string fileName){
	takeScreenshot = ScreenshotOfDisplay;
	screenShotFile = fileName;
}

void X11WindowSystem::doScreenShotOfLayer(std::string fileName, const uint id){
	takeScreenshot = ScreenshotOfLayer;
	screenShotFile = fileName;
	screenShotID = id;
}

void X11WindowSystem::doScreenShotOfSurface(std::string fileName, const uint id){
	takeScreenshot = ScreenshotOfSurface;
	screenShotFile = fileName;
	screenShotID = id;
}
