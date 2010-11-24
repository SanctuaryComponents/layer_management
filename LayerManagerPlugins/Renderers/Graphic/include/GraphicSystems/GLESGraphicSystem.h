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

#ifndef _GLESGRAPHICSYSTEM_H_
#define _GLESGRAPHICSYSTEM_H_

#include "GraphicSystems/BaseGraphicSystem.h"
#include "ShaderProgramFactory.h"
#include "EGL/egl.h"
#include "Log.h"
#include "Shader.h"
/* uncomment if layer drawing needed */
/* #define DRAW_LAYER_DEBUG */
class GLESGraphicsystem : public BaseGraphicSystem {
public:
	GLESGraphicsystem(PfnShaderProgramCreator shaderProgram, ITextureBinder* binder);

	bool init(void* display, void* WindowID,int WindowHeight, int WindowWidth);
	void clearBackground();
	void swapBuffers();
	void drawSurface(Layer* currentLayer, Surface* surface);
	bool initOpenGLES(EGLint displayWidth, EGLint displayHeight);
	void resize(EGLint displayWidth, EGLint displayHeight);
	void doScreenShot(std::string fileToSave);

	PfnShaderProgramCreator 			m_shaderCreatorFunc;
	EGLConfig 							m_eglConfig;
	EGLContext							m_eglContext;
	EGLSurface 							m_eglSurface;
	EGLDisplay							m_eglDisplay;
	uint								m_vbo;
	EGLint								m_displayWidth;
	EGLint								m_displayHeight;
	EGLBoolean							m_blendingStatus;
	Shader* 							m_defaultShader;
#ifdef DRAW_LAYER_DEBUG
	Shader* 							m_layerShader;
#endif
private:
	void saveScreenShot();
	std::string screenShotFile;
	bool takescreenshot;
};

#endif /* _GLESGRAPHICSYSTEM_H_ */
