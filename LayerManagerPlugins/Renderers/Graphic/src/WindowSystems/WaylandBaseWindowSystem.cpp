/***************************************************************************
*
* Copyright 2010, 2011 BMW Car IT GmbH
* Copyright (C) 2011 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
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
*
* THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
* FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
* SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
* RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
* CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
* CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*
****************************************************************************/

#include "WindowSystems/WaylandBaseWindowSystem.h"
#include "Log.h"
#include "Layer.h"
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iomanip>

#include "WindowSystems/WaylandServerinfoServerProtocol.h"

extern "C" {
    struct serverinfo {
        struct wl_resource base;
        WaylandBaseWindowSystem* windowSystem;
    };

    struct serverinfoClient {
        struct wl_client *client;
        uint connectionId;
        struct wl_list link;
    };

    void WaylandBaseWindowSystem::serverinfoIFCreateConnection(struct wl_client *client, struct wl_resource *resource)
    {
        struct serverinfo* deliver = (struct serverinfo*)resource->data;

        // creates new connection id and store it in memory
        struct serverinfoClient* clientPair = (struct serverinfoClient*)malloc( sizeof *clientPair );

        clientPair->client = client;
        clientPair->connectionId = deliver->windowSystem->m_manageConnectionId;
        deliver->windowSystem->m_manageConnectionId++;

        wl_list_init(&clientPair->link);
        wl_list_insert(&deliver->windowSystem->m_connectionList, &clientPair->link);

        // send native client handle to this client.
        // by protocol default, this information is not needed.
        wl_resource_post_event(resource, SERVERINFO_CONNECTION_ID, clientPair->connectionId);
        LOG_DEBUG("WaylandBaseWindowSystem", "serverinfoIFCreateConnection() create connection id" << clientPair->connectionId << " for client " << clientPair->client);
    }

    struct serverinfo_interface g_serverinfoImplementation = {
        WaylandBaseWindowSystem::serverinfoIFCreateConnection,
    };

    void WaylandBaseWindowSystem::bindServerinfo(struct wl_client *client, void *data, uint32_t version, uint32_t id)
    {
        LOG_INFO("WaylandBaseWindowSystem", "bindServerinfo client:" << client << ", data:" << data << ", version:" << version << ", id:" << id);
        wl_client_add_object(client, &serverinfo_interface, &g_serverinfoImplementation, id, data);
    }

    void WaylandBaseWindowSystem::createServerinfo(WaylandBaseWindowSystem* windowSystem)
    {
        struct serverinfo* serverInfo;

        serverInfo = (struct serverinfo*)malloc(sizeof *serverInfo);
        if (NULL == serverInfo){
            LOG_ERROR("WaylandBaseWindowSystem", "failed to alloc serverinfo");
            return;
        }

        serverInfo->base.object.interface = &serverinfo_interface;
        serverInfo->base.object.implementation = (void(**)(void)) &g_serverinfoImplementation;
        serverInfo->base.client = NULL;
        serverInfo->base.data = NULL;
        serverInfo->windowSystem = windowSystem;

        wl_display_add_global(windowSystem->m_wlDisplay, &serverinfo_interface, serverInfo, WaylandBaseWindowSystem::bindServerinfo);
    };
}

WaylandBaseWindowSystem::WaylandBaseWindowSystem(const char* displayname, int width, int height, Scene* pScene)
: BaseWindowSystem(pScene)
, m_initialized(false)
, m_takeScreenshot(ScreenShotNone)
, m_displayname(displayname)
, m_success(false)
, m_systemState(IDLE_STATE)
, m_manageConnectionId(256)
, m_debugMode(false)
, m_error(false)
, m_width(width)
, m_height(height)
{
    LOG_DEBUG("WaylandBaseWindowSystem", "creating WaylandBaseWindowSystem width:" << width << " height:" << height);

    // init and take mutex, so windowsystem only does init phase until mutex is released again
    pthread_mutex_init(&run_lock, NULL);
    pthread_mutex_lock(&run_lock);
}

WaylandBaseWindowSystem::~WaylandBaseWindowSystem()
{
}

void WaylandBaseWindowSystem::printDebug()
{
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
    LOG_DEBUG("WaylandBaseWindowSystem",debugmessage.str());
}

