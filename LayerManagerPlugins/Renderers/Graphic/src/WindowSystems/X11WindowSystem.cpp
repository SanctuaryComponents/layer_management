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

#include "WindowSystems/X11WindowSystem.h"
#include "Log.h"
#include "Layer.h"
#include <time.h>
#include <sys/time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iomanip>

int     X11WindowSystem::composite_opcode;
int		X11WindowSystem::damage_opcode;
const char X11WindowSystem::CompositorWindowTitle[] = "LayerManager";
bool    X11WindowSystem::m_xerror = false;

X11WindowSystem::X11WindowSystem(const char* displayname, int width, int height, Scene* pScene,GetVisualInfoFunction func)
: BaseWindowSystem(pScene)
, takeScreenshot(ScreenShotNone)
, displayname(displayname)
, getVisualFunc(func)
, debugMode(false)
, resolutionWidth(width)
, resolutionHeight(height)
, m_initialized(false)
, m_success(false)
, windowWidth(width)
, windowHeight(height)
, redrawEvent(false)
{
    LOG_DEBUG("X11WindowSystem", "creating X11WindowSystem");

    // init and take mutex, so windowsystem only does init phase until mutex is released again
    pthread_mutex_init(&run_lock, NULL);
    pthread_mutex_lock(&run_lock);
}

X11WindowSystem::~X11WindowSystem()
{
}

XVisualInfo* X11WindowSystem::getDefaultVisual(Display *dpy)
{
    XVisualInfo* windowVis = new XVisualInfo();
    if (windowVis)
    {
        windowVis->depth = 32;//DefaultDepth(dpy, 0);
        XMatchVisualInfo( dpy, 0, windowVis->depth, TrueColor, windowVis);
    }
    else
    {
        LOG_ERROR("X11WindowSystem", "Error: Unable to acquire visual\n");
    }
    return windowVis;
}

bool X11WindowSystem::OpenDisplayConnection()
{
    char* displayEnvironment = getenv("DISPLAY");
    
    if  (displayEnvironment == NULL ) 
    {
        displayEnvironment = ":0.0";
        setenv("DISPLAY",displayEnvironment,1);
    }
    
    x11Display = XOpenDisplay(displayEnvironment);
    if (!x11Display)
    {
        LOG_ERROR("X11WindowSystem", "Couldn't open default display!");
        return false;
    }
    LOG_DEBUG("X11WindowSystem", "Display connection: " << x11Display);
    return true;
}

