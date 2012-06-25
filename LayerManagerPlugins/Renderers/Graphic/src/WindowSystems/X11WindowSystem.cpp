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
#include "config.h"
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
int     X11WindowSystem::damage_opcode;
const char X11WindowSystem::CompositorWindowTitle[] = "LayerManager";
bool    X11WindowSystem::m_xerror = false;

X11WindowSystem::X11WindowSystem(const char* displayname, int width, int height, Scene* pScene,GetVisualInfoFunction func)
: BaseWindowSystem(pScene)
, takeScreenshot(ScreenShotNone)
, screenShotFile()
, screenShotSurfaceID(0)
, screenShotLayerID()
, displayname(displayname)
, getVisualFunc(func)
, debugMode(false)
, resolutionWidth(width)
, resolutionHeight(height)
, composite_event(0)
, composite_error(0)
, composite_major(0)
, composite_minor(0)
, damage_event(0)
, damage_error(0)
, damage_major(0)
, damage_minor(0)
, m_running(false)
, m_initialized(false)
, m_success(false)
, m_systemState(IDLE_STATE)
, m_displayEnvironment(NULL)
, x11Display(0)
, renderThread(0)
, windowWidth(width)
, windowHeight(height)
, CompositorWindow(0)
, windowVis(0)
, run_lock()
, graphicSystem(0)
{
    LOG_DEBUG("X11WindowSystem", "creating X11WindowSystem");

    // init and take mutex, so windowsystem only does init phase until mutex is released again
    pthread_mutex_init(&run_lock, NULL);
    pthread_mutex_lock(&run_lock);
}

X11WindowSystem::~X11WindowSystem()
{
    if (windowVis)
    {
        delete windowVis;
    }
}

XVisualInfo* X11WindowSystem::getDefaultVisual(Display *dpy)
{
    XVisualInfo* windowVis = new XVisualInfo();
    if (windowVis)
    {
        windowVis->depth = DefaultDepth(dpy, DefaultScreen(dpy));
        if (!XMatchVisualInfo( dpy, 0, windowVis->depth, TrueColor, windowVis))
        {
            LOG_ERROR("X11WindowSystem", "Error: Required visual not found\n");
            delete windowVis;
            return NULL;
        }
    }
    else
    {
        LOG_ERROR("X11WindowSystem", "Error: Unable to acquire visual\n");
    }
    return windowVis;
}

bool X11WindowSystem::OpenDisplayConnection()
{
    m_displayEnvironment = getenv("DISPLAY");

    if  (m_displayEnvironment == NULL )
    {
        m_displayEnvironment = ":0.0";
        setenv("DISPLAY",m_displayEnvironment,1);
    }

    x11Display = XOpenDisplay(m_displayEnvironment);
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
    if (x11Display == NULL || !XQueryExtension (x11Display, COMPOSITE_NAME, &composite_opcode, &composite_event, &composite_error))
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
    if (x11Display == NULL || !XQueryExtension (x11Display, DAMAGE_NAME, &damage_opcode,
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
    LOG_DEBUG("X11WindowSystem",debugmessage.str());
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
    return (w!=None && w != CompositorWindow);
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
        if (currentSurface->getNativeContent() == static_cast<int>(w))
        {
                return currentSurface;
        }
    }
    LOG_DEBUG("X11WindowSystem", "could not find surface for window " << w);
    return NULL;
}

void X11WindowSystem::checkForNewSurfaceNativeContent()
{
    m_pScene->lockScene();
    LayerList layers = m_pScene->getCurrentRenderOrder();
    for(LayerListConstIterator current = layers.begin(); current != layers.end(); current++)
    {
        SurfaceList surfaces = (*current)->getAllSurfaces();
        for(SurfaceListConstIterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++)
        {
            if ((*currentS)->hasNativeContent())
            {
                allocatePlatformSurface(*currentS);
            }
            else // While we are at it, also cleanup any stale native content
            {
                deallocatePlatformSurface(*currentS);
            }
        }
    }
    m_pScene->unlockScene();
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
             LOG_WARNING("X11WindowSystem", "Could not find surface for window " << window);
            return;
        }
        if (!surface->platform)
        {
            LOG_WARNING("X11WindowSystem", "Platform surface not available for window " << window);
            return;
        }

        LOG_DEBUG("X11WindowSystem", "Updating surface " << surface->getID());

        surface->OriginalSourceHeight = winHeight;
        surface->OriginalSourceWidth = winWidth;

        LOG_DEBUG("X11WindowSystem", "Done Updating window " << window);
    }
}