Surface* WaylandBaseWindowSystem::getSurfaceFromNativeSurface(struct native_surface* nativeSurface)
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
	WaylandPlatformSurface* nativePlatform = (WaylandPlatformSurface*)currentSurface->platform;
        if (!nativePlatform)
        {
            continue;
        }
        if (nativePlatform->connectionId != nativeSurface->connectionId)
        {
            continue;
        }
        if (nativePlatform->surfaceId != nativeSurface->surface.resource.object.id)
        {
            continue;
        }
        return currentSurface;
    }
    LOG_DEBUG("WaylandBaseWindowSystem", "could not find surface for surface " << nativeSurface);
    return NULL;
}

void WaylandBaseWindowSystem::checkForNewSurface()
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
        }
    }
    m_pScene->unlockScene();
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
        LOG_INFO("WaylandBaseWindowSystem", floatStringBuffer);
        tv0 = tv;
        Frame = 0;
    }
}

void WaylandBaseWindowSystem::CheckRedrawAllLayers()
{
    graphicSystem->activateGraphicContext();
    std::list<Layer*> layers = m_pScene->getCurrentRenderOrder();
    for(std::list<Layer*>::const_iterator current = layers.begin(); current != layers.end(); current++)
    {
        Layer* currentLayer = (Layer*)*current;
        graphicSystem->beginLayer(currentLayer);
        graphicSystem->checkRenderLayer();
        graphicSystem->endLayer();
    }
    graphicSystem->releaseGraphicContext();
}

void WaylandBaseWindowSystem::RedrawAllLayers()
{
    std::list<Layer*> layers = m_pScene->getCurrentRenderOrder();
    for(std::list<Layer*>::const_iterator current = layers.begin(); current != layers.end(); current++)
    {
        graphicSystem->beginLayer(*current);
        graphicSystem->renderLayer();
        graphicSystem->endLayer();
    }
}

void WaylandBaseWindowSystem::Redraw()
{
    // draw all the layers
    //graphicSystem->clearBackground();
    /*LOG_INFO("WaylandBaseWindowSystem","Locking List");*/
    m_pScene->lockScene();

    CheckRedrawAllLayers();
    if (m_damaged)
    {
        graphicSystem->activateGraphicContext();
        graphicSystem->clearBackground();
        RedrawAllLayers();
        graphicSystem->swapBuffers();
        graphicSystem->releaseGraphicContext();
        m_pScene->unlockScene();
        if (m_debugMode)
        {
            printDebug();
        }

        CalculateFPS();

        /* Reset the damage flag, all is up to date */
        m_damaged = false;
    }
    else
    {
        m_pScene->unlockScene();
        /*LOG_INFO("WaylandBaseWindowSystem","UnLocking List");*/
    }
}

void WaylandBaseWindowSystem::Screenshot()
{
    /*LOG_INFO("WaylandBaseWindowSystem","Locking List");*/
    m_pScene->lockScene();
    graphicSystem->clearBackground();
    graphicSystem->activateGraphicContext();

    if (m_takeScreenshot==ScreenshotOfDisplay){
    LOG_DEBUG("WaylandBaseWindowSystem", "Taking screenshot");
        RedrawAllLayers();
    }else if(m_takeScreenshot==ScreenshotOfLayer){
        LOG_DEBUG("WaylandBaseWindowSystem", "Taking screenshot of layer");
        Layer* currentLayer = m_pScene->getLayer(m_screenShotLayerID);
        if (currentLayer!=NULL){
            graphicSystem->beginLayer(currentLayer);
            graphicSystem->renderLayer();
            graphicSystem->endLayer();
        }
    }else if(m_takeScreenshot==ScreenshotOfSurface){
        LOG_DEBUG("WaylandBaseWindowSystem", "Taking screenshot of surface");
        Layer* currentLayer = m_pScene->getLayer(m_screenShotLayerID);
        Surface* currentSurface = m_pScene->getSurface(m_screenShotSurfaceID);
        if (currentLayer!=NULL && currentSurface!=NULL){
            graphicSystem->beginLayer(currentLayer);
            graphicSystem->renderSurface(currentSurface);
            graphicSystem->endLayer();
        }
    }

    graphicSystem->saveScreenShotOfFramebuffer(m_screenShotFile);
//  graphicSystem->swapBuffers();
    m_takeScreenshot = ScreenShotNone;
    LOG_DEBUG("WaylandBaseWindowSystem", "Done taking screenshot");

    graphicSystem->releaseGraphicContext();
    m_pScene->unlockScene();
    /*LOG_INFO("WaylandBaseWindowSystem","UnLocking List");*/
}

