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

#include "GraphicSystems/GLESGraphicSystem.h"

#include "string.h"
#include "EGL/egl.h"
#include "GLES2/gl2.h"

static const float vertices[8*12] = {
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        1.0, 1.0,
        0.0, 1.0,
        0.0, 0.0,

        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        1.0, 1.0,
        0.0, 1.0,
        0.0, 0.0,

        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        1.0, 1.0,
        0.0, 1.0,
        0.0, 0.0,

        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,
        0.0, 1.0,
        0.0, 0.0,
        1.0, 0.0,

        0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,

		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,

		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,

		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0

};

GLESGraphicsystem::GLESGraphicsystem(PfnShaderProgramCreator shaderProgram, ITextureBinder* binder ) : m_shaderCreatorFunc(shaderProgram){
	LOG_DEBUG("GLESGraphicsystem", "creating GLESGraphicsystem");
	m_binder = binder;
};

bool GLESGraphicsystem::init(void* displayPtr, void* WIndowIDPtr,int WindowHeight, int WindowWidth){
	EGLNativeDisplayType display = *(EGLNativeDisplayType*)displayPtr;
	LOG_DEBUG("GLESGraphicsystem", "Using EGLNativeDisplay:" << display);
	EGLNativeWindowType WIndowID = *(EGLNativeWindowType*)WIndowIDPtr;
	LOG_DEBUG("GLESGraphicsystem", "Using EGLNativeWindow:" << WIndowID);

	EGLint iMajorVersion, iMinorVersion;
	LOG_DEBUG("GLESGraphicsystem", "Getting EGL Display");
	m_eglDisplay = eglGetDisplay(display);

	LOG_DEBUG("GLESGraphicsystem", "Initialising EGL");
	if (!eglInitialize(m_eglDisplay, &iMajorVersion, &iMinorVersion))
		return false;

	LOG_DEBUG("GLESGraphicsystem", "Binding GLES API");
	eglBindAPI(EGL_OPENGL_ES_API);

	EGLint pi32ConfigAttribs[]={
			EGL_SURFACE_TYPE,EGL_WINDOW_BIT | EGL_PIXMAP_BIT,
			EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,
			EGL_ALPHA_SIZE,8,
			EGL_NONE
	};

	LOG_DEBUG("GLESGraphicsystem", "EGLChooseConfig");
	int iConfigs;
	if (!eglChooseConfig(m_eglDisplay, pi32ConfigAttribs, &m_eglConfig, 1, &iConfigs) || (iConfigs != 1))
	{
		LOG_DEBUG("GLESGraphicsystem", "Error: eglChooseConfig() failed.");
		return false;
	}
	EGLint id =0;
	eglGetConfigAttrib(m_eglDisplay, m_eglConfig, EGL_CONFIG_ID, &id);

	LOG_DEBUG("GLESGraphicsystem", "Config chosen:" << id);
	LOG_DEBUG("GLESGraphicsystem", "Create Window surface");

	m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, WIndowID, NULL);
	if (!m_eglSurface){
		LOG_DEBUG("GLESGraphicsystem", "create unsucessfull");
	}else{
		LOG_DEBUG("GLESGraphicsystem", "create successfull");
	}
	EGLint contextAttrs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	m_eglContext = eglCreateContext(m_eglDisplay,
                                        m_eglConfig,
                                        NULL,
                                        contextAttrs);
    if (!m_eglContext)
    {
        LOG_ERROR("GLESGraphicsystem","EGL couldn't create context\n");
        return false;
    }
    LOG_INFO("GLESGraphicsystem", "EGL make current ...");
    // Make the context and surface current for rendering
    EGLBoolean eglStatus = false;
    eglStatus = eglMakeCurrent(m_eglDisplay,
                               m_eglSurface, m_eglSurface,
                               m_eglContext);
    LOG_INFO("GLESGraphicsystem", "made current");


	if (!initOpenGLES(WindowWidth,WindowHeight) )
	{
				return false;
	}
	return true;
}

void GLESGraphicsystem::clearBackground()
{
	glClear(GL_COLOR_BUFFER_BIT);
}
void GLESGraphicsystem::swapBuffers()
{
	eglSwapBuffers(m_eglDisplay,m_eglSurface);
}

