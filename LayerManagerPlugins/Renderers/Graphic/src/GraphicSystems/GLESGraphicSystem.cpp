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

#include "GraphicSystems/GLESGraphicSystem.h"
#include "IlmMatrix.h"
#include "string.h"
#include "EGL/egl.h"
#include "GLES2/gl2.h"
#include "Bitmap.h"
#include "Transformation/ViewportTransform.h"

static const float vertices[8 * 12] =
{ 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0,

0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0,

0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0,

1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0,

0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0,

1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0,

0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0,

0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0

};

GLESGraphicsystem::GLESGraphicsystem(int windowWidth, int windowHeight, PfnShaderProgramCreator shaderProgram)
: m_windowWidth(windowWidth)
, m_windowHeight(windowHeight)
, m_nativeDisplay(0)
, m_nativeWindow(0)
, m_shaderCreatorFunc(shaderProgram)
, m_eglConfig(0)
, m_eglContext(0)
, m_eglSurface(0)
, m_eglDisplay(0)
, m_vbo(0)
, m_displayWidth(0)
, m_displayHeight(0)
, m_blendingStatus(false)
, m_defaultShader(0)
, m_defaultShaderNoUniformAlpha(0)
, m_currentLayer(0)
#ifdef DRAW_LAYER_DEBUG
, m_layerShader(0)
#endif
{
    LOG_DEBUG("GLESGraphicsystem", "creating GLESGraphicsystem");
}

void GLESGraphicsystem::activateGraphicContext()
{
    eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext);
}

void GLESGraphicsystem::releaseGraphicContext() 
{
    eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

bool GLESGraphicsystem::init(EGLNativeDisplayType display, EGLNativeWindowType NativeWindow)
{
    m_nativeDisplay = display;
    m_nativeWindow = NativeWindow;

    EGLint iMajorVersion, iMinorVersion;
    LOG_DEBUG("GLESGraphicsystem", "Getting EGL Display with native display " << m_nativeDisplay);
    m_eglDisplay = eglGetDisplay(m_nativeDisplay);

    if (m_eglDisplay == EGL_NO_DISPLAY){
        LOG_ERROR("GLESGraphicsystem", "failed to Get EGL Display");
        return false;
    }

    LOG_DEBUG("GLESGraphicsystem", "Initialising EGL");
    if (!eglInitialize(m_eglDisplay, &iMajorVersion, &iMinorVersion))
    {
        LOG_ERROR("GLESGraphicsystem", "Initialising EGL failed");
        return false;
    }

    LOG_DEBUG("GLESGraphicsystem", "Binding GLES API");
    eglBindAPI(EGL_OPENGL_ES_API);

    EGLint pi32ConfigAttribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RENDERABLE_TYPE,
            EGL_OPENGL_ES2_BIT,
            EGL_RED_SIZE,
            8,
            EGL_ALPHA_SIZE,
            8,
            EGL_NONE
    };

    LOG_DEBUG("GLESGraphicsystem", "EGLChooseConfig");
    int iConfigs;
    if (!eglChooseConfig(m_eglDisplay, pi32ConfigAttribs, &m_eglConfig, 1, &iConfigs) || (iConfigs != 1))
    {
        LOG_DEBUG("GLESGraphicsystem", "Error: eglChooseConfig() failed.");
        return false;
    }
    EGLint id = 0;
    eglGetConfigAttrib(m_eglDisplay, m_eglConfig, EGL_CONFIG_ID, &id);

    EGLint windowsAttr[] = { EGL_RENDER_BUFFER, EGL_BACK_BUFFER, EGL_NONE };

    LOG_DEBUG("GLESGraphicsystem", "Config chosen:" << id);
    LOG_DEBUG("GLESGraphicsystem", "Create Window surface");

    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_nativeWindow, windowsAttr);
    if (!m_eglSurface)
    {
        EGLenum status = eglGetError();
        LOG_ERROR("GLESGraphicsystem", "Window Surface creation failed with EGL Error Code: "<< status);
        return false;
    }
    LOG_DEBUG("GLESGraphicsystem", "Window Surface creation successfull");

    EGLint contextAttrs[] = {
            EGL_CONTEXT_CLIENT_VERSION,
            2,
            EGL_NONE
    };

    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, NULL, contextAttrs);
    if (!m_eglContext)
    {
        LOG_ERROR("GLESGraphicsystem","EGL couldn't create context\n");
        return false;
    }
    LOG_INFO("GLESGraphicsystem", "EGL make current ...");
    // Make the context and surface current for rendering
    eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext);
    LOG_INFO("GLESGraphicsystem", "made current");

    eglSwapInterval(m_eglDisplay, 1);

    if (!initOpenGLES(m_windowWidth, m_windowHeight))
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
    eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