void X11WindowSystem::MapWindow(Window window)
{
    LOG_DEBUG("X11WindowSystem","Map window begin");
    if (isWindowValid(window))
    {
        XWindowAttributes att;
        XGetWindowAttributes (x11Display, window, &att);
/*      LOG_DEBUG("X11WindowSystem", "XCompositeRedirectWindow()");
        XCompositeRedirectWindow(x11Display, window, CompositeRedirectManual);
        XSync(x11Display, 0);*/
        if (att.map_state == IsViewable && att.override_redirect==0)
        {
            LOG_DEBUG("X11WindowSystem", "Mapping surface to window " << window);
            Surface* surface = getSurfaceForWindow(window);
            if (!surface)
            {
                LOG_WARNING("X11WindowSystem", "Could not map surface to window " << window);
                return;
            }
            if (!surface->platform)
            {
                LOG_WARNING("X11WindowSystem", "Platform surface not available for window " << window);
                return;
            }

            XPlatformSurface* x11surf = (XPlatformSurface*)surface->platform;
            if (x11surf->isMapped)
            {
                LOG_WARNING("X11WindowSystem", "Platform surface already mapped");
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
            surface->renderPropertyChanged = true;

            graphicSystem->activateGraphicContext();
            graphicSystem->getTextureBinder()->createClientBuffer(surface);
            graphicSystem->releaseGraphicContext();
            XSync(x11Display, 0);

            LOG_DEBUG("X11WindowSystem", "Mapping Surface " << surface->getID() << " to window " << window);
            LOG_DEBUG("X11WindowSystem", "Mapping successfull");
        }
    }
    LOG_DEBUG("X11WindowSystem","Map window end");
}

void X11WindowSystem::UnMapWindow(Window window)
{
    LOG_DEBUG("X11WindowSystem", "Unmap begin");
    if (isWindowValid(window))
    {
        LOG_DEBUG("X11WindowSystem", "Unmapping surface from window " << window);
        Surface* surface = getSurfaceForWindow(window);
        if (!surface)
        {
            LOG_WARNING("X11WindowSystem", "Could not unmap window " << window);
            return;
        }
        if (!surface->platform)
        {
            LOG_WARNING("X11WindowSystem", "Platform surface not available for window " << window);
            return;
        }
        XPlatformSurface* x11surf = (XPlatformSurface*)surface->platform;
        LOG_DEBUG("X11WindowSystem", "Unmapping surface " << surface->getID());
        if (!x11surf->isMapped)
        {
            LOG_WARNING("X11WindowSystem", "Platform surface already unmapped");
            return;
        }
        x11surf->isMapped = false;


        LOG_DEBUG("X11WindowSystem", "Destroying ClientBuffer");
        graphicSystem->activateGraphicContext();
        graphicSystem->getTextureBinder()->destroyClientBuffer(surface);
        graphicSystem->releaseGraphicContext();
        XSync(x11Display, 0);

        LOG_DEBUG("X11WindowSystem", "Removing X Pixmap");
        if (x11surf->pixmap)
        {
            int result = XFreePixmap(x11Display, x11surf->pixmap);
            LOG_DEBUG("X11WindowSystem", "XFreePixmap() returned " << result);
        }

        surface->renderPropertyChanged = true;
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
        if (status >= Success && name)
        {
            LOG_DEBUG("X11WindowSystem", "Found window: " << window << " " << name);
            char GuiTitle[]  = "Layermanager Remote GUI\0";
            if (strcmp(name,GuiTitle)==0)
            {
                LOG_DEBUG("X11WindowSystem", "Found gui window: repositioning it");
                XCompositeUnredirectWindow(x11Display,window,CompositeRedirectManual);
                XMoveWindow(x11Display, window, 50, 500);
                XMapRaised(x11Display, window);
            }
        }
        else
        {
            LOG_DEBUG("X11WindowSystem", "Error fetching window name");
        }

        if (att.c_class == InputOutput)
        {
            LOG_DEBUG("X11WindowSystem","Creating New Damage for window - " << window);
            XDamageCreate(x11Display,window,XDamageReportNonEmpty);
        }

        XFree(name);
        XLowerWindow(x11Display,window);

        surface->setNativeContent(window);
        XPlatformSurface * platformSurface = (XPlatformSurface*)graphicSystem->getTextureBinder()->createPlatformSurface(surface);
        platformSurface->isMapped = false;

        LOG_DEBUG("X11WindowSystem", "Created native Surface for X11 Window id " << window);

        surface->platform = platformSurface;

        int winWidth = att.width;
        int winHeight = att.height;

        surface->OriginalSourceHeight = winHeight;
        surface->OriginalSourceWidth = winWidth;

        LOG_DEBUG("X11WindowSystem", "Original width " << surface->OriginalSourceWidth);
        LOG_DEBUG("X11WindowSystem", "Original heigth " << surface->OriginalSourceHeight);
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
            LOG_WARNING("X11WindowSystem", "Could not find surface for window " << window);
            return;
        }
        graphicSystem->activateGraphicContext();
        graphicSystem->getTextureBinder()->destroyClientBuffer(surface);
        graphicSystem->releaseGraphicContext();
        LOG_DEBUG("X11WindowSystem", "Unmapping window " << window);
        UnMapWindow(window);
        LOG_DEBUG("X11WindowSystem", "Remove Native Content from Surface " << surface->getID());
        surface->removeNativeContent();
        delete surface->platform;
        surface->platform = NULL;
    }
}

