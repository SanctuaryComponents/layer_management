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

#include "WaylandGLESRenderer.h"
#include "config.h"
#include "Shader.h"
#include "ShaderProgramGLES.h"
#include "TextureBinders/WaylandCopyGLES.h"
#include "TextureBinders/WaylandEglImage.h"
#include "WindowSystems/WaylandFbdevWindowSystem.h"
#include "WindowSystems/WaylandX11WindowSystem.h"

WaylandGLESRenderer::WaylandGLESRenderer(Scene* pScene)
: BaseRenderer(pScene)
, m_pWindowSystem(0)
, m_pGraphicSystem(0)
, m_width(0)
, m_height(0)
{
    LOG_DEBUG("WaylandGLESRenderer", "Creating Renderer");
}

bool WaylandGLESRenderer::start(int width, int height, const char* displayname)
{
    struct wl_display* nativeDisplayHandle = NULL;
    EGLDisplay eglDisplayhandle = NULL;
    ITextureBinder* binder = NULL;
    m_width = width;
    m_height = height;
    // create Wayland windows, register as composite manager etc
    m_pWindowSystem = NULL;
#ifdef WITH_WAYLAND_FBDEV
    m_pWindowSystem = new WaylandFbdevWindowSystem(displayname, width, height, m_pScene);
#endif
#ifdef WITH_WAYLAND_X11
    m_pWindowSystem = new WaylandX11WindowSystem(displayname, width, height, m_pScene);
#endif
    if( m_pWindowSystem == NULL )
    {
    LOG_ERROR("WaylandGLESRenderer", "Window system is not specified. Consider to specify WITH_WAYLAND_X11 or WITH_WAYLAND_FBDEV");
    goto fail; // TODO bad style
    }

    m_pGraphicSystem = new GLESGraphicsystem(width,height, ShaderProgramGLES::createProgram);

    if (!m_pWindowSystem->init((BaseGraphicSystem<EGLNativeDisplayType, EGLNativeWindowType>*) m_pGraphicSystem))
    {
        goto fail; // TODO bad style
    }

    m_pGraphicSystem->setBaseWindowSystem(m_pWindowSystem);

    // create graphic context from window, init egl etc
    nativeDisplayHandle = m_pWindowSystem->getNativeDisplayHandle();

    LOG_DEBUG("WaylandGLESRenderer", "Got nativedisplay handle: " << nativeDisplayHandle << " from windowsystem");

    eglDisplayhandle = m_pGraphicSystem->getEGLDisplay();

#ifdef WITH_FORCE_COPY
    binder = new WaylandCopyGLES(eglDisplayhandle, nativeDisplayHandle);
#else // WITH_FORCE_COPY
#ifdef EGL_NATIVE_PIXMAP_KHR
    binder = new WaylandEglImage(eglDisplayhandle, nativeDisplayHandle);
#else // EGL_NATIVE_PIXMAP_KHR
    binder = new WaylandCopyGLES(eglDisplayhandle, nativeDisplayHandle);
#endif // EGL_NATIVE_PIXMAP_KHR
#endif // WITH_FORCE_COPY
    if ( binder && nativeDisplayHandle && eglDisplayhandle)
    {
        m_pGraphicSystem->setTextureBinder(binder);

        if (!m_pWindowSystem->start())
        {
            goto fail; // TODO bad style
        }
    }
    else
    {
        goto fail; // TODO bad style
    }
    return true;

    fail: // TODO bad style

    LOG_ERROR("WaylandGLESRenderer", "Initialization failed !");
    return false;
}

void WaylandGLESRenderer::stop()
{
    m_pWindowSystem->stop();
}

void WaylandGLESRenderer::doScreenShot(std::string fileToSave)
{
    m_pWindowSystem->doScreenShot(fileToSave);
}

void WaylandGLESRenderer::doScreenShotOfLayer(std::string fileToSave, uint id)
{
    m_pWindowSystem->doScreenShotOfLayer(fileToSave,id);
}

void WaylandGLESRenderer::doScreenShotOfSurface(std::string fileToSave, uint id, uint layer_id)
{
    m_pWindowSystem->doScreenShotOfSurface(fileToSave,id,layer_id);
}

uint WaylandGLESRenderer::getNumberOfHardwareLayers(uint screenID)
{
    uint screen_id;
    screen_id = screenID;

    return 0; // TODO provide real value here
}

uint* WaylandGLESRenderer::getScreenResolution(uint screenID)
{
    uint screen_id;
    screen_id = screenID;   
    // TODO provide value of real screen here
    uint * resolution = new uint[2];
    resolution[0] = m_width;
    resolution[1] = m_height;
    return resolution;
}

uint* WaylandGLESRenderer::getScreenIDs(uint* length)
{
    // TODO necessary to implement
    uint* screenIDS = new uint[1];
	screenIDS[0] = 0;
    *length = 1;
    return screenIDS;
}

void WaylandGLESRenderer::signalWindowSystemRedraw()
{
    m_pWindowSystem->signalRedrawEvent();
}

void WaylandGLESRenderer::forceCompositionWindowSystem()
{
    m_pWindowSystem->m_damaged = true;
}

Shader* WaylandGLESRenderer::createShader(const string* vertexName, const string* fragmentName)  
{
    Shader *result = NULL;
    m_pWindowSystem->setSystemState(WAKEUP_STATE);
    m_pWindowSystem->wakeUpRendererThread();    
    m_pGraphicSystem->activateGraphicContext();
    result = Shader::createShader(*vertexName,*fragmentName);
    m_pGraphicSystem->releaseGraphicContext();
    m_pWindowSystem->setSystemState(IDLE_STATE);   
    return result;
}

extern "C" BaseRenderer* createWaylandGLESRenderer(Scene* pScene){
    return new WaylandGLESRenderer(pScene);
}

extern "C" void destroyWaylandGLESRenderer(WaylandGLESRenderer* p)
{
    delete p;
}
