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
 ****************************************************************************/
#ifndef _OpenGLES2App_h
#define _OpenGLES2App_h

#include "wayland-client.h"
#include "wayland-egl.h"
#include <GLES2/gl2.h>

#include "ilm_client.h"
#include <EGL/egl.h>

struct SurfaceConfiguration
{
    unsigned int layerId;
    unsigned int surfaceId;
    unsigned int surfaceWidth;
    unsigned int surfaceHeight;
};

class OpenGLES2App
{
public:
    OpenGLES2App(float fps, float animationSpeed, SurfaceConfiguration* config);
    virtual ~OpenGLES2App();

    void mainloop();

    static void serverinfoListener( void *data, struct serverinfo *pServerinfo, uint32_t client_handle );
    static void display_handle_global(struct wl_display* display, uint32_t id, const char* interface, uint32_t version, void* data);
    static int event_mask_update(uint32_t mask, void* data);
    static void frame_listener_func(void *data, struct wl_callback *callback, uint32_t time);

protected:
    virtual void update(int currentTimeInMs, int elapsedTimeInMs) = 0;
    virtual void render() = 0;
    void swapBuffers();

private:
    bool createWLContext(SurfaceConfiguration* config);
    void destroyWLContext();

    bool createEGLContext();
    void destroyEglContext();

    bool setupLayerMangement(SurfaceConfiguration* config);

    unsigned int GetTickCount();

protected:
    float m_framesPerSecond;
    float m_animationSpeed;
    unsigned int m_timerIntervalInMs;

    struct EglContextStruct
    {
	EGLNativeDisplayType nativeDisplay;
	EGLNativeWindowType  nativeWindow;
        EGLDisplay eglDisplay;
        EGLConfig eglConfig;
        EGLSurface eglSurface;
        EGLContext eglContext;
    };

    EglContextStruct m_eglContextStruct;

    typedef struct t_wlContextStruct
    {
        struct wl_display* wlDisplay;
        struct wl_compositor* wlCompositor;
	struct wl_egl_window* wlNativeWindow;
        struct wl_surface* wlSurface;
        int width;
        int height;

        uint32_t mask;

        struct serverinfo* wlExtServerinfo;
        uint32_t connect_id;
    } WLContextStruct;
    WLContextStruct m_wlContextStruct;
};

#endif