bool X11WindowSystem::CreatePixmapsForAllWindows()
{
    bool result = true;
    LOG_DEBUG("X11WindowSystem", "redirecting all windows");
    Window root = RootWindow(x11Display, DefaultScreen(x11Display));
    XCompositeRedirectSubwindows(x11Display,root,CompositeRedirectManual);
    XSync(x11Display,0);
    return result;
}

bool X11WindowSystem::CreateCompositorWindow()
{
    LOG_DEBUG("X11WindowSystem", "Get root window");
    bool result = true;
    CompositorWindow = None;
    Window root = RootWindow(x11Display, DefaultScreen(x11Display));

    LOG_DEBUG("X11WindowSystem", "Creating Compositor Window");

    XSetWindowAttributes attr;
    // draw a black background the full size of the resolution
    attr.override_redirect = True;
#ifdef WITH_INPUT_EVENTS
    attr.event_mask =  ExposureMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | Button1MotionMask;
#else
    attr.event_mask = ExposureMask | StructureNotifyMask;
#endif
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    windowVis = getVisualFunc(x11Display);
    if (!windowVis)
    {
        return false;
    }
    attr.colormap = XCreateColormap(x11Display, root, windowVis->visual, AllocNone);
    attr.override_redirect = True;

    Window compManager = XGetSelectionOwner(x11Display,XInternAtom(x11Display,"_NET_WM_CM_S0",0));
    if ( None != compManager )
    {
        LOG_ERROR("X11WindowSystem", "Could not create compositor window, annother compisite manager is already running");
        return false;
    }

    CompositorWindow = XCreateWindow(x11Display, root, 0, 0, windowWidth, windowHeight,
            0, windowVis->depth, InputOutput,
            windowVis->visual, CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect, &attr);

    if (None == CompositorWindow)
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


void X11WindowSystem::calculateSurfaceFps(Surface *currentSurface, float time ) 
{
        char floatStringBuffer[256];
        float surfaceUpdateFps = ((float)(currentSurface->updateCounter)) / time ;
        float surfaceDrawFps = ((float)(currentSurface->drawCounter)) / time ;
        sprintf(floatStringBuffer, "0x%08x update fps: %3.2f", currentSurface->getID(),surfaceUpdateFps);
        currentSurface->updateCounter = 0;      
        LOG_INFO("X11WindowSystem", "Surface " << floatStringBuffer);        
        sprintf(floatStringBuffer, "0x%08x draw fps: %3.2f", currentSurface->getID(),surfaceDrawFps);
        currentSurface->drawCounter = 0;        
        LOG_INFO("X11WindowSystem", "Surface " << floatStringBuffer);
}

void X11WindowSystem::calculateFps()
{
    // we have rendered a frame
    Frame ++;
    std::list<Layer*> layers = m_pScene->getCurrentRenderOrder();
    // every 3 seconds, calculate & print fps
    gettimeofday(&tv, NULL);
    timeSinceLastCalc = (float)(tv.tv_sec-tv0.tv_sec) + 0.000001*((float)(tv.tv_usec-tv0.tv_usec));

    if (timeSinceLastCalc > 10.0f)
    {
        FPS = ((float)(Frame)) / timeSinceLastCalc;
        char floatStringBuffer[256];
        sprintf(floatStringBuffer, "Overall fps: %f", FPS);
        for(std::list<Layer*>::const_iterator current = layers.begin(); current != layers.end(); current++)
        {
            SurfaceList surfaceList = (*current)->getAllSurfaces();
            SurfaceListIterator surfaceIter = surfaceList.begin();
            SurfaceListIterator surfaceIterEnd = surfaceList.end();
            for(; surfaceIter != surfaceIterEnd ; ++surfaceIter)
            {
                calculateSurfaceFps((*surfaceIter),timeSinceLastCalc);
            }
        }
        LOG_INFO("X11WindowSystem", floatStringBuffer);
        tv0 = tv;
        Frame = 0;
    }
}

void X11WindowSystem::CheckRedrawAllLayers()
{
    std::list<Layer*> layers = m_pScene->getCurrentRenderOrder();
    for(std::list<Layer*>::const_iterator current = layers.begin(); current != layers.end(); current++)
    {
        Layer* currentLayer = (Layer*)*current;
        graphicSystem->beginLayer(currentLayer);
        graphicSystem->checkRenderLayer();
        graphicSystem->endLayer();
    }
}

void X11WindowSystem::RedrawAllLayers()
{
    std::list<Layer*> layers = m_pScene->getCurrentRenderOrder();

    // m_damaged represents that SW composition is required
    // At this point if a layer has damaged = true then it must be a HW layer that needs update.
    // A SW layer which needs update will make m_damaged = true
    if (m_forceComposition || m_damaged)
    {
        graphicSystem->activateGraphicContext();
        graphicSystem->clearBackground();
    }
    for(std::list<Layer*>::const_iterator current = layers.begin(); current != layers.end(); current++)
    {
        if ((*current)->getLayerType() == Hardware)
        {
            if (m_forceComposition || (*current)->damaged)
            {
                renderHWLayer(*current);
                (*current)->damaged = false;
            }
        }
        else if (m_forceComposition || m_damaged)
        {
            graphicSystem->beginLayer(*current);
            graphicSystem->renderSWLayer();
            graphicSystem->endLayer();
        }
    }
    if (m_forceComposition || m_damaged)
    {
        graphicSystem->swapBuffers();
        graphicSystem->releaseGraphicContext();
    }
}

void X11WindowSystem::renderHWLayer(Layer *layer)
{
    (void)layer;
}

void X11WindowSystem::Redraw()
{
    // draw all the layers
    //graphicSystem->clearBackground();
    /*LOG_INFO("X11WindowSystem","Locking List");*/
    m_pScene->lockScene();

    CheckRedrawAllLayers();
    RedrawAllLayers();

    m_pScene->unlockScene();

    if (m_forceComposition || m_damaged)
    {
        // TODO: This block won't be executed for HW only changes
        // Is that acceptable?
       if (debugMode)
        {
            printDebug();
        }

        calculateFps();

        /* Reset the damage flag, all is up to date */
        m_forceComposition = false;
        m_damaged = false;
    }
}

void X11WindowSystem::Screenshot()
{
    /*LOG_INFO("X11WindowSystem","Locking List");*/
    m_pScene->lockScene();
    graphicSystem->activateGraphicContext();
    graphicSystem->clearBackground();
    if (takeScreenshot==ScreenshotOfDisplay)
    {
        LOG_DEBUG("X11WindowSystem", "Taking screenshot");
        std::list<Layer*> layers = m_pScene->getCurrentRenderOrder();

        for(std::list<Layer*>::const_iterator current = layers.begin(); current != layers.end(); current++)
        {
            if ((*current)->getLayerType() != Hardware)
            {
                graphicSystem->beginLayer(*current);
                graphicSystem->renderSWLayer();
                graphicSystem->endLayer();
            }
        }
    }
    else if(takeScreenshot==ScreenshotOfLayer)
    {
        LOG_DEBUG("X11WindowSystem", "Taking screenshot of layer");
        Layer* currentLayer = m_pScene->getLayer(screenShotLayerID);

        if (currentLayer!=NULL){
            graphicSystem->beginLayer(currentLayer);
            graphicSystem->renderSWLayer();
            graphicSystem->endLayer();
        }
    }
    else if(takeScreenshot==ScreenshotOfSurface)
    {
        LOG_DEBUG("X11WindowSystem", "Taking screenshot of surface");
        Layer* currentLayer = m_pScene->getLayer(screenShotLayerID);
        Surface* currentSurface = m_pScene->getSurface(screenShotSurfaceID);

        if (currentLayer!=NULL && currentSurface!=NULL){
            graphicSystem->beginLayer(currentLayer);
            graphicSystem->renderSurface(currentSurface);
            graphicSystem->endLayer();
        }
    }

    graphicSystem->saveScreenShotOfFramebuffer(screenShotFile);
//  graphicSystem->swapBuffers();
    takeScreenshot = ScreenShotNone;
    LOG_DEBUG("X11WindowSystem", "Done taking screenshot");
    graphicSystem->releaseGraphicContext();
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
    if ( status == false ) goto init_complete;
    LOG_DEBUG("X11WindowSystem", "check for composite extension");
    status &= this->checkForCompositeExtension();
    if ( status == false ) goto init_complete;
    LOG_DEBUG("X11WindowSystem", "check for damage extension");
    status &= this->checkForDamageExtension();
    if ( status == false ) goto init_complete;
    LOG_DEBUG("X11WindowSystem", "init xserver");
    status &= this->initXServer();
    if ( status == false ) goto init_complete;
    status &= this->graphicSystem->init(this->x11Display,this->CompositorWindow);

init_complete:
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
#ifndef WITH_XTHREADS
        if ( XPending(this->x11Display) > 0) {
#endif //WITH_XTHREADS
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
                //           if (event.xreparent.parent == root)
                //               renderer->NewWindow(event.xreparent.window);
                //           else
                //               renderer->DestroyWindow(event.xreparent.window);
                break;
#ifdef WITH_INPUT_EVENTS
            case ButtonPress:
            case ButtonRelease:

            case MotionNotify:
                ManageXInputEvent(&event);
                break;
#endif
            default:
                if (event.type == this->damage_event + XDamageNotify)
                {
                    XDamageSubtract(this->x11Display, ((XDamageNotifyEvent*)(&event))->damage, None, None);
                    Surface* currentSurface = this->getSurfaceForWindow(((XDamageNotifyEvent*)(&event))->drawable);
                    if (currentSurface==NULL)
                    {
                        LOG_WARNING("X11WindowSystem", "Surface empty during damage notification");
                        break;
                    } else {
                        if (currentSurface->platform != NULL) 
                        {
                            /* Enable Rendering for Surface, after damage Notification was send successfully */
                            /* This will ensure, that the content is not dirty */
                            ((XPlatformSurface *)(currentSurface->platform))->enableRendering();
                        }
                    }
                    currentSurface->damaged = true;
                    currentSurface->updateCounter++;
                    checkRedraw = true;
                }
                break;
            }
            this->m_pScene->unlockScene();

#ifndef WITH_XTHREADS
        }
#endif //WITH_XTHREADS
        if (this->m_systemState == REDRAW_STATE)
        {
            LOG_DEBUG("X11WindowSystem", "Enter Redraw State");
            this->m_systemState = IDLE_STATE;

            // check if we are supposed to take screenshot
            if (this->takeScreenshot!=ScreenShotNone)
            {
                this->Screenshot();
            }
            else
            {
                this->checkForNewSurfaceNativeContent();
                checkRedraw = true;
            }

        } 
        else if (this->m_systemState == WAKEUP_STATE)         
        {
            LOG_DEBUG("X11WindowSystem", "Enter Wake Up State");
        }

        if (checkRedraw)
        {
            this->Redraw();
            checkRedraw = false;
        }
#ifndef WITH_XTHREADS
        else {
            /* put thread in sleep mode for 500 useconds due to safe cpu performance */

            usleep(500);
        }
#endif
    }
    this->cleanup();
    LOG_DEBUG("X11WindowSystem", "Renderer thread finished");
    return NULL;
}