void WaylandBaseWindowSystem::destroyListenerSurfaceBuffer(struct wl_listener* listener, struct wl_resource* resource, uint32_t time)
{
    LOG_INFO("WaylandBaseWindowSystem", "destroyListenerSurfaceBuffer listener:" << listener << ", resource:" << resource << ", time:" << time);
    struct native_surface *es = container_of(listener, struct native_surface, buffer_destroy_listener);

    es->buffer = NULL;
}

struct native_surface* WaylandBaseWindowSystem::createNativeSurface()
{
    LOG_DEBUG("WaylandBaseWindowSystem", "createNativeSurface IN");
    struct native_surface* surface = (struct native_surface*)calloc(1, sizeof *surface);
    if (NULL == surface)
    {
        LOG_ERROR("WaylandBaseWindowSystem", "failed to create native surface");
        return NULL;
    }

    wl_list_init(&surface->link);
    wl_list_init(&surface->buffer_link);

    surface->surface.resource.client = NULL;

    surface->windowSystem = this;
    // TODO visual
    // surface->visual = NONE_VISUAL;

    surface->buffer = NULL;
    surface->buffer_destroy_listener.func = destroyListenerSurfaceBuffer;

    LOG_DEBUG("WaylandBaseWindowSystem", "createNativeSurface OUT");
    return surface;
}

