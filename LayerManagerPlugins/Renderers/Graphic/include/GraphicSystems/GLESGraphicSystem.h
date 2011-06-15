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

#ifndef _GLESGRAPHICSYSTEM_H_
#define _GLESGRAPHICSYSTEM_H_

#include "GraphicSystems/BaseGraphicSystem.h"
#include "ShaderProgramFactory.h"
#include "EGL/egl.h"
#include "Log.h"
#include "Shader.h"
/* uncomment if layer drawing needed */
/* #define DRAW_LAYER_DEBUG */
class GLESGraphicsystem: public BaseGraphicSystem<EGLNativeDisplayType, EGLNativeWindowType>
{
public:

    GLESGraphicsystem(int windowHeight, int windowWidth,
            PfnShaderProgramCreator shaderProgram);

    bool init(EGLNativeDisplayType display, EGLNativeWindowType window);

    void clearBackground();

    void swapBuffers();

	void beginLayer(Layer* layer);
	void checkRenderLayer();
	void renderLayer();
	void endLayer();

    bool initOpenGLES(EGLint displayWidth, EGLint displayHeight);
    void resize(EGLint displayWidth, EGLint displayHeight);
    void doScreenShot(std::string fileToSave);

    void saveScreenShotOfFramebuffer(std::string fileToSave);

    EGLDisplay getEGLDisplay()
    {
        return m_eglDisplay;
    }

    // TODO: should be private/protected
    int m_windowWidth;
    int m_windowHeight;
    EGLNativeDisplayType m_nativeDisplay;
    EGLNativeWindowType m_nativeWindow;
    PfnShaderProgramCreator m_shaderCreatorFunc;
    EGLConfig m_eglConfig;
    EGLContext m_eglContext;
    EGLSurface m_eglSurface;
    EGLDisplay m_eglDisplay;
    uint m_vbo;
    EGLint m_displayWidth;
    EGLint m_displayHeight;
    EGLBoolean m_blendingStatus;
    Shader* m_defaultShader;
    Shader* m_defaultShaderNoUniformAlpha;
    Layer* m_currentLayer;
    void renderSurface(Surface* surface);
	Shader *pickOptimizedShader(Shader* currentShader, const ShaderProgram::CommonUniforms curUniforms);
#ifdef DRAW_LAYER_DEBUG
    Shader* m_layerShader;
#endif

protected:

private:
    void saveScreenShot();
};

#endif /* _GLESGRAPHICSYSTEM_H_ */