void X11WindowSystem::ManageXInputEvent(XEvent *pevent)
{
    (void)pevent;

#ifdef WITH_INPUT_EVENTS
    XEvent* currentEvent = pevent;
    Surface * surf;
    int *pX = NULL;
    int *pY = NULL;

    switch (currentEvent->type)
    {
        case ButtonRelease:
        case ButtonPress:
            pX = &(((XButtonEvent*)currentEvent)->x);
            pY = &(((XButtonEvent*)currentEvent)->y);
            break;

        case MotionNotify:
            pX = &(((XMotionEvent*)currentEvent)->x);
            pY = &(((XMotionEvent*)currentEvent)->y);
            break;
        default:
            break;
    }

    surf = m_pScene->getSurfaceAt((unsigned int *) pX, (unsigned int *) pY, 0.1);
    if (surf != NULL)
    {
        pevent->xany.window = surf->getNativeContent();
        XSendEvent(x11Display, currentEvent->xany.window, false, 0, currentEvent);
    }
    currentEvent = NULL;
#endif
}


#ifdef WITH_XTHREADS
static Display* displaySignal = NULL;
#endif //WITH_XTHREADS
void X11WindowSystem::wakeUpRendererThread() 
{
#ifdef WITH_XTHREADS
    // send dummy expose event, to wake up blocking x11 event loop (XNextEvent)
    LOG_DEBUG("X11WindowSystem", "Sending dummy event to wake up renderer thread");
    if (NULL == displaySignal )
    {
        displaySignal = XOpenDisplay(m_displayEnvironment);
    }
    XExposeEvent ev = { Expose, 0, 1, displaySignal, CompositorWindow, 0, 0, 100, 100, 0 };
    XLockDisplay(displaySignal);
    XSendEvent(displaySignal, CompositorWindow, False, ExposureMask, (XEvent *) &ev);
    XUnlockDisplay(displaySignal);
    XFlush(displaySignal);
    LOG_DEBUG("X11WindowSystem", "Event successfully sent to renderer");
#endif //WITH_XTHREADS
}