void GLESGraphicsystem::beginLayer(Layer* currentLayer)
{
    //LOG_DEBUG("GLESGraphicsystem", "Beginning to draw layer: " << currentLayer->getID());
    m_currentLayer = currentLayer;
    // TODO layer destination / source
}

void GLESGraphicsystem::checkRenderLayer()
{
    SurfaceList surfaces = m_currentLayer->getAllSurfaces();

    m_currentLayer->damaged = false;

    if (!(m_baseWindowSystem->m_damaged && m_currentLayer->getLayerType() != Hardware))
    {
        if (m_currentLayer->renderPropertyChanged)
        {
            m_currentLayer->damaged = true;
        }
        else if ((m_currentLayer)->visibility && (m_currentLayer)->opacity > 0.0)
        {
            for(std::list<Surface*>::const_iterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++)
            {
                if ((*currentS)->renderPropertyChanged)
                {
                    m_currentLayer->damaged = true;
                    break;
                }
                else if ((*currentS)->hasNativeContent() && (*currentS)->damaged && (*currentS)->visibility && (*currentS)->opacity>0.0f)
                {
                    m_currentLayer->damaged = true;
                    break;
                }
            }
        }

        // Preseve m_currentLayer->damaged for HW layers so that they can be updated independently
        if (m_currentLayer->damaged && m_currentLayer->getLayerType() != Hardware)
        {
            m_baseWindowSystem->m_damaged = true;
            m_currentLayer->damaged = false;
        }
    }

    for(std::list<Surface*>::const_iterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++)
    {
        (*currentS)->damaged = false;
        (*currentS)->renderPropertyChanged = false;
    }

    m_currentLayer->renderPropertyChanged = false;
}

void GLESGraphicsystem::renderSWLayer()
{
    if ( (m_currentLayer)->visibility && (m_currentLayer)->opacity > 0.0 )
    {
        SurfaceList surfaces = m_currentLayer->getAllSurfaces();
        for(std::list<Surface*>::const_iterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++)
        {
            if ((*currentS)->hasNativeContent() && (*currentS)->visibility && (*currentS)->opacity>0.0f)
            {
                Surface* currentSurface = (Surface*)*currentS;
                renderSurface(currentSurface);
            }
        }
    }
}

void GLESGraphicsystem::endLayer()
{
    //LOG_DEBUG("GLESGraphicsystem", "Done with rendering layer: " << m_currentLayer->getID());
    m_currentLayer = NULL;
}

//this is a particularly simple function currently, but it will likely be expanded as more shaders and effects are implemented.
Shader *GLESGraphicsystem::pickOptimizedShader(Shader* currentShader, const ShaderProgram::CommonUniforms& curUniforms)
{
    Shader * retShader = currentShader;

    if (currentShader == m_defaultShader && curUniforms.opacity == 1.0f)
    {
      //no need for multiply in shader, just use texture
      retShader = m_defaultShaderNoUniformAlpha;
    }

    return retShader;

}

void GLESGraphicsystem::applyLayerMatrix(IlmMatrix& matrix)
{
    IlmMatrixRotateZ(matrix, m_currentLayer->getOrientation() * 90.0f);
}