void GLESGraphicsystem::drawSurface(Layer* currentLayer,Surface* surface)
{
	ShaderProgram::CommonUniforms uniforms;
	Shader* layerShader = currentLayer->getShader();
	if (!layerShader)
	{
		// use default shader if no custom shader is assigned to this layer
		layerShader = defaultShader;
	}
	Shader* shader = (surface)->getShader();
	if (!shader)
	{
		// use layer shader if no custom shader is assigned to this surface
		shader = layerShader;
	}
	Rectangle dest = (surface)->getDestinationRegion();
	Rectangle src = (surface)->getSourceRegion();
	float surfaceXOfScreen = 	(float) dest.x / m_displayWidth;
	float surfaceYOfScreen =	(float) dest.y / m_displayHeight;
	float surfaceWidthOfScreen = (float) dest.width / m_displayWidth;
	float surfaceHeightOfScreen =  (float) dest.height / m_displayHeight;

	float sourceViewportWidthPercent = (float)src.width/(surface)->OriginalSourceWidth;
	float sourceViewportHeightpercent = (float)src.height/(surface)->OriginalSourceHeight;
	float sourceViewportXPercent = (float)src.x/(surface)->OriginalSourceWidth;
	float sourceViewportYPercent = (float)src.y/(surface)->OriginalSourceHeight;

	//	LOG_INFO("GLESGraphicsystem", "Binding Buffer");
	glBindBuffer(GL_ARRAY_BUFFER,m_vbo);

	//	LOG_INFO("GLESGraphicsystem", "Binding Buffer done");
	shader->use();

	//	LOG_INFO("GLESGraphicsystem", "Update Uniforms");
	// update all common uniforms
	uniforms.x = surfaceXOfScreen;
	uniforms.y = surfaceYOfScreen;
	uniforms.width = surfaceWidthOfScreen;
	uniforms.height = surfaceHeightOfScreen;
	uniforms.opacity = (surface)->getOpacity() * currentLayer->getOpacity();
	uniforms.texRange[0] = sourceViewportWidthPercent;
	uniforms.texRange[1] = sourceViewportHeightpercent;
	uniforms.texOffset[0] = sourceViewportXPercent;
	uniforms.texOffset[1] = -sourceViewportYPercent;
	uniforms.texUnit = 0;

	//	LOG_INFO("GLESGraphicsystem", "Load Common Uniforms");
	shader->loadCommonUniforms(uniforms);

	//	LOG_INFO("GLESGraphicsystem", "Load Custom Uniforms");
	// update all custom defined uniforms
	shader->loadUniforms();
	/* Bind texture and set section */
	glActiveTexture(GL_TEXTURE0);
	m_binder->bindSurfaceTexture(surface);

	// rotated positions are saved sequentially in vbo
	// offset in multiples of 12 decide rotation
	int orientation = (surface)->getOrientation() + (currentLayer)->getOrientation();
	orientation %= 4;
	int index = orientation * 12;
//	LOG_INFO("GLESGraphicsystem", "Drawing surface");
	// Draw two triangles
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// get the next surface
	glBindBuffer(GL_ARRAY_BUFFER,NULL);
//	LOG_INFO("GLESGraphicsystem", "Drawing surface done");
//	LOG_INFO("GLESGraphicsystem", "Drawing surface done now checking for error");
	GLenum status = glGetError();
}

bool GLESGraphicsystem::initOpenGLES(EGLint displayWidth,EGLint displayHeight)
{
	LOG_DEBUG("GLESGraphicsystem", "initEGL");
	bool result = true;
    ShaderProgramFactory::setCreatorFunc(m_shaderCreatorFunc);
    defaultShader = Shader::createShader("default", "default");
    if (!defaultShader)
    {
        LOG_ERROR("GLESGraphicsystem", "Failed to create and link default shader program");
        delete defaultShader;
        result = false;
    }
    else
	{
        LOG_INFO("GLESGraphicsystem", "Default Shader successfully applied");
    	glGenBuffers(1,&m_vbo);
    	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) *12));
        glEnableVertexAttribArray(1);

		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		m_blendingStatus = true;
		glClearColor(1.0, 0.0, 0.0, 1.0);
		resize(displayWidth,displayHeight);
	}
	return result;
}
void GLESGraphicsystem::resize(EGLint displayWidth,EGLint displayHeight)
{
	m_displayWidth = displayWidth;
	m_displayHeight = displayHeight;
    glViewport(0, 0, m_displayWidth, m_displayHeight);
}
