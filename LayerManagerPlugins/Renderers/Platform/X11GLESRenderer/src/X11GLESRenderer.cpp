/***************************************************************************
*
* Copyright 2010 BMW Car IT GmbH
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

#include "X11GLESRenderer.h"
#include "ShaderProgramGLES.h"
#include "X11/Xlib.h"
#include "TextureBinders/X11CopyGLES.h"
#include "TextureBinders/X11EglImage.h"

X11GLESRenderer::X11GLESRenderer(Scene* pScene)
: BaseRenderer(pScene)
, m_pWindowSystem(0)
, m_pGraphicSystem(0)
, m_width(0)
, m_height(0)
{
    LOG_DEBUG("X11GLESRenderer", "Creating Renderer");
}

bool X11GLESRenderer::start(int width, int height, const char* displayname)
{
    Display* nativeDisplayHandle = NULL;
    EGLDisplay eglDisplayhandle = NULL;
    ITextureBinder* binder = NULL;
    m_width = width;
    m_height = height;
    // create X11 windows, register as composite manager etc
    m_pWindowSystem = new X11WindowSystem(displayname, width, height, m_pScene);
    m_pGraphicSystem = new GLESGraphicsystem(width,height, ShaderProgramGLES::createProgram);

    if (!m_pWindowSystem->init(m_pGraphicSystem))
    {
        goto fail; // TODO bad style
    }

    m_pGraphicSystem->setBaseWindowSystem(m_pWindowSystem);

    // create graphic context from window, init egl etc
    nativeDisplayHandle = m_pWindowSystem->getNativeDisplayHandle();

    LOG_DEBUG("X11GLESRenderer", "Got nativedisplay handle: " << nativeDisplayHandle << " from windowsystem");

    eglDisplayhandle = m_pGraphicSystem->getEGLDisplay();

#ifdef GLES_FORCE_COPY
    binder = new X11CopyGLES(eglDisplayhandle, nativeDisplayHandle);
#else
#ifdef EGL_NATIVE_PIXMAP_KHR
    binder = new X11EglImage(eglDisplayhandle, nativeDisplayHandle);
#else
    binder = new X11CopyGLES(eglDisplayhandle, nativeDisplayHandle);
#endif
#endif
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

    LOG_ERROR("X11GLESRenderer", "Initialization failed !");
    return false;
}

void X11GLESRenderer::stop()
{
    m_pWindowSystem->stop();
}

void X11GLESRenderer::doScreenShot(std::string fileToSave)
{
    m_pWindowSystem->doScreenShot(fileToSave);
}

void X11GLESRenderer::doScreenShotOfLayer(std::string fileToSave, uint id)
{
    m_pWindowSystem->doScreenShotOfLayer(fileToSave,id);
}

void X11GLESRenderer::doScreenShotOfSurface(std::string fileToSave, uint id, uint layer_id)
{
    m_pWindowSystem->doScreenShotOfSurface(fileToSave,id,layer_id);
}

uint X11GLESRenderer::getNumberOfHardwareLayers(uint screenID)
{
    (void)screenID; // TODO: remove, only prevents warning

    return 0; // TODO provide real value here
}

uint* X11GLESRenderer::getScreenResolution(uint screenID)
{
    (void)screenID; // TODO: remove, only prevents warning

    // TODO provide value of real screen here
    uint * resolution = new uint[2];
    resolution[0] = m_width;
    resolution[1] = m_height;
    return resolution;
}

uint* X11GLESRenderer::getScreenIDs(uint* length)
{
    Display* x11Display = m_pWindowSystem->getNativeDisplayHandle();
    if (!x11Display)
    {
        return NULL;
    }
    // Screens in X11 can be addresses/accessed by just the number - we must only know how many there are
    uint numberOfScreens = ScreenCount(x11Display);
    uint* screenIDS = new uint[numberOfScreens];
    for (uint i = 0; i < numberOfScreens; ++i)
    {
        screenIDS[i] = i;
    }
    *length = numberOfScreens;
    return screenIDS;
}

void X11GLESRenderer::signalWindowSystemRedraw()
{
	m_pWindowSystem->signalRedrawEvent();
}

extern "C" BaseRenderer* createX11GLESRenderer(Scene* pScene){
    return new X11GLESRenderer(pScene);
}

extern "C" void destroyX11GLESRenderer(X11GLESRenderer* p)
{
    delete p;
}