void GLESGraphicsystem::renderSurface(Surface* surface)
{
//  LOG_DEBUG("GLESGraphicsystem", "renderSurface " << surface->getID());
    GLenum glErrorCode = GL_NO_ERROR;
    // check if surface is cropped completely, if so then skip rendering
    if (ViewportTransform::isFullyCropped(surface->getDestinationRegion(), m_currentLayer->getSourceRegion() ) )
        return; // skip rendering of this surface, because it is cropped by layer source region

    GLint index = 0;
    IlmMatrix layerMatrix;
    IlmMatrixIdentity(layerMatrix);

    ShaderProgram::CommonUniforms uniforms;
#ifdef DRAW_LAYER_DEBUG
    ShaderProgram::CommonUniforms layeruniforms;
#endif
    Shader* layerShader = m_currentLayer->getShader();
    if (!layerShader)
    {
        // use default shader if no custom shader is assigned to this layer
        layerShader = m_defaultShader;
    }
    Shader* shader = (surface)->getShader();
    if (!shader)
    {
        // use layer shader if no custom shader is assigned to this surface
        shader = layerShader;
    }

    const FloatRectangle layerSourceRegion = m_currentLayer->getSourceRegion();
    const FloatRectangle layerDestinationRegion = m_currentLayer->getDestinationRegion();

    FloatRectangle targetSurfaceSource = surface->getSourceRegion();
    FloatRectangle targetSurfaceDestination = surface->getDestinationRegion();

    ViewportTransform::applyLayerSource(layerSourceRegion, targetSurfaceSource, targetSurfaceDestination);
    ViewportTransform::applyLayerDestination(layerDestinationRegion, layerSourceRegion, targetSurfaceDestination);
    float textureCoordinates[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSource, surface->OriginalSourceWidth, surface->OriginalSourceHeight, textureCoordinates);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    applyLayerMatrix(layerMatrix);
    // update all common uniforms, scale values to display size
    uniforms.x = targetSurfaceDestination.x / m_displayWidth;
    uniforms.y = targetSurfaceDestination.y / m_displayHeight;;
    uniforms.width = targetSurfaceDestination.width / m_displayWidth;
    uniforms.height = targetSurfaceDestination.height / m_displayHeight;
    uniforms.opacity = (surface)->getOpacity() * m_currentLayer->getOpacity();
    uniforms.texRange[0] = (textureCoordinates[1]-textureCoordinates[0]);
    uniforms.texRange[1] = (textureCoordinates[3]-textureCoordinates[2]);
    uniforms.texOffset[0] = textureCoordinates[0];
    uniforms.texOffset[1] = -textureCoordinates[2];
    uniforms.texUnit = 0;
    uniforms.matrix = &layerMatrix.f[0];


    //We only know about specific Shaders, only do this if we start with the defaultShader
    if (shader == m_defaultShader && uniforms.opacity == 1.0f)
    {
        if(!PixelFormatHasAlpha((surface)->getPixelFormat()))
        {
            //disable alpha blend completely
            glDisable (GL_BLEND);
        }
        else
        {
            //make sure alpha blend is enabled
            glEnable (GL_BLEND);
        }
    }
    else
    {
        //make sure alpha blend is enabled
        glEnable (GL_BLEND);
    }

    shader = pickOptimizedShader(shader, uniforms);


#ifdef DRAW_LAYER_DEBUG
    layeruniforms.x = (float) layerDestinationRegion.x / m_displayWidth;
    layeruniforms.y = (float) layerDestinationRegion.y / m_displayHeight;
    layeruniforms.width = (float)layerDestinationRegion.width / m_displayWidth;
    layeruniforms.height = (float)layerDestinationRegion.height / m_displayHeight;
    layeruniforms.opacity = m_currentLayer->getOpacity();
    layeruniforms.matrix = &layerMatrix.f[0];
    m_layerShader->use();
    m_layerShader->loadCommonUniforms(layeruniforms);
    m_layerShader->loadUniforms();

    glDrawArrays(GL_TRIANGLES, 0, 6);
#endif
    shader->use();

    /* load common uniforms */
    shader->loadCommonUniforms(uniforms);

    /* update all custom defined uniforms */
    shader->loadUniforms();
    /* Bind texture and set section */
    glActiveTexture(GL_TEXTURE0);
    if (false == m_binder->bindSurfaceTexture(surface))
    {
        LOG_WARNING("GLESGraphicsystem", "Surface not successfully bind " << surface->getID());
        return;
    }

    /* rotated positions are saved sequentially in vbo
     offset in multiples of 12 decide rotation */
    /* Draw two triangles */
    int orientation = (surface)->getOrientation();
    orientation %= 4;
    index = orientation * 12;
    surface->frameCounter++;
    surface->drawCounter++;
    glDrawArrays(GL_TRIANGLES, index, 6);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_binder->unbindSurfaceTexture(surface);
    glErrorCode = glGetError();
    if ( GL_NO_ERROR != glErrorCode )
    {
        LOG_ERROR("GLESGraphicsystem", "GL Error occured :" << glErrorCode );
    };
}