void X11WindowSystem::signalRedrawEvent()
{
    // set flag that redraw is needed
    this->m_systemState = REDRAW_STATE;
    this->wakeUpRendererThread();
}

void X11WindowSystem::cleanup(){
    LOG_DEBUG("X11WindowSystem", "Cleanup");
    if (None != CompositorWindow)
    {
        Window root = RootWindow(x11Display, DefaultScreen(x11Display));
        XCompositeUnredirectSubwindows(x11Display,root,CompositeRedirectManual);
        XDestroyWindow(x11Display,CompositorWindow);
    }

    if (windowVis)
    {
        delete windowVis;
    }

#ifdef WITH_XTHREADS
    if ( NULL  != displaySignal )
    {
        XCloseDisplay(displaySignal);
    }
#endif //WITH_XTHREADS
    XCloseDisplay(x11Display);
    m_running = false;
}

bool X11WindowSystem::init(BaseGraphicSystem<Display*,Window>* base)
{
#ifdef WITH_XTHREADS
    XInitThreads();
#endif //WITH_XTHREADS
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
        LOG_DEBUG("X11WindowSystem","Waiting start complete " << m_initialized);
    }
    LOG_INFO("X11WindowSystem","Start complete " << m_initialized << " success " << m_success);
    return m_success;
}

