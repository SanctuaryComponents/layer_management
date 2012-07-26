/***************************************************************************
 *
 * Copyright 2010,2011 BMW Car IT GmbH
 * Copyright (C) 2012 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
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
#include "ViewportTransform.h"
#include "config.h"
#include <string>

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
, m_defaultShaderAddUniformChromaKey(0)
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
    LOG_INFO("GLESGraphicsystem", "Initialisation");
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
    LOG_DEBUG("GLESGraphicsystem", "EGL make current ...");
    // Make the context and surface current for rendering
    eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext);
    LOG_DEBUG("GLESGraphicsystem", "made current");
    eglSwapInterval(m_eglDisplay, 1);

    if (!initOpenGLES(m_windowWidth, m_windowHeight))
    {
        LOG_ERROR("GLESGraphicsystem", "Initialisation failed");
        return false;
    }
    LOG_INFO("GLESGraphicsystem", "Initialisation successfull");
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

// Reports whether a single layer is damaged/dirty
// Can not account for possible occlusion by other layers
bool GLESGraphicsystem::needsRedraw(Layer *layer)
{
    if (layer->renderPropertyChanged)
    {
        return true;
    }

    if (layer->visibility && layer->opacity > 0.0)
    {
        SurfaceList surfaces = layer->getAllSurfaces();
        for(SurfaceListConstIterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++)
        {
            if ((*currentS)->renderPropertyChanged)
            {
                return true;
            }

            if ((*currentS)->hasNativeContent() && (*currentS)->damaged && (*currentS)->visibility && (*currentS)->opacity>0.0f)
            {
                return true;
            }
        }
    }

    return false;
}

// Reports whether the passed in layers have visible damage or are otherwise
// dirty because render properties changed.
// Assumes that layers in the list belong to same composition. ie. damage to
// one layer affects the others.  A warning is logged if the assumption is wrong.
bool GLESGraphicsystem::needsRedraw(LayerList layers)
{
    // TODO: Ignore damage from completely obscured surfaces

    for (LayerListConstIterator layer = layers.begin(); layer != layers.end(); layer++)
    {
        if ((*layer)->getLayerType() == Hardware && layers.size() > 1)
        {
            // Damage in a hardware layer should not imply a redraw in other layers
            LOG_WARNING("GLESGraphicsystem", "needsRedraw() called with layers not in the same composition");
        }

        if (needsRedraw(*layer))
        {
            return true;
        }
    }

    return false;
}

void GLESGraphicsystem::renderSWLayer(Layer *layer, bool clear)
{
    beginLayer(layer);

    if ((layer->getLayerType() != Software_2D) &&
        (layer->getLayerType() != Software_2_5D))
    {
        LOG_WARNING("GLESGraphicsystem", "renderSWLayer() called with a non-SW layer");
    }

    if (clear)
    {
        clearBackground();
    }

    if ( layer->visibility && layer->opacity > 0.0 )
    {
        SurfaceList surfaces = layer->getAllSurfaces();
        for(SurfaceListConstIterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++)
        {
            if ((*currentS)->hasNativeContent() && (*currentS)->visibility && (*currentS)->opacity>0.0f)
            {
                renderSurface(*currentS);
            }
        }
    }

    endLayer();
}

void GLESGraphicsystem::renderSWLayers(LayerList layers, bool clear)
{
    // This is a stub.
    //
    // TODO: render in a more optimal way
    //   1. Turn off blending for first surface rendered
    //   2. Don't clear when it's legal to avoid it
    //         eg. a fullscreen opaque surface exists
    //   3. Render multiple surfaces at time via multi-texturing
    //   4. Remove fully obscured layers/surfaces
    if (clear)
    {
        clearBackground();
    }

    for (LayerListConstIterator layer = layers.begin(); layer != layers.end(); layer++)
    {
        renderSWLayer(*layer, false); // Don't clear
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

    LOG_DEBUG("GLESGraphicsystem", "shader:currentShader");
    do
    {
        if (currentShader != m_defaultShader)
        {
        LOG_DEBUG("GLESGraphicsystem", "shader:default");
            break;
        }

        if (false == curUniforms.chromaKeyEnabled)
        {
            if (curUniforms.opacity == 1.0f)
            {
                //no need for multiply in shader, just use texture
                retShader = m_defaultShaderNoUniformAlpha;
        LOG_DEBUG("GLESGraphicsystem", "shader:defaultShaderNoUniformAlpha");
            }
        }
        else
        {
            // Add chromakey to default fragment shader
            retShader = m_defaultShaderAddUniformChromaKey;
        LOG_DEBUG("GLESGraphicsystem", "shader:defaultShaderAddUniformChromaKey");
        }
    } while(0);

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
    if (surface->isCropped())
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


    FloatRectangle targetSurfaceSource = surface->getTargetSourceRegion();
    FloatRectangle targetSurfaceDestination = surface->getTargetDestinationRegion();

    float textureCoordinates[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSource, surface->OriginalSourceWidth, surface->OriginalSourceHeight, textureCoordinates);

    applyLayerMatrix(layerMatrix);
    // update all common uniforms, scale values to display size
    // offsets are generated w.r.t lower left corner (following GL conventions)
    uniforms.x = targetSurfaceDestination.x / m_displayWidth;
    uniforms.y = 1.0f - (targetSurfaceDestination.y + targetSurfaceDestination.height) / m_displayHeight;;
    uniforms.width = targetSurfaceDestination.width / m_displayWidth;
    uniforms.height = targetSurfaceDestination.height / m_displayHeight;
    uniforms.opacity = (surface)->getOpacity() * m_currentLayer->getOpacity();
    uniforms.texRange[0] = (textureCoordinates[2]-textureCoordinates[0]);
    uniforms.texRange[1] = (textureCoordinates[3]-textureCoordinates[1]);
    uniforms.texOffset[0] = textureCoordinates[0];
    uniforms.texOffset[1] = textureCoordinates[1];
    uniforms.texUnit = 0;
    uniforms.matrix = &layerMatrix.f[0];
    uniforms.chromaKeyEnabled = (surface)->getChromaKeyEnabled();
    if (true == uniforms.chromaKeyEnabled)
    {
        unsigned char red = 0;
        unsigned char green = 0;
        unsigned char blue = 0;
        (surface)->getChromaKey(red, green, blue);
        uniforms.chromaKey[0] = (float)red / 255.0f;
        uniforms.chromaKey[1] = (float)green / 255.0f;
        uniforms.chromaKey[2] = (float)blue / 255.0f;
    }

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
    m_defaultShaderAddUniformChromaKey= Shader::createShader("default", "default_add_uniform_chromakey");

#ifdef DRAW_LAYER_DEBUG
    std::string pluginLookupPath = getenv("LM_PLUGIN_PATH");
    std::string vertexShaderPath;
    std::string fragmentShaderPath;

    if (pluginLookupPath.empty())
    {
        pluginLookupPath = CMAKE_INSTALL_PREFIX"/lib/layermanager";
    }

    vertexShaderPath   = pluginLookupPath + "/renderer/renderer_layer.glslv";
    fragmentShaderPath = pluginLookupPath + "/renderer/renderer_layer.glslf";

    m_layerShader = Shader::createShader(vertexShaderPath, fragmentShaderPath);
#endif
    if (
      !m_defaultShader || !m_defaultShaderNoUniformAlpha || !m_defaultShaderAddUniformChromaKey
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
        glActiveTexture(GL_TEXTURE0);
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