bool GLESGraphicsystem::initOpenGLES(EGLint displayWidth, EGLint displayHeight)
{
    LOG_DEBUG("GLESGraphicsystem", "initEGL");
    bool result = true;
    ShaderProgramFactory::setCreatorFunc(m_shaderCreatorFunc);
    m_defaultShader = Shader::createShader("default", "default");
    m_defaultShaderNoUniformAlpha = Shader::createShader("default", "default_no_uniform_alpha");

#ifdef DRAW_LAYER_DEBUG
    m_layerShader = Shader::createShader("/usr/lib/layermanager/renderer/renderer_layer.glslv", "/usr/lib/layermanager/renderer/renderer_layer.glslf");
    if (m_layerShader==0)
    {
        m_layerShader = Shader::createShader("/usr/local/lib/layermanager/renderer/renderer_layer.glslv", "/usr/local/lib/layermanager/renderer/renderer_layer.glslf");
    }
#endif
    if (
      !m_defaultShader || !m_defaultShaderNoUniformAlpha
#ifdef DRAW_LAYER_DEBUG
    || !m_layerShader
#endif
    )
    {
        LOG_ERROR("GLESGraphicsystem", "Failed to create and link default shader program");
        delete m_defaultShader;
        result = false;
    }
    else
    {
        LOG_INFO("GLESGraphicsystem", "Default Shader successfully applied");
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(float) * 12));
        glEnableVertexAttribArray(1);

        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_blendingStatus = true;
        glClearColor(0.0, 0.0, 0.0, 0.0);
        resize(displayWidth, displayHeight);
    }
    return result;
}

void GLESGraphicsystem::resize(EGLint displayWidth, EGLint displayHeight)
{
    m_displayWidth = displayWidth;
    m_displayHeight = displayHeight;
    glViewport(0, 0, m_displayWidth, m_displayHeight);
}

void GLESGraphicsystem::saveScreenShotOfFramebuffer(std::string fileToSave)
{
    // clear error if any
    int error = glGetError();
    LOG_DEBUG("GLESGraphicSystem","taking screenshot and saving it to:" << fileToSave);

    LOG_DEBUG("GLESGraphicSystem","Screenshot: " << m_displayWidth << " * " << m_displayHeight);
    char *buffer = (char *) malloc( m_displayWidth * m_displayHeight * 4 * sizeof(char));
    glReadPixels(0, 0, m_displayWidth, m_displayHeight, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    error = glGetError();
    if (error != GL_NO_ERROR)
    {
        LOG_DEBUG("GLESGraphicSystem","error reading pixels for screenshot: " << error);
    }
    // convert to RGB for bitmap
    int pixelcount = m_displayHeight * m_displayWidth;
    char *rgbbuffer = (char *) malloc(pixelcount * 3 * sizeof(char));
    for (int row = 0; row < m_displayHeight; row++)
    {
        for (int col = 0; col < m_displayWidth; col++)
        {
            int offset = row * m_displayWidth + col;
            rgbbuffer[offset * 3] = buffer[offset * 4 + 2];
            rgbbuffer[offset * 3 + 1] = buffer[offset * 4 + 1];
            rgbbuffer[offset * 3 + 2] = buffer[offset * 4];
        }
    }

    writeBitmap(fileToSave, rgbbuffer, m_displayWidth, m_displayHeight);
    free(buffer);
    free(rgbbuffer);
}

