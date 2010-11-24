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

#include "X11GLESRenderer.h"
#include "ShaderProgramGLES.h"
#include "WindowSystems/X11WindowSystem.h"
#include "X11/Xlib.h"
#include "TextureBinders/X11CopyGLES.h"
#include "TextureBinders/X11EglImage.h"

X11GLESRenderer::X11GLESRenderer(LayerList* layerlist) : BaseRenderer(layerlist), graphicSystem(NULL)
{
	LOG_DEBUG("X11GLESRenderer", "Creating Renderer");

	// if no binder has been set yet
	Display* display = XOpenDisplay(0);
	EGLDisplay m_eglDisplay = eglGetDisplay(display);
	ITextureBinder* binder=NULL;
	const char* query = eglQueryString(m_eglDisplay, EGL_EXTENSIONS);
	LOG_DEBUG("X11GLESRenderer", "EGL_EXTENSIONS: " << query);
	#ifdef EGL_NATIVE_PIXMAP_KHR
			binder = new X11EglImage((Display*)display);
	#else
			binder = new X11CopyGLES((Display*)display);
	#endif

	graphicSystem = new GLESGraphicsystem( ShaderProgramGLES::createProgram, binder);
	m_windowSystem = new X11WindowSystem(layerlist, graphicSystem);
};

void X11GLESRenderer::doScreenShot(std::string fileToSave){
	graphicSystem->doScreenShot(fileToSave);
}

extern "C" BaseRenderer* createX11GLESRenderer(LayerList* layerlist){
    return new X11GLESRenderer(layerlist);
}

extern "C" void destroyX11GLESRenderer(X11GLESRenderer* p){
    delete p;
}