bool X11WindowSystem::start()
{
    bool result = true;
    LOG_DEBUG("X11WindowSystem", "Starting / Creating thread");
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
    this->signalRedrawEvent();
    pthread_mutex_unlock(&run_lock);
    pthread_join(renderThread,NULL);
}

void X11WindowSystem::allocatePlatformSurface(Surface* surface)
{
    LOG_DEBUG("X11WindowSystem","allocatePlatformSurface begin");
    XPlatformSurface* nativeSurface = (XPlatformSurface*)surface->platform;
    if (!nativeSurface)
    {
        LOG_DEBUG("X11WindowSystem","creating native surface for new window");
        // this surface does not have a native platform surface attached yet!
        NewWindow(surface, surface->getNativeContent());
        MapWindow(surface->getNativeContent());
    }
    LOG_DEBUG("X11WindowSystem","allocatePlatformSurface end");
}

void X11WindowSystem::deallocatePlatformSurface(Surface* surface)
{
    LOG_DEBUG("X11WindowSystem","deallocatePlatformSurface begin");
    XPlatformSurface* nativeSurface = (XPlatformSurface*)surface->platform;
    if (nativeSurface)
    {
        LOG_DEBUG("X11WindowSystem","destroyingnative surface");
        graphicSystem->getTextureBinder()->destroyClientBuffer(surface);

        if (nativeSurface->pixmap)
        {
            int result = XFreePixmap(x11Display, nativeSurface->pixmap);
        }

        surface->renderPropertyChanged = true;
        delete surface->platform;
        surface->platform = NULL;
    }
    LOG_DEBUG("X11WindowSystem","deallocatePlatformSurface end");
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
    screenShotLayerID = id;
}

void X11WindowSystem::doScreenShotOfSurface(std::string fileName, const uint id, const uint layer_id)
{
    takeScreenshot = ScreenshotOfSurface;
    screenShotFile = fileName;
    screenShotSurfaceID = id;
    screenShotLayerID = layer_id;
}