bool X11WindowSystem::checkForCompositeExtension()
{
    if (!XQueryExtension (x11Display, COMPOSITE_NAME, &composite_opcode, &composite_event, &composite_error))
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

bool X11WindowSystem::checkForDamageExtension(){
	if (!XQueryExtension (x11Display, DAMAGE_NAME, &damage_opcode,
				&damage_event, &damage_error))
	{
		LOG_ERROR("X11WindowSystem", "No damage extension");
		return false;
	}
	XDamageQueryVersion (x11Display, &damage_major, &damage_minor);
	LOG_DEBUG("X11WindowSystem", "Found damage extension: damage opcode: " << damage_opcode);
	LOG_DEBUG("X11WindowSystem", "damage_major: " << damage_major);
	LOG_DEBUG("X11WindowSystem", "damage_minor: " << damage_minor);
	return true;
}

void X11WindowSystem::printDebug(){
	// print stuff about layerlist
	std::stringstream debugmessage;
	debugmessage << "Layer:  ID |   X  |   Y  |   W  |   H  | Al. \n";

    LayerList list = m_pScene->getCurrentRenderOrder();

    // loop the layers
    LayerListConstIterator iter = list.begin();
    LayerListConstIterator iterEnd = list.end();

    for(; iter != iterEnd; ++iter)
    {
        Rectangle dest = (*iter)->getDestinationRegion();
        debugmessage << "            " << std::setw(4) << (*iter)->getID() << " " << std::setw(3) << dest.x << " " << std::setw(3) << dest.y << " " << std::setw(3) << dest.width << " " << std::setw(3) << dest.height << " " << std::setw(3) << (*iter)->opacity << "\n";

        debugmessage << "    Surface:  ID |Al.|  SVP: X |  Y |  W |  H     DVP:  X |  Y |  W |  H \n";

        // loop the surfaces of within each layer
        SurfaceList surfaceList = (*iter)->getAllSurfaces();
        SurfaceListIterator surfaceIter = surfaceList.begin();
        SurfaceListIterator surfaceIterEnd = surfaceList.end();

        for(; surfaceIter != surfaceIterEnd ; ++surfaceIter)
        {
            Rectangle src = (*surfaceIter)->getSourceRegion();
            Rectangle dest = (*surfaceIter)->getDestinationRegion();
            debugmessage << "                        " << std::setw(4) << (*surfaceIter)->getID() << " " << std::setprecision(3) << (*surfaceIter)->opacity<< " " << std::setw(3) << src.x << " " << std::setw(3) << src.y << " " << std::setw(3) << src.width << " " << std::setw(3) << src.height << " " << std::setw(3) << dest.x << " " << std::setw(3) << dest.y << " " << std::setw(3) << dest.width << " " << std::setw(3) << dest.height  << "\n";
        }
    }
    LOG_INFO("X11WindowSystem",debugmessage.str());
}

Window * getListOfAllTopLevelWindows (Display *disp, unsigned int *len)
{
    LOG_DEBUG("X11WindowSystem", "Getting list of all windows");
    Window *children;
    Window    root_return, parent_return;
    Window  root = XDefaultRootWindow(disp);
    XQueryTree (disp, root, &root_return, &parent_return, &children, len);
    return children;
}

bool X11WindowSystem::isWindowValid(Window w)
{
    // skip our own two windows
    return (w!=NULL && w != CompositorWindow);
}

Surface* X11WindowSystem::getSurfaceForWindow(Window w)
{
    // go though all surfaces
    const std::map<unsigned int,Surface*> surfaces = m_pScene->getAllSurfaces();
    for(std::map<unsigned int, Surface*>::const_iterator currentS = surfaces.begin(); currentS != surfaces.end(); ++currentS)
    {
        Surface* currentSurface = (*currentS).second;
        if (!currentSurface)
        {
            continue;
        }
        if (currentSurface->nativeHandle == static_cast<int>(w))
        {
                return currentSurface;
        }
    }
    LOG_DEBUG("X11WindowSystem", "could not find surface for window " << w);
    return NULL;
}

void X11WindowSystem::checkForNewSurface()
{
    LayerList layers = m_pScene->getCurrentRenderOrder();
    for(LayerListConstIterator current = layers.begin(); current != layers.end(); current++)
    {
        SurfaceList surfaces = (*current)->getAllSurfaces();
        for(SurfaceListConstIterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++)
        {
            allocatePlatformSurface(*currentS);
        }
    }
}

void X11WindowSystem::configureSurfaceWindow(Window window)
{
    if (isWindowValid(window))
    {
        LOG_DEBUG("X11WindowSystem", "Updating window " << window);
        UnMapWindow(window);
        MapWindow(window);
        XWindowAttributes att;
        XGetWindowAttributes (x11Display, window, &att);
        int winWidth = att.width;
        int winHeight = att.height;

        Surface* surface = getSurfaceForWindow(window);
        if (!surface)
        {
            LOG_ERROR("X11WindowSystem", "surface emtpy");
            return;
        }
        if (!surface->platform)
        {
            LOG_ERROR("X11WindowSystem", "platform surface empty");
            return;
        }

        LOG_DEBUG("X11WindowSystem", "Updating surface " << surface->getID());

        surface->OriginalSourceHeight = winHeight;
        surface->OriginalSourceWidth = winWidth;
/*        surface->setDestinationRegion(Rectangle(0, 0, winWidth, winHeight));*/
/*        surface->setSourceRegion(Rectangle(0, 0, winWidth, winHeight));*/

        LOG_DEBUG("X11WindowSystem", "Done Updating window " << window);
    }
}

void X11WindowSystem::MapWindow(Window window)
{
    if (isWindowValid(window))
    {
        XWindowAttributes att;
        XGetWindowAttributes (x11Display, window, &att);
/*      LOG_DEBUG("X11WindowSystem", "XCompositeRedirectWindow()");
        XCompositeRedirectWindow(x11Display, window, CompositeRedirectManual);
        XSync(x11Display, 0);*/
        if (att.map_state == IsViewable && att.override_redirect==0)
        {
            LOG_DEBUG("X11WindowSystem", "Mapping window " << window);
            Surface* surface = getSurfaceForWindow(window);
            if (!surface)
            {
                LOG_ERROR("X11WindowSystem", "surface emtpy");
                return;
            }
            if (!surface->platform)
            {
                LOG_ERROR("X11WindowSystem", "platform surface empty");
                return;
            }

            XPlatformSurface* x11surf = (XPlatformSurface*)surface->platform;
            if (x11surf->isMapped)
            {
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
/*            surface->setDestinationRegion(Rectangle(0,0,winWidth,winHeight));*/
/*            surface->setSourceRegion(Rectangle(0,0,winWidth,winHeight)); */

            graphicSystem->m_binder->createClientBuffer(surface);
            XSync(x11Display, 0);

/*          surface->visibility = true;
            surface->setOpacity(1.0);*/
            LOG_DEBUG("X11WindowSystem", "Mapping Surface " << surface->getID());
            LOG_DEBUG("X11WindowSystem", "Done mapping");
        }
    }
    LOG_INFO("X11WindowSystem","mapping end");
}

void X11WindowSystem::UnMapWindow(Window window)
{
    if (isWindowValid(window))
    {
        LOG_DEBUG("X11WindowSystem", "Unmapping window " << window);
        Surface* surface = getSurfaceForWindow(window);
        if (!surface)
        {
            LOG_ERROR("X11WindowSystem", "surface emtpy");
            return;
        }
        // set invisible first, so it wont be used for rendering anymore
/*      surface->visibility = false;*/
        if (!surface->platform)
        {
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
        if (x11surf->pixmap)
        {
            int result = XFreePixmap(x11Display, x11surf->pixmap);
            LOG_DEBUG("X11WindowSystem", "XFreePixmap() returned " << result);
        }
    }
    LOG_DEBUG("X11WindowSystem", "Unmap finished");
}

void X11WindowSystem::NewWindow(Surface* surface, Window window)
{
    if (isWindowValid(window))
    {
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
            if (name != NULL && strcmp(name,GuiTitle)==0)
            {
                LOG_DEBUG("X11WindowSystem", "Found gui window: repositioning it");
                XCompositeUnredirectWindow(x11Display,window,CompositeRedirectManual);
//XLowerWindow(x11Display,window);
				XMoveWindow(x11Display, window, 50,	500);
				XMapRaised(x11Display, window);
			}
		}else {
			LOG_DEBUG("X11WindowSystem", "Error fetching window name");
		}
		LOG_DEBUG("X11WindowSystem","Creating New Damage for window - " << window);
		XDamageCreate(x11Display,window,XDamageReportNonEmpty);
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
/*      surface->visibility = false; */

        int winWidth = att.width;
        int winHeight = att.height;

        surface->OriginalSourceHeight =winHeight;
        surface->OriginalSourceWidth = winWidth;
/*        surface->setDestinationRegion(Rectangle(0,0,winWidth,winHeight));*/
/*        surface->setSourceRegion(Rectangle(0,0,winWidth,winHeight));*/
        LOG_DEBUG("X11WindowSystem", "orig width " << surface->OriginalSourceWidth);
        LOG_DEBUG("X11WindowSystem", "orig heigth " << surface->OriginalSourceHeight);
    }
    else
    {
        LOG_DEBUG("X11WindowSystem", "skipping window");
    }
    LOG_DEBUG("X11WindowSystem", "created the new surface");
}

void X11WindowSystem::DestroyWindow(Window window)
{
    if (isWindowValid(window))
    {
        LOG_DEBUG("X11WindowSystem", "Destroying Surface for window " << window);
        Surface* surface = getSurfaceForWindow(window);
        if (!surface)
        {
            LOG_ERROR("X11WindowSystem", "surface empty");
            return;
        }
        graphicSystem->m_binder->destroyClientBuffer(surface);
        LOG_DEBUG("X11WindowSystem", "Unmapping window " << window);
        UnMapWindow(window);
        LOG_DEBUG("X11WindowSystem", "Removed Surface " << surface->getID());
        m_pScene->removeSurface(surface);
    }
}

bool X11WindowSystem::CreatePixmapsForAllWindows()
{
    bool result = true;
    LOG_DEBUG("X11WindowSystem", "redirecting all windows");
    Window root = RootWindow(x11Display, 0);
    XCompositeRedirectSubwindows(x11Display,root,CompositeRedirectManual);
    XSync(x11Display,0);
    return result;
}

bool X11WindowSystem::CreateCompositorWindow()
{
    LOG_DEBUG("X11WindowSystem", "Get root window");
    bool result = true;
    Window root = RootWindow(x11Display,0);

    LOG_DEBUG("X11WindowSystem", "Get default screen");
    // draw a black background the full size of the resolution
    //long x11Screen = XDefaultScreen( x11Display );

    LOG_DEBUG("X11WindowSystem", "Creating Compositor Window");
    // create the actual content window
//    CompositorWindow = XCreateSimpleWindow(  x11Display, root, 0, 0, windowWidth, windowHeight,
//                                             0, BlackPixel(x11Display, x11Screen), WhitePixel(x11Display, x11Screen));
    XSetWindowAttributes attr;
    // draw a black background the full size of the resolution
    attr.override_redirect = True;
    attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask;
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    windowVis = getVisualFunc(x11Display);
    attr.colormap = XCreateColormap(x11Display, root, windowVis->visual, AllocNone);
    attr.override_redirect = True;

    Window compManager = XGetSelectionOwner(x11Display,XInternAtom(x11Display,"_NET_WM_CM_S0",0));
    if ( 0 != compManager )
    {
        LOG_ERROR("X11WindowSystem", "Could not create compositor window, annother compisite manager is already running");
        return false;
    }

//    Window overlaywindow = XCompositeGetOverlayWindow(x11Display,root);

    CompositorWindow = XCreateWindow(x11Display, root, 0, 0, windowWidth, windowHeight,
            0, windowVis->depth, InputOutput,
            windowVis->visual, CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect, &attr);

    //    Screen* s = ScreenOfDisplay( x11Display, 0 );
    //    CompositorWindow = XCreateSimpleWindow( x11Display,
    //                                                             RootWindowOfScreen(s),
    //                                                             0, 0, windowWidth, windowHeight, 0,
    //                                                             BlackPixelOfScreen(s),
    //                                                             WhitePixelOfScreen(s) );
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

static float timeSinceLastCalc = 0.0;
static float FPS = 0.0;
static struct timeval tv;
static struct timeval tv0;
static int Frame = 0;

void CalculateFPS()
{
	// we have rendered a frame
    Frame ++;

    // every 3 seconds, calculate & print fps
    gettimeofday(&tv, NULL);
    timeSinceLastCalc = (float)(tv.tv_sec-tv0.tv_sec) + 0.000001*((float)(tv.tv_usec-tv0.tv_usec));

    if (timeSinceLastCalc > 3.0f)
    {
    	FPS = ((float)(Frame)) / timeSinceLastCalc;
    	char floatStringBuffer[256];
    	sprintf(floatStringBuffer, "FPS: %f", FPS);
    	LOG_INFO("X11WindowSystem", floatStringBuffer);
    	tv0 = tv;
    	Frame = 0;
    }
}

void
X11WindowSystem::RedrawAllLayers()
{
	std::list<Layer*> layers = m_pScene->getCurrentRenderOrder();
	for(std::list<Layer*>::const_iterator current = layers.begin(); current != layers.end(); current++)
	{
		Layer* currentLayer = (Layer*)*current;
		graphicSystem->beginLayer(currentLayer);
		graphicSystem->checkRenderLayer();
		graphicSystem->endLayer();
	}

	if (m_damaged)
	{
		graphicSystem->clearBackground();
		for(std::list<Layer*>::const_iterator current = layers.begin(); current != layers.end(); current++)
		{
			graphicSystem->beginLayer(*current);
			graphicSystem->renderLayer();
			graphicSystem->endLayer();
		}
	}
    /* Reset the damage flag, all is up to date */
	m_damaged = false;
}

void X11WindowSystem::Redraw()
{
    // draw all the layers
    //graphicSystem->clearBackground();
    /*LOG_INFO("X11WindowSystem","Locking List");*/
    m_pScene->lockScene();

    RedrawAllLayers();

    if (debugMode)
    {
        printDebug();
    }
    CalculateFPS();

    m_pScene->unlockScene();
    /*LOG_INFO("X11WindowSystem","UnLocking List");*/
    graphicSystem->swapBuffers();
}

void X11WindowSystem::Screenshot()
{
	graphicSystem->clearBackground();

	/*LOG_INFO("X11WindowSystem","Locking List");*/
	m_pScene->lockScene();

	if (takeScreenshot==ScreenshotOfDisplay){
	LOG_DEBUG("X11WindowSystem", "Taking screenshot");
		RedrawAllLayers();
	}else if(takeScreenshot==ScreenshotOfLayer){
		LOG_DEBUG("X11WindowSystem", "Taking screenshot of layer");
		Layer* currentLayer = m_pScene->getLayer(screenShotID);
		if (currentLayer!=NULL){
			graphicSystem->beginLayer(currentLayer);
			graphicSystem->renderLayer();
			graphicSystem->endLayer();
		}
	}else if(takeScreenshot==ScreenshotOfSurface){
		LOG_DEBUG("X11WindowSystem", "Taking screenshot of surface");
		Surface* currentSurface = m_pScene->getSurface(screenShotID);
		if (NULL!=currentSurface){
			Layer* l = m_pScene->createLayer(GraphicalObject::INVALID_ID);
			l->setOpacity(1.0);
			l->setDestinationRegion(currentSurface->getDestinationRegion());
			l->setSourceRegion(currentSurface->getSourceRegion());
			graphicSystem->beginLayer(l);
			graphicSystem->renderSurface(currentSurface);
			graphicSystem->endLayer();
			m_pScene->removeLayer(l);
			// layer is deleted in removeLayer.
		}else{
			LOG_ERROR("X11WindowSystem", "Could not take screenshot of non existing surface");
		}
	}

	graphicSystem->saveScreenShotOfFramebuffer(screenShotFile);
//		graphicSystem->swapBuffers();
	takeScreenshot = ScreenShotNone;
	LOG_DEBUG("X11WindowSystem", "Done taking screenshot");

	m_pScene->unlockScene();
	/*LOG_INFO("X11WindowSystem","UnLocking List");*/
}

int
X11WindowSystem::error (Display *dpy, XErrorEvent *ev)
{
    const char* name = NULL;
    static char buffer[256];

    if (ev->request_code == composite_opcode && ev->minor_code == X_CompositeRedirectSubwindows)
    {
        LOG_ERROR("X11WindowSystem", "Maybe another composite manager is already running");
    }

    if (!name)
    {
        buffer[0] = '\0';
        XGetErrorText (dpy, ev->error_code, buffer, sizeof (buffer));
        name = buffer;
    }
    name = (strlen (name) > 0) ? name : "unknown";
    LOG_ERROR("X11WindowSystem", "X Error: " << (int)ev->error_code << " " << name << " request : " << (int)ev->request_code << " minor: " <<  (int)ev->minor_code << " serial: " << (int)ev->serial);
    m_xerror = true;
    return 0;
}

bool X11WindowSystem::initXServer()
{
    LOG_DEBUG("X11WindowSystem", "Initialising XServer connection");
    bool result = true;

    //setDisplayMode();
    if (!CreateCompositorWindow())
    {
        LOG_ERROR("X11WindowSystem", "Compositor Window creation failed " );
        return false;
    }

    LOG_DEBUG("X11WindowSystem", "Compositor Window ID: " << CompositorWindow);

    if ( CreatePixmapsForAllWindows() )
    {
            //unredirect our window
#ifdef FULLSCREEN
        XCompositeUnredirectWindow(x11Display, background, CompositeRedirectManual);
#endif
        XCompositeUnredirectWindow(x11Display, CompositorWindow, CompositeRedirectManual);
        LOG_DEBUG("X11WindowSystem", "Initialised XServer connection complete");
    } else {
        LOG_ERROR("X11WindowSystem", "Initialised XServer connection failed");
        result = false;        
    }

    return result;
}

/**
 * Thread in charge of the CompositorWindow eventloop
 * Friend function of class X11WindowSystem
 */
void * X11eventLoopCallback(void *ptr)
{
	X11WindowSystem *windowsys = static_cast<X11WindowSystem*>( (X11WindowSystem*) ptr);
	return windowsys->EventLoop();
}

void* X11WindowSystem::EventLoop()
{
    // INITALIZATION
    LOG_DEBUG("X11WindowSystem", "Enter thread");

    bool status = true;
	bool checkRedraw = false;

    XSetErrorHandler(error);
    // init own stuff
    LOG_DEBUG("X11WindowSystem", "open display connection");
    status &= this->OpenDisplayConnection();

    LOG_DEBUG("X11WindowSystem", "check for composite extension");
    status &= this->checkForCompositeExtension();

	LOG_DEBUG("X11WindowSystem", "check for damage extension");
	status &= this->checkForDamageExtension();

	LOG_DEBUG("X11WindowSystem", "init xserver");
	status &= this->initXServer();

    status &= this->graphicSystem->init(this->x11Display,this->CompositorWindow);

    this->m_success = status;
    this->m_initialized = true;

    // Done with init, wait for lock to actually run (ie start/stop method called)
    pthread_mutex_lock(&this->run_lock);

    LOG_DEBUG("X11WindowSystem", "Starting Event loop");
    Layer* defaultLayer = 0;

	// run the main event loop while rendering
	gettimeofday(&tv0, NULL);
	if (this->debugMode)
	{
		defaultLayer = this->m_pScene->createLayer(0);
		defaultLayer->setOpacity(1.0);
		defaultLayer->setDestinationRegion(Rectangle(0,0,this->resolutionWidth,this->resolutionHeight));
		defaultLayer->setSourceRegion(Rectangle(0,0,this->resolutionWidth,this->resolutionHeight));
		this->m_pScene->getCurrentRenderOrder().push_back(defaultLayer);
	}
	LOG_DEBUG("X11WindowSystem", "Enter render loop");

	// clear screen to avoid garbage on startup
	this->graphicSystem->clearBackground();
	this->graphicSystem->swapBuffers();
    XFlush(this->x11Display);
	while (this->m_running)
	{
#ifndef USE_XTHREADS
		if ( XPending(this->x11Display) > 0) {
#endif //USE_XTHREADS
			XEvent event;
			// blocking wait for event
			XNextEvent(this->x11Display, &event);
            this->m_pScene->lockScene();
			switch (event.type)
			{
			case CreateNotify:
			{
				if (this->debugMode)
				{
					LOG_DEBUG("X11WindowSystem", "CreateNotify Event");
					Surface* s = this->m_pScene->createSurface(0);
					s->setOpacity(1.0);
					this->NewWindow(s, event.xcreatewindow.window);
					defaultLayer->addSurface(s);
				}
				break;
			}
			case ConfigureNotify:
				LOG_DEBUG("X11WindowSystem", "Configure notify Event");
				this->configureSurfaceWindow( event.xconfigure.window);
				checkRedraw = true;
				break;

			case DestroyNotify:
				LOG_DEBUG("X11WindowSystem", "Destroy  Event");
				this->DestroyWindow(event.xdestroywindow.window);
				checkRedraw = true;
				break;
			case Expose:
				LOG_DEBUG("X11WindowSystem", "Expose Event");
				checkRedraw = true;
				break;
			case ButtonPress:
				LOG_DEBUG("X11WindowSystem", "Button press Event");
				//running = False;
				break;
			case MapNotify:
				LOG_DEBUG("X11WindowSystem", "Map Event");
				this->MapWindow(event.xmap.window);
				checkRedraw = true;
				break;
			case UnmapNotify:
				LOG_DEBUG("X11WindowSystem", "Unmap Event");
				this->UnMapWindow(event.xunmap.window);
				checkRedraw = true;  
				break;
			case ReparentNotify:
				LOG_DEBUG("X11WindowSystem", "Reparent Event");
				//			 if (event.xreparent.parent == root)
				//				 renderer->NewWindow(event.xreparent.window);
				//			 else
				//				 renderer->DestroyWindow(event.xreparent.window);
				break;

			default:
				if (event.type == this->damage_event + XDamageNotify)
				{
					XDamageSubtract(this->x11Display, ((XDamageNotifyEvent*)(&event))->damage, None, None);
					Surface* currentSurface = this->getSurfaceForWindow(((XDamageNotifyEvent*)(&event))->drawable);
					if (currentSurface==NULL)
					{
						LOG_ERROR("X11WindowSystem", "surface empty");
						break;
					}
					currentSurface->damaged = true;
                    currentSurface->frameCounter++;
					checkRedraw = true;
				}
				break;
			}
            this->m_pScene->unlockScene();
#ifndef USE_XTHREADS
		}
#endif //USE_XTHREADS
		if (this->redrawEvent)
		{
			this->redrawEvent = false;

			// check if we are supposed to take screenshot
			if (this->takeScreenshot!=ScreenShotNone)
			{
				this->Screenshot();
			}
			else
			{
                this->checkForNewSurface();
				checkRedraw = true;
			}

		}

		if (checkRedraw)
		{
			this->Redraw();
			checkRedraw = false;
        } 
#ifndef USE_XTHREADS        
        else {
            /* put thread in sleep mode for 500 useconds due to safe cpu performance */

            usleep(500);
        }
#endif
	}
	this->cleanup();
	LOG_INFO("X11WindowSystem", "Renderer thread finished");
	return NULL;
}
#ifdef USE_XTHREADS
static Display* displaySignal = NULL;
#endif //USE_XTHREADS
void X11WindowSystem::signalRedrawEvent()
{
	// set flag that redraw is needed
	redrawEvent = true;
    m_damaged = true;
#ifdef USE_XTHREADS
	// send dummy expose event, to wake up blocking x11 event loop (XNextEvent)
	LOG_DEBUG("X11WindowSystem", "Sending dummy event to wake up renderer thread");
    if (NULL == displaySignal ) 
    {
        displaySignal = XOpenDisplay(":0");
    }   
	XExposeEvent ev = { Expose, 0, 1, displaySignal, CompositorWindow, 0, 0, 100, 100, 0 };
	XLockDisplay(displaySignal);
	XSendEvent(displaySignal, CompositorWindow, False, ExposureMask, (XEvent *) &ev);
	XUnlockDisplay(displaySignal);
	XFlush(displaySignal);
	LOG_DEBUG("X11WindowSystem", "Event successfully sent to renderer");
#endif //USE_XTHREADS
}

void X11WindowSystem::cleanup(){
	LOG_INFO("X11WindowSystem", "Cleanup");
	Window root = RootWindow(x11Display, 0);
	XCompositeUnredirectSubwindows(x11Display,root,CompositeRedirectManual);
	XDestroyWindow(x11Display,CompositorWindow);
#ifdef USE_XTHREADS
    if ( NULL  != displaySignal ) 
    {
    	XCloseDisplay(displaySignal);        
    }
#endif //USE_XTHREADS
	XCloseDisplay(x11Display);
	m_running = false;
}

bool X11WindowSystem::init(BaseGraphicSystem<Display*,Window>* base)
{
#ifdef USE_XTHREADS
	XInitThreads();
#endif //USE_XTHREADS
    X11WindowSystem *renderer = this;
    graphicSystem = base;
    int status = pthread_create( &renderThread, NULL, X11eventLoopCallback, (void*) renderer);
    if (0 != status )
    {
        return false;
    }

    while (!m_initialized)
    {
        usleep(10000); // TODO
        LOG_INFO("X11WindowSystem","Waiting start complete " << m_initialized);
    }
    LOG_INFO("X11WindowSystem","Start complete " << m_initialized << " success " << m_success);
    return m_success;
}

bool X11WindowSystem::start()
{
    bool result = true;
    LOG_INFO("X11WindowSystem", "Starting / Creating thread");
    // let thread actually run
    if ( m_xerror == false ) 
    {
        this->m_running = true;
        pthread_mutex_unlock(&run_lock);
    } else {
        this->m_running = false;
        pthread_mutex_unlock(&run_lock);
        result = false;
    }
    return result;
}

void X11WindowSystem::stop()
{
    LOG_INFO("X11WindowSystem","Stopping..");
    this->m_running = false;
    // needed if start was never called, we wake up thread, so it can immediatly finish
    pthread_mutex_unlock(&run_lock);
    pthread_join(renderThread,NULL);
}

void X11WindowSystem::allocatePlatformSurface(Surface* surface)
{
    XPlatformSurface* nativeSurface = (XPlatformSurface*)surface->platform;
    if (!nativeSurface)
    {
        /*LOG_INFO("X11WindowSystem","creating native surface for new window");*/
        // this surface does not have a native platform surface attached yet!
        NewWindow(surface,surface->nativeHandle);
        MapWindow(surface->nativeHandle);
    }
}

void X11WindowSystem::doScreenShot(std::string fileName)
{
    takeScreenshot = ScreenshotOfDisplay;
    screenShotFile = fileName;
}

void X11WindowSystem::doScreenShotOfLayer(std::string fileName, const uint id)
{
    takeScreenshot = ScreenshotOfLayer;
    screenShotFile = fileName;
    screenShotID = id;
}

void X11WindowSystem::doScreenShotOfSurface(std::string fileName, const uint id)
{
    takeScreenshot = ScreenshotOfSurface;
    screenShotFile = fileName;
    screenShotID = id;
}

