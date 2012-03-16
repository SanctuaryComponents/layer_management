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

#ifndef _WAYLANDWINDOWSYSTEM_H_
#define _WAYLANDWINDOWSYSTEM_H_
#include "WindowSystems/BaseWindowSystem.h"
#include "GraphicSystems/BaseGraphicSystem.h"
#include "Surface.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <wayland-server.h>
#include "PlatformSurfaces/WaylandPlatformSurface.h"
#include "Log.h"
#include "ScreenShotType.h"
#include "config.h"

extern "C" {
// TODO:to abstract
typedef enum waylandWindowSystemStates
{
    REDRAW_STATE = 0,
    WAKEUP_STATE = 1,
    IDLE_STATE = 2,
    UNKOWN_STATE    
} WaylandWindowSystemStates;

} // extern "C"

struct native_surface;

class WaylandWindowSystem: public BaseWindowSystem
{
public:
    WaylandWindowSystem(const char* displayname, int width, int height, Scene* pScene);
    virtual ~WaylandWindowSystem();
    bool init(BaseGraphicSystem<int, int>* sys);
    virtual bool start();
    virtual void stop();
    void signalRedrawEvent();
    void wakeUpRendererThread();
    void setSystemState(WaylandWindowSystemStates state);	// TODO:don't check state
    WaylandWindowSystemStates getSystemState();
    struct wl_display* getNativeDisplayHandle();
    virtual void allocatePlatformSurface(Surface *surface);
    void doScreenShot(std::string fileName);
    void doScreenShotOfLayer(std::string fileName, const uint id);
    void doScreenShotOfSurface(std::string fileName, const uint id, const uint layer_id);

protected:
    struct wl_display* m_wlDisplay;
    int m_fdFB;
    pthread_t renderThread;
    pthread_mutex_t run_lock;
    BaseGraphicSystem<int, int>* graphicSystem;    
    virtual void CheckRedrawAllLayers();
    virtual void RedrawAllLayers();

private:
    bool m_initialized;
    ScreenShotType m_takeScreenshot;
    const char* m_displayname;
    bool m_success;
    WaylandWindowSystemStates m_systemState;
    uint m_manageConnectionId;
    std::string m_screenShotFile;
    uint m_screenShotSurfaceID;
    uint m_screenShotLayerID;
    bool m_debugMode;
    bool m_error;
    struct wl_list m_listFrameCallback;	// TODO:confirm:what's this
    struct wl_shm* m_wlShm;
    struct wl_event_source* m_idleSource; // TODO:confirm:what's this
    int m_idleTime; // TODO:confirm:what's this

    void createServerinfo(WaylandWindowSystem* windowSystem);
    void addIdleEventRepaint();	// TODO:confirm:what's this
    struct native_surface* createNativeSurface();
    void postReleaseBuffer(struct wl_buffer *buffer);
    void attachBufferToNativeSurface(struct wl_buffer* buffer, struct wl_surface* surface); // ADIT TODO:reconfirm!!
    void repaint(int msecs);
    void cleanup();
    void Screenshot();
    void Redraw();
    bool initCompositor();
    void shutdownCompositor();
    Surface* getSurfaceFromNativeSurface(struct native_surface* nativeSurface);
    void checkForNewSurface();
    void printDebug();
    void* eventLoop();

    static void bindCompositor(struct wl_client* client, void* data, uint32_t version, uint32_t id);
    static int signalEventOnTerm(int signal_number, void *data);
    static void destroyListenerSurfaceBuffer(struct wl_listener* listener, struct wl_resource* resource, uint32_t time);
    static int timerEventIdle(void *data);
    static void idleEventRepaint(void *data);

public:
    static void serverinfoIFCreateConnection(struct wl_client *client, struct wl_resource *resource);
    static void bindServerinfo(struct wl_client *client, void *data, uint32_t version, uint32_t id);
    static uint32_t getTime(void);
    static void destroySurfaceCallback(struct wl_resource* resource);
    static void destroyFrameCallback(struct wl_resource *resource);
    static void* eventLoopCallback(void* ptr);

    // wl_surface interface
    static void surfaceIFDestroy(struct wl_client *client, struct wl_resource *resource);
    static void surfaceIFAttach(struct wl_client *client, struct wl_resource *resource, struct wl_resource *buffer_resource, int32_t x, int32_t y);
    static void surfaceIFDamage(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height);
    static void surfaceIFFrame(struct wl_client *client, struct wl_resource *resource, uint32_t callback);

    // wl_compositor interface
    static void compositorIFCreateSurface(struct wl_client *client, struct wl_resource* resource, uint32_t id);

    // wl_shm interface
    static void shmIFBufferCreated(struct wl_buffer *buffer);
    static void shmIFBufferDamaged(struct wl_buffer *buffer, int32_t x, int32_t y, int32_t width, int32_t height);
    static void shmIFBufferDestroyed(struct wl_buffer *buffer);

    struct wl_list m_connectionList;
};

inline void WaylandWindowSystem::setSystemState (WaylandWindowSystemStates state)
{
    m_systemState = state;
};
inline WaylandWindowSystemStates WaylandWindowSystem::getSystemState () 
{
    return m_systemState;
};

inline struct wl_display* WaylandWindowSystem::getNativeDisplayHandle()
{
    return m_wlDisplay;
}

extern "C" {
    struct native_surface {
        struct wl_surface surface;
        WaylandWindowSystem* windowSystem;
        struct wl_buffer* buffer;
        int connectionId;

        struct wl_list buffer_link; // TODO confirm:it's necessary or not
        uint32_t visual; // TODO confirm:it's necessary or not
        struct wl_listener buffer_destroy_listener; // TODO it's necessary or not
        struct wl_list link;
    };

    struct native_frame_callback {
        struct wl_resource resource;
        struct wl_list link;
    };

    struct native_process;
    typedef void (*cleanupFuncForNativeProcess)(struct native_process* process, int status);

    struct native_process {
        pid_t pid;
        cleanupFuncForNativeProcess cleanup;
        struct wl_list link;
    };
}

#endif /* _WAYLANDWINDOWSYSTEM_H_ */