uint32_t WaylandBaseWindowSystem::getTime(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void WaylandBaseWindowSystem::destroySurfaceCallback(struct wl_resource* resource)
{
    struct native_surface* nativeSurface = container_of(resource, struct native_surface, surface.resource);
#if 0 /* ADIT TODO */
    Surface* surface = getSurfaceFromNativeSurface(struct native_surface* nativeSurface);

    WaylandBaseWindowSystem* windowSystem = static_cast<WaylandBaseWindowSystem*>( (WaylandBaseWindowSystem*)surface->windowSystem);
    windowSystem->graphicSystem->getTextureBinder()->destroyClientBuffer(nativeSurface);
#endif /* ADIT TODO */

    wl_list_remove(&nativeSurface->link);
    wl_list_remove(&nativeSurface->buffer_link);

    if (nativeSurface->buffer)
    {
        wl_list_remove(&nativeSurface->buffer_destroy_listener.link);
    }

    free(nativeSurface);
}

extern "C" void WaylandBaseWindowSystem::surfaceIFDestroy(struct wl_client *client, struct wl_resource *resource)
{
    LOG_INFO("WaylandBaseWindowSystem", "surfaceIFDestroy client:" << client << ", resource:" << resource);
    wl_resource_destroy(resource, getTime());
}

void WaylandBaseWindowSystem::postReleaseBuffer(struct wl_buffer *buffer)
{
    LOG_DEBUG("WaylandBaseWindowSystem", "postReleaseBufferIN");
    if (--buffer->busy_count > 0)
    {
        return;
    }

    if (NULL == buffer->resource.client)
    {
        LOG_ERROR("WaylandBaseWindowSystem", "Release client is NULL");
    }
    wl_resource_queue_event(&buffer->resource, WL_BUFFER_RELEASE);
    LOG_DEBUG("WaylandBaseWindowSystem", "postReleaseBuffer OUT");
}

void WaylandBaseWindowSystem::attachBufferToNativeSurface(struct wl_buffer* buffer, struct wl_surface* surface)
{
    LOG_DEBUG("WaylandBaseWindowSystem", "attachBufferToNativeSurface IN");
    struct native_surface* nativeSurface = (struct native_surface*)surface;
    WaylandBaseWindowSystem* windowSystem = static_cast<WaylandBaseWindowSystem*>( (WaylandBaseWindowSystem*)nativeSurface->windowSystem);
    Surface* ilmSurface = windowSystem->getSurfaceFromNativeSurface(nativeSurface);
    if (NULL == ilmSurface)
    {
        LOG_ERROR("WaylandBaseWindowSystem", "failed to get surface for wl_surface");
        return;
    }
    struct wl_list* surfaces_attached_to;

    if (wl_buffer_is_shm(buffer))
    {
        LOG_DEBUG("WaylandBaseWindowSystem", "shm buffer" << buffer);
        ilmSurface->removeNativeContent();
        ilmSurface->setNativeContent((long int)buffer);

        WaylandPlatformSurface* nativePlatformSurface = (WaylandPlatformSurface*)ilmSurface->platform;
        if (0 != nativePlatformSurface)
        {
            windowSystem->graphicSystem->getTextureBinder()->createClientBuffer(ilmSurface);
            LOG_DEBUG("WaylandBaseWindowSystem","nativePlatformSurface->enable");
            nativePlatformSurface->enableRendering();
        }

        // TODO:only ARGB32.
        //switch (wl_shm_buffer_get_format(buffer)) {
        //case WL_SHM_FORMAT_ARGB32:
        //es->visual = WLSC_ARGB_VISUAL;
        //break;
        //case WL_SHM_FORMAT_PREMULTIPLIED_ARGB32:
        //es->visual = WLSC_PREMUL_ARGB_VISUAL;
        //break;
        //case WL_SHM_FORMAT_XRGB32:
        //es->visual = WLSC_RGB_VISUAL;
        //break;
        //}

        surfaces_attached_to = (wl_list*)buffer->user_data;

        wl_list_remove(&nativeSurface->buffer_link);
        wl_list_insert(surfaces_attached_to, &nativeSurface->buffer_link);
    }
    else {
        LOG_DEBUG("WaylandBaseWindowSystem", "wl buffer");

        ilmSurface->removeNativeContent();
        ilmSurface->setNativeContent((long int)buffer);

        WaylandPlatformSurface* nativePlatformSurface = (WaylandPlatformSurface*)ilmSurface->platform;
        if (0 != nativePlatformSurface)
        {
            windowSystem->graphicSystem->getTextureBinder()->createClientBuffer(ilmSurface);
            LOG_DEBUG("WaylandBaseWindowSystem","nativePlatformSurface->enable");
            nativePlatformSurface->enableRendering();
        }

        // TODO: we need to get the visual from the wl_buffer */
        // es->visual = WLSC_PREMUL_ARGB_VISUAL;
        // es->pitch = es->width;
    }
    LOG_DEBUG("WaylandBaseWindowSystem", "attachBufferToNativeSurface OUT");
}

extern "C" void WaylandBaseWindowSystem::surfaceIFAttach(struct wl_client* client,
	       struct wl_resource* resource,
	       struct wl_resource* buffer_resource, int32_t x, int32_t y)
{
    LOG_INFO("WaylandBaseWindowSystem", "surfaceIFAttach client:" << client << ", resource:" << resource << ", buffer_resource:" << buffer_resource << ", x:" << x << ", y:" << y);
    LOG_DEBUG("WaylandBaseWindowSystem", "surfaceIFAttach IN");
    struct native_surface* nativeSurface = (struct native_surface*)resource->data;
    WaylandBaseWindowSystem* windowSystem = static_cast<WaylandBaseWindowSystem*>( (WaylandBaseWindowSystem*)nativeSurface->windowSystem);
    struct wl_buffer* buffer = (struct wl_buffer*)buffer_resource->data;

    if (nativeSurface->buffer)
    {
        windowSystem->postReleaseBuffer(nativeSurface->buffer);
        wl_list_remove(&nativeSurface->buffer_destroy_listener.link);
    }

    buffer->busy_count++;
    nativeSurface->buffer = buffer;
    wl_list_insert(nativeSurface->buffer->resource.destroy_listener_list.prev, &nativeSurface->buffer_destroy_listener.link);

    windowSystem->attachBufferToNativeSurface(buffer, &nativeSurface->surface);

    LOG_DEBUG("WaylandBaseWindowSystem", "surfaceIFAttach OUT");
}

extern "C" void WaylandBaseWindowSystem::surfaceIFDamage(struct wl_client *client,
	       struct wl_resource *resource,
	       int32_t x, int32_t y, int32_t width, int32_t height)
{
    LOG_INFO("WaylandBaseWindowSystem", "surfaceIFDamage client:" << client << ", resource:" << resource << ", x:" << x << ", y:" << y << ", width:" << width << ", height:" << height);
    LOG_DEBUG("WaylandBaseWindowSystem", "surfaceIFDamage IN");
    struct native_surface* nativeSurface = (struct native_surface*)resource->data;
    WaylandBaseWindowSystem* windowSystem = static_cast<WaylandBaseWindowSystem*>( (WaylandBaseWindowSystem*)nativeSurface->windowSystem);

    Surface* surface = windowSystem->getSurfaceFromNativeSurface(nativeSurface);
    if (NULL == surface)
    {
        LOG_ERROR("WaylandBaseWindowSystem", "invalid surface");
        return;
    }
    surface->damaged = true;

    LOG_DEBUG("WaylandBaseWindowSystem", "surfaceIFDamage OUT");
}

extern "C" void WaylandBaseWindowSystem::destroyFrameCallback(struct wl_resource *resource)
{
    struct native_frame_callback* cb = (struct native_frame_callback*)resource->data;

    wl_list_remove(&cb->link);
    free(cb);
}

extern "C" void WaylandBaseWindowSystem::surfaceIFFrame(struct wl_client *client,
	      struct wl_resource *resource, uint32_t callback)
{
    LOG_DEBUG("WaylandBaseWindowSystem", "surfaceIFFrame IN");
    struct native_frame_callback* cb;
    struct native_surface* es = (struct native_surface*)resource->data;
    WaylandBaseWindowSystem* windowSystem = static_cast<WaylandBaseWindowSystem*>( (WaylandBaseWindowSystem*)es->windowSystem);

    cb = (struct native_frame_callback*)malloc(sizeof *cb);
    if (NULL == cb)
    {
        wl_resource_post_no_memory(resource);
        return;
    }

    cb->resource.object.interface = &wl_callback_interface;
    cb->resource.object.id = callback;
    cb->resource.destroy = destroyFrameCallback;
    cb->resource.client = client;
    cb->resource.data = cb;

    wl_client_add_resource(client, &cb->resource);
    wl_list_insert(windowSystem->m_listFrameCallback.prev, &cb->link);

    windowSystem->checkForNewSurface();
    idleEventRepaint(windowSystem);

    LOG_DEBUG("WaylandBaseWindowSystem", "surfaceIFFrame OUT");
}

extern "C" const struct wl_surface_interface g_surfaceInterface = {
    WaylandBaseWindowSystem::surfaceIFDestroy,
    WaylandBaseWindowSystem::surfaceIFAttach,
    WaylandBaseWindowSystem::surfaceIFDamage,
    WaylandBaseWindowSystem::surfaceIFFrame
};

extern "C" void WaylandBaseWindowSystem::compositorIFCreateSurface
		(struct wl_client *client, struct wl_resource* resource, uint32_t id)
{
    LOG_DEBUG("WaylandBaseWindowSystem", "compositorIFCreateSurface IN");
    WaylandBaseWindowSystem* windowSystem = static_cast<WaylandBaseWindowSystem*>( (WaylandBaseWindowSystem*) resource->data);
    struct native_surface* surface;

    surface = windowSystem->createNativeSurface();
    if (NULL == surface)
    {
        wl_resource_post_no_memory(resource);
        return;
    }

    surface->surface.resource.destroy = destroySurfaceCallback;
    surface->windowSystem = windowSystem;

    surface->surface.resource.object.id = id;
    surface->surface.resource.object.interface = &wl_surface_interface;
    surface->surface.resource.object.implementation = (void (**)(void))&g_surfaceInterface;
    surface->surface.resource.data = surface;

    struct serverinfoClient* serverinfoPairNode;

    wl_list_for_each(serverinfoPairNode, &windowSystem->m_connectionList, link) {
        if (serverinfoPairNode->client != client)
        {
            continue;
        }
        surface->connectionId = serverinfoPairNode->connectionId;
        break;
    }

    wl_client_add_resource(client, &surface->surface.resource);
    LOG_DEBUG("WaylandBaseWindowSystem", "compositorIFCreateSurface OUT");
}

const static struct wl_compositor_interface g_compositorInterface = {
    WaylandBaseWindowSystem::compositorIFCreateSurface,
};

extern "C" void WaylandBaseWindowSystem::shmIFBufferCreated(struct wl_buffer *buffer)
{
    LOG_DEBUG("WaylandBaseWindowSystem", "shmIFBufferCreated IN");
    struct wl_list* surfaces_attached_to;

    surfaces_attached_to = (struct wl_list*)malloc(sizeof *surfaces_attached_to);
    if (NULL == surfaces_attached_to)
    {
        buffer->user_data = NULL;
        return;
    }

    wl_list_init(surfaces_attached_to);

    buffer->user_data = surfaces_attached_to;
    LOG_DEBUG("WaylandBaseWindowSystem", "shmIFBufferCreated OUT");
}

extern "C" void WaylandBaseWindowSystem::shmIFBufferDamaged(struct wl_buffer* buffer, int32_t x, int32_t y, int32_t width, int32_t height)
{
    LOG_INFO("WaylandBaseWindowSystem", "shmIFBufferDamaged buffer:" << buffer << ", x:" << x << ", y:" << y << ", width:" << width << ", height:" << height);
    LOG_DEBUG("WaylandBaseWindowSystem", "shmIFBufferDamaged IN");
    struct wl_list* surfaces_attached_to = (struct wl_list*)buffer->user_data;
    struct native_surface* nativeSurface;

    wl_list_for_each(nativeSurface, surfaces_attached_to, buffer_link) {
#if 0 /* ADIT TODO */
        glBindTexture(GL_TEXTURE_2D, es->texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, tex_width, buffer->height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, wl_shm_buffer_get_data(buffer));
        /* Hmm, should use glTexSubImage2D() here but GLES2 doesn't
         * support any unpack attributes except GL_UNPACK_ALIGNMENT. */
#endif /* ADIT TODO */
    }
    LOG_DEBUG("WaylandBaseWindowSystem", "shmIFBufferDamaged OUT");
}

extern "C" void WaylandBaseWindowSystem::shmIFBufferDestroyed(struct wl_buffer *buffer)
{
    LOG_DEBUG("WaylandBaseWindowSystem", "shmIFBufferDestroyed IN");
    struct wl_list* surfaces_attached_to = (struct wl_list*)buffer->user_data;
    struct native_surface* nativeSurface;
    struct native_surface* next;

    wl_list_for_each_safe(nativeSurface, next, surfaces_attached_to, buffer_link)
    {
        wl_list_remove(&nativeSurface->buffer_link);
        wl_list_init(&nativeSurface->buffer_link);
    }

    free(surfaces_attached_to);
    LOG_DEBUG("WaylandBaseWindowSystem", "shmIFBufferDestroyed OUT");
}

const static struct wl_shm_callbacks g_shmCallbacks = {
    WaylandBaseWindowSystem::shmIFBufferCreated,
    WaylandBaseWindowSystem::shmIFBufferDamaged,
    WaylandBaseWindowSystem::shmIFBufferDestroyed
};

/**
 * Thread in charge of the CompositorWindow eventloop
 * Friend function of class WaylandBaseWindowSystem
 */
extern "C" void* WaylandBaseWindowSystem::eventLoopCallback(void *ptr)
{
    WaylandBaseWindowSystem *windowsys = static_cast<WaylandBaseWindowSystem*>( (WaylandBaseWindowSystem*) ptr);
    return windowsys->eventLoop();
}

void WaylandBaseWindowSystem::bindCompositor(struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
    LOG_INFO("WaylandBaseWindowSystem", "bindCompositor client:" << client << ", data:" << data << ", version:" << version << ", id:" << id);
    wl_client_add_object(client, &wl_compositor_interface, &g_compositorInterface, id, data);
}

void WaylandBaseWindowSystem::repaint(int msecs)
{
    LOG_DEBUG("WaylandBaseWindowSystem", "repaint IN");
    struct native_frame_callback* cb;
    struct native_frame_callback* cnext;

    Redraw();

    wl_event_source_timer_update(m_idleSource, 10);

    wl_list_for_each_safe(cb, cnext, &m_listFrameCallback, link)
    {
        wl_resource_post_event(&cb->resource, WL_CALLBACK_DONE, msecs);
        wl_resource_destroy(&cb->resource, 0);
    }
    LOG_DEBUG("WaylandBaseWindowSystem", "repaint OUT");
}

void WaylandBaseWindowSystem::idleEventRepaint(void *data)
{
    WaylandBaseWindowSystem* windowSystem = static_cast<WaylandBaseWindowSystem*>( (WaylandBaseWindowSystem*)data);
    LOG_DEBUG("WaylandBaseWindowSystem", "idleEventRepaint IN");
    windowSystem->repaint(getTime());
    LOG_DEBUG("WaylandBaseWindowSystem", "idleEventRepaint OUT");
}

int WaylandBaseWindowSystem::timerEventIdle(void *data)
{
    WaylandBaseWindowSystem* windowSystem = static_cast<WaylandBaseWindowSystem*>( (WaylandBaseWindowSystem*)data);

    LOG_DEBUG("WaylandBaseWindowSystem", "timerEventIdle IN");
    windowSystem->Redraw();
    LOG_DEBUG("WaylandBaseWindowSystem", "timerEventIdle OUT");

    return 1;
}

void WaylandBaseWindowSystem::addIdleEventRepaint()
{
    LOG_DEBUG("WaylandBaseWindowSystem", "addIdleEventRepaint IN");

    struct wl_event_loop* loop;
    loop = wl_display_get_event_loop(m_wlDisplay);
    wl_event_loop_add_idle(loop, idleEventRepaint, this);

    LOG_DEBUG("WaylandBaseWindowSystem", "addIdleEventRepaint OUT");
}


bool WaylandBaseWindowSystem::initCompositor()
{
    LOG_DEBUG("WaylandBaseWindowSystem", "initCompositor START");
    long int status = 0;

    status = (long int)wl_display_add_global(m_wlDisplay, &wl_compositor_interface, this, bindCompositor);
    if (0 == status)
    {
        LOG_ERROR("WaylandBaseWindowSystem", "wl_display_add_global:failed to set wl_compositor_interface");
        return false;
    }
    LOG_DEBUG("WaylandBaseWindowSystem", "wl_display_add_global:SUCCESS");

    m_wlShm = wl_shm_init(m_wlDisplay, &g_shmCallbacks);

    wl_list_init(&m_listFrameCallback);

    wl_list_init(&m_connectionList);
    createServerinfo(this);

    struct wl_event_loop *loop;
    loop = wl_display_get_event_loop(m_wlDisplay);
    m_idleSource = wl_event_loop_add_timer(loop, timerEventIdle, this);
    wl_event_source_timer_update(m_idleSource, m_idleTime * 1000);

    addIdleEventRepaint();

    LOG_DEBUG("WaylandBaseWindowSystem", "initCompositor END");
    return true;
}

void WaylandBaseWindowSystem::shutdownCompositor()
{
}

int WaylandBaseWindowSystem::signalEventOnTerm(int signal_number, void *data)
{
    struct wl_display* display = (struct wl_display*)data;

    LOG_ERROR("WaylandBaseWindowSystem", "caught signal " << signal_number);
    wl_display_terminate(display);

    return 1;
}

void* WaylandBaseWindowSystem::eventLoop()
{
    // INITALIZATION
    LOG_DEBUG("WaylandBaseWindowSystem", "Enter thread");

    int option_idle_time = 3000;
    bool status = true;
    struct wl_event_loop *loop;

    do
    {
        m_wlDisplay = wl_display_create();

        if (NULL == m_wlDisplay)
        {
            LOG_ERROR("WaylandBaseWindowSystem", "failed to create wayland display");
            break;
        }
        LOG_DEBUG("WaylandBaseWindowSystem", "create wayland display");

        loop = wl_display_get_event_loop(m_wlDisplay);
        wl_event_loop_add_signal(loop, SIGTERM, signalEventOnTerm, m_wlDisplay);
        wl_event_loop_add_signal(loop, SIGINT, signalEventOnTerm, m_wlDisplay);
        wl_event_loop_add_signal(loop, SIGQUIT, signalEventOnTerm, m_wlDisplay);
        wl_event_loop_add_signal(loop, SIGKILL, signalEventOnTerm, m_wlDisplay);

        LOG_DEBUG("WaylandBaseWindowSystem", "wl_event_loop_add_signal");

        this->m_idleTime = option_idle_time;

        status = this->initCompositor();
        if (false == status)
        {
            LOG_ERROR("WaylandBaseWindowSystem", "failed to init compositor");
            break;
        }
        LOG_DEBUG("WaylandBaseWindowSystem", "SUCCESS:initCompositor");

        //. create Native Context
        status = createNativeContext();
        if (false == status)
        {
            LOG_ERROR("WaylandBaseWindowSystem", "failed to create Native context");
            break;
        }
        LOG_DEBUG("WaylandBaseWindowSystem", "SUCCESS:createNativeContext");

        //. create egl context
        status = initGraphicSystem();
        if (false == status)
        {
            LOG_ERROR("WaylandBaseWindowSystem", "failed to init graphicSystem");
            break;
        }
        LOG_DEBUG("WaylandBaseWindowSystem", "SUCCESS:init GraphicSystem");

        this->m_success = status;
        this->m_initialized = true;

        // Done with init, wait for lock to actually run (ie start/stop method called)
        pthread_mutex_lock(&this->run_lock);

        LOG_DEBUG("WaylandBaseWindowSystem", "Starting Event loop");

        // run the main event loop while rendering
        gettimeofday(&tv0, NULL);
        LOG_DEBUG("WaylandBaseWindowSystem", "Enter render loop");

        // clear screen to avoid garbage on startup
        this->graphicSystem->clearBackground();
        this->graphicSystem->swapBuffers();

        if (wl_display_add_socket(m_wlDisplay, NULL))
        {
            LOG_ERROR("WaylandBaseWindowSystem", "failed to add socket");
                this->m_success = false;
                this->m_initialized = false;
                break;
        }

        wl_display_run(m_wlDisplay);

    } while(0);

    this->cleanup();
    LOG_DEBUG("WaylandBaseWindowSystem", "Renderer thread finished");
    return NULL;
}

void WaylandBaseWindowSystem::wakeUpRendererThread()
{
}

void WaylandBaseWindowSystem::signalRedrawEvent()
{
    LOG_DEBUG("WaylandBaseWindowSystem", "signalRedrawEvent");
    // set flag that redraw is needed
    this->m_systemState = REDRAW_STATE;
    this->wakeUpRendererThread();
}

void WaylandBaseWindowSystem::cleanup()
{
    LOG_DEBUG("WaylandBaseWindowSystem", "Cleanup");

#if 1 /* ADIT TODO */
    shutdownCompositor();
#endif /* ADIT TODO */

#if 0 /* ADIT */
    if (NULL != m_wlDisplay)
    {
        wl_display_destroy(m_wlDisplay);
        m_wlDisplay = NULL;
    }
#endif /* ADIT */
}

bool WaylandBaseWindowSystem::init(BaseGraphicSystem<EGLNativeDisplayType, EGLNativeWindowType>* base)
{
    LOG_INFO("WaylandBaseWindowSystem", "init base:" << base);
    graphicSystem = base;
    int status = pthread_create(&renderThread, NULL, eventLoopCallback, (void*)this);
    if (0 != status)
    {
        return false;
    }

    while (false == m_initialized)
    {
        usleep(10000); // TODO
        LOG_DEBUG("WaylandBaseWindowSystem","Waiting start compositor complete " << m_initialized);
    }
    LOG_INFO("WaylandBaseWindowSystem","Start complete [connect display]" << m_initialized << " success " << m_success);
    return m_success;
}

bool WaylandBaseWindowSystem::start()
{
    bool result = true;
    LOG_DEBUG("WaylandBaseWindowSystem", "Starting / Creating thread");
    // let thread actually run
    if ( m_error == false )
    {
        pthread_mutex_unlock(&run_lock);
    } else {
        pthread_mutex_unlock(&run_lock);
        result = false;
    }
    return result;
}

void WaylandBaseWindowSystem::stop()
{
    LOG_INFO("WaylandBaseWindowSystem","Stopping..");
    // needed if start was never called, we wake up thread, so it can immediatly finish
    // this->signalRedrawEvent();
    if (NULL != m_wlDisplay)
    {
        wl_display_terminate(m_wlDisplay);
    }
    pthread_mutex_unlock(&run_lock);
    pthread_join(renderThread, NULL);
}

void WaylandBaseWindowSystem::allocatePlatformSurface(Surface* surface)
{
    LOG_INFO("WaylandBaseWindowSystem","allocatePlatformSurface begin");
    WaylandPlatformSurface* nativeSurface = (WaylandPlatformSurface*)surface->platform;
    if (!nativeSurface)
    {
        LOG_DEBUG("WaylandBaseWindowSystem","creating native surface for new window");
        // this surface does not have a native platform surface attached yet!
        nativeSurface = (WaylandPlatformSurface*)graphicSystem->getTextureBinder()->createPlatformSurface(surface);
        if (0 != nativeSurface)
        {
            unsigned int surfaceId = surface->getNativeContent();
            LOG_DEBUG("WaylandBaseWindowSystem","surface->getNativeContent()"<<surfaceId);
            nativeSurface->connectionId = (unsigned short)((surfaceId >> 16) & 0xFFFF);
            nativeSurface->surfaceId = (unsigned short)(surfaceId & 0xFFFF);
            surface->platform = nativeSurface;
        }
        else
        {
            LOG_ERROR("WaylandBaseWindowSystem","failed to allocate platformsurface");
        }
    }
    LOG_INFO("WaylandBaseWindowSystem","allocatePlatformSurface end");
}

void WaylandBaseWindowSystem::doScreenShot(std::string fileName)
{
    m_takeScreenshot = ScreenshotOfDisplay;
    m_screenShotFile = fileName;
}

void WaylandBaseWindowSystem::doScreenShotOfLayer(std::string fileName, const uint id)
{
    m_takeScreenshot = ScreenshotOfLayer;
    m_screenShotFile = fileName;
    m_screenShotLayerID = id;
}

void WaylandBaseWindowSystem::doScreenShotOfSurface(std::string fileName, const uint id, const uint layer_id)
{
    m_takeScreenshot = ScreenshotOfSurface;
    m_screenShotFile = fileName;
    m_screenShotSurfaceID = id;
    m_screenShotLayerID = layer_id;
}
