/***************************************************************************
 *
 * Copyright 2010,2011 BMW Car IT GmbH
<<<<<<< HEAD
 * Copyright (C) 2012 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
=======
 * Copyright (c) 2012, NVIDIA CORPORATION.  All rights reserved.
>>>>>>> Renderers: new GLES shader selection mechanism
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
, m_eglPbufferSurface(0)
, m_eglDisplay(0)
, m_vbo(0)
, m_displayWidth(0)
, m_displayHeight(0)
, m_blendingStatus(false)
, m_defaultShader(0)
, m_defaultShaderNoUniformAlpha(0)
, m_defaultShaderAddUniformChromaKey(0)
, m_currentLayer(0)
, m_texId(0)
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
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_RED_SIZE,        8,
            EGL_ALPHA_SIZE,      8,
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

// Bits 31:30 = numSurfaces
// Bit  29    = blend on/off
// Bit  28    = unused
// Bits 27:21 = Surface 4 Attributes
// Bits 20:14 = Surface 3 Attributes
// Bits 13:7  = Surface 2 Attributes
// Bits 6:0   = Surface 1 Attributes
#define SHADERKEY_MAX_SURFACES 4
#define SHADERKEY_NUM_SURF_SHIFT 30
#define SHADERKEY_NUM_SURF_MASK  (0x3 << SHADERKEY_NUM_SURF_SHIFT)

#define SHADERKEY_BLEND_SHIFT        29
#define SHADERKEY_BLEND_MASK         (0x1 << SHADERKEY_BLEND_SHIFT)

#define SHADERKEY_BITS_PER_SURF    7
#define SHADERKEY_CHROMAKEY_SHIFT(x)     (2 + (SHADERKEY_BITS_PER_SURF * x))
#define SHADERKEY_ALPHA_CHANNEL_SHIFT(x) (1 + (SHADERKEY_BITS_PER_SURF * x))
#define SHADERKEY_TRANSPARENCY_SHIFT(x)  (0 + (SHADERKEY_BITS_PER_SURF * x))
#define SHADERKEY_CHROMAKEY_MASK(x)     (0x1 << (SHADERKEY_CHROMAKEY_SHIFT(x)))
#define SHADERKEY_ALPHA_CHANNEL_MASK(x) (0x1 << (SHADERKEY_ALPHA_CHANNEL_SHIFT(x)))
#define SHADERKEY_TRANSPARENCY_MASK(x)  (0x1 << (SHADERKEY_TRANSPARENCY_SHIFT(x)))

unsigned int GLESGraphicsystem::shaderKey(int numSurfaces,
                                 int needsBlend,
                                 int hasTransparency0, int hasAlphaChannel0, int hasChromakey0,
                                 int hasTransparency1, int hasAlphaChannel1, int hasChromakey1,
                                 int hasTransparency2, int hasAlphaChannel2, int hasChromakey2,
                                 int hasTransparency3, int hasAlphaChannel3, int hasChromakey3)
{
    return (((numSurfaces << SHADERKEY_NUM_SURF_SHIFT) & SHADERKEY_NUM_SURF_MASK) |
            ((needsBlend << SHADERKEY_BLEND_SHIFT) & SHADERKEY_BLEND_MASK) |

            ((hasChromakey3 << SHADERKEY_CHROMAKEY_SHIFT(3)) & SHADERKEY_CHROMAKEY_MASK(3)) |
            ((hasChromakey2 << SHADERKEY_CHROMAKEY_SHIFT(2)) & SHADERKEY_CHROMAKEY_MASK(2)) |
            ((hasChromakey1 << SHADERKEY_CHROMAKEY_SHIFT(1)) & SHADERKEY_CHROMAKEY_MASK(1)) |
            ((hasChromakey0 << SHADERKEY_CHROMAKEY_SHIFT(0)) & SHADERKEY_CHROMAKEY_MASK(0)) |

            ((hasAlphaChannel3 << SHADERKEY_ALPHA_CHANNEL_SHIFT(3)) & SHADERKEY_ALPHA_CHANNEL_MASK(3)) |
            ((hasAlphaChannel2 << SHADERKEY_ALPHA_CHANNEL_SHIFT(2)) & SHADERKEY_ALPHA_CHANNEL_MASK(2)) |
            ((hasAlphaChannel1 << SHADERKEY_ALPHA_CHANNEL_SHIFT(1)) & SHADERKEY_ALPHA_CHANNEL_MASK(1)) |
            ((hasAlphaChannel0 << SHADERKEY_ALPHA_CHANNEL_SHIFT(0)) & SHADERKEY_ALPHA_CHANNEL_MASK(0)) |

            ((hasTransparency3 << SHADERKEY_TRANSPARENCY_SHIFT(3)) & SHADERKEY_TRANSPARENCY_MASK(3)) |
            ((hasTransparency2 << SHADERKEY_TRANSPARENCY_SHIFT(2)) & SHADERKEY_TRANSPARENCY_MASK(2)) |
            ((hasTransparency1 << SHADERKEY_TRANSPARENCY_SHIFT(1)) & SHADERKEY_TRANSPARENCY_MASK(1)) |
            ((hasTransparency0 << SHADERKEY_TRANSPARENCY_SHIFT(0)) & SHADERKEY_TRANSPARENCY_MASK(0)));
}

void GLESGraphicsystem::debugShaderKey(unsigned int key)
{
    int numSurfaces = ((key & SHADERKEY_NUM_SURF_MASK) >> SHADERKEY_NUM_SURF_SHIFT);
    int needsBlend =  ((key & SHADERKEY_BLEND_MASK) >> SHADERKEY_BLEND_SHIFT);

    LOG_DEBUG("GLESGraphicsystem", "Key: " << std::hex << key
                               << " numSurfaces:" << numSurfaces
                               << " needsBlend:" << needsBlend);

    for (int i = 0; i < SHADERKEY_MAX_SURFACES; i++)
    {
        int hasChromakey =    ((key & SHADERKEY_CHROMAKEY_MASK(i)) >> SHADERKEY_CHROMAKEY_SHIFT(i));
        int hasAlphaChannel = ((key & SHADERKEY_ALPHA_CHANNEL_MASK(i)) >> SHADERKEY_ALPHA_CHANNEL_SHIFT(i));
        int hasTransparency = ((key & SHADERKEY_TRANSPARENCY_MASK(i)) >> SHADERKEY_TRANSPARENCY_SHIFT(i));
        LOG_DEBUG("GLESGraphicsystem", "    Surface" << i << ":"
                                   << " hasTransparency:" << hasTransparency
                                   << " hasAlphaChannel:" << hasAlphaChannel
                                   << " hasChromakey:" << hasChromakey);
    }
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
        bool bChromaKeyEnabled = m_currentLayer->getChromaKeyEnabled();
        if (bChromaKeyEnabled && !setupTextureForChromaKey())
        {
            LOG_WARNING("GLESGraphicsystem", "Failed to create Pbuffer. Layer chroma key to be disabled.");
            bChromaKeyEnabled = false;
        }

        SurfaceList surfaces = m_currentLayer->getAllSurfaces();
        for(std::list<Surface*>::const_iterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++)
        {
            if ((*currentS)->hasNativeContent() && (*currentS)->visibility && (*currentS)->opacity>0.0f)
            {
                renderSurface(*currentS);
            }
        }

        if (bChromaKeyEnabled)
        {
            activateGraphicContext();
            renderTempTexture();
            destroyTempTexture();
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

Shader *GLESGraphicsystem::pickOptimizedShader(SurfaceList surfaces, bool needsBlend)
{
    int numSurfaces = surfaces.size();

    if (numSurfaces > SHADERKEY_MAX_SURFACES)
    {
        return NULL;
    }

    int hastransparency[SHADERKEY_MAX_SURFACES] = {0};
    int hasalphachannel[SHADERKEY_MAX_SURFACES] = {0};
    int haschromakey[SHADERKEY_MAX_SURFACES] = {0};

    int i = 0;
    for (SurfaceListConstIterator surface = surfaces.begin(); surface != surfaces.end(); surface++, i++)
    {
        int layerId = (*surface)->getContainingLayerId();
        Layer* layer = m_baseWindowSystem->m_pScene->getLayer(layerId);

        hastransparency[i] = ((*surface)->getOpacity() * layer->getOpacity()) < 1.0f;
        hasalphachannel[i] = PixelFormatHasAlpha((*surface)->getPixelFormat());
        haschromakey[i] = 0;
    }

    unsigned int key = shaderKey(numSurfaces,
                        needsBlend ? 1 : 0,
                        hastransparency[0], hasalphachannel[0], haschromakey[0],
                        hastransparency[1], hasalphachannel[1], haschromakey[1],
                        hastransparency[2], hasalphachannel[2], haschromakey[2],
                        hastransparency[3], hasalphachannel[3], haschromakey[3]);

    std::map<int, Shader*>::const_iterator iter = m_shaders.find(key);

//    LOG_DEBUG("GLESGraphicsystem", "Looking for shader with key=" << std::hex << key);
//    debugShaderKey(key);

    if (iter == m_shaders.end())
    {
        LOG_WARNING("GLESGraphicsystem", "No optimal shader found for key=" << std::hex << key);
        LOG_WARNING("GLESGraphicsystem", "Falling back to default shader");

        switch (numSurfaces) {
        case 1:
            return m_defaultShader;
        default:
            return NULL;
        }
    }

    return (*iter).second;
}

bool GLESGraphicsystem::needsBlending(SurfaceList surfaces)
{
    // Completely opaque surfaces don't need to be blended with framebuffer.
    // Look for any possible translucency.
    for (SurfaceListConstIterator surface = surfaces.begin(); surface != surfaces.end(); surface++)
    {
        if (PixelFormatHasAlpha((*surface)->getPixelFormat()))
        {
            return true;
        }

        int layerId = (*surface)->getContainingLayerId();
        Layer* layer = m_baseWindowSystem->m_pScene->getLayer(layerId);

        if (((*surface)->getOpacity() * layer->getOpacity()) < 1.0f)
        {
            return true;
        }
    }
    return false;
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

    {
        SurfaceList sl;
        sl.push_back(surface);
        bool needblend = shader != m_defaultShader ||
                         PixelFormatHasAlpha((surface)->getPixelFormat()) ||
                         uniforms.opacity < 1.0f;
        shader = pickOptimizedShader(sl, needblend);
    }

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

    if (!m_defaultShader || !m_defaultShaderNoUniformAlpha || !m_defaultShaderAddUniformChromaKey )
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
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        m_blendingStatus = true;
        glClearColor(0.0, 0.0, 0.0, 0.0);
        resize(displayWidth, displayHeight);
        glActiveTexture(GL_TEXTURE0);
    }

    //   Build Shader Map
    //  --------------------------------------------
    //   #: Number of Textures
    //   B: Blends with framebuffer
    //   T: Texture n has layer/surface transparency (layerAlpha != 1 or surfaceAlpha != 1)
    //   A: Texture n has alpha channel
    //   C: Texture n has chromakey
    //                         0      1      2      3
    //                       -----  -----  -----  -----
    //                  # B  T A C  T A C  T A C  T A C
    m_shaders[shaderKey(1,1, 1,1,0, 0,0,0, 0,0,0, 0,0,0)] = m_defaultShader;
    m_shaders[shaderKey(1,1, 0,1,0, 0,0,0, 0,0,0, 0,0,0)] = m_defaultShaderNoUniformAlpha;

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

bool GLESGraphicsystem::setupTextureForChromaKey()
{
    createPbufferSurface();
    if (m_eglPbufferSurface == EGL_NO_SURFACE)
    {
        return false;
    }

    // Switch the current context to pbuffer surface
    eglMakeCurrent(m_eglDisplay, m_eglPbufferSurface, m_eglPbufferSurface, m_eglContext);
    createTempTexture();
    return true;
}

void GLESGraphicsystem::createPbufferSurface()
{
    if (m_eglPbufferSurface != EGL_NO_SURFACE)
    {
        LOG_DEBUG("GLESGraphicsystem", "m_eglPbufferSurface is alread created");
        return;
    }

    const FloatRectangle layerDestRegion = m_currentLayer->getDestinationRegion();
    EGLint width  = static_cast<EGLint>(layerDestRegion.width);
    EGLint height = static_cast<EGLint>(layerDestRegion.height);

    EGLint pb_attrs[] = {
        EGL_WIDTH,  width,
        EGL_HEIGHT, height,
        EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGBA,
        EGL_TEXTURE_TARGET, EGL_TEXTURE_2D,
        EGL_NONE
    };

    m_eglPbufferSurface = eglCreatePbufferSurface(m_eglDisplay, m_eglConfig, pb_attrs);
    if (m_eglPbufferSurface == EGL_NO_SURFACE)
    {
        LOG_ERROR("GLESGraphicsystem", "Failed to create EGL pbuffer: " << eglGetError());
        return;
    }
}

void GLESGraphicsystem::createTempTexture()
{
    glGenTextures(1, &m_texId);
    glBindTexture(GL_TEXTURE_2D, m_texId);

    if (!eglBindTexImage(m_eglDisplay, m_eglPbufferSurface, EGL_BACK_BUFFER))
    {
        LOG_ERROR("GLESGraphicsystem", "Failed to bind texture for chroma key layer");
        glDeleteTextures(1, &m_texId);
        return;
    }

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void GLESGraphicsystem::destroyTempTexture()
{
    if (m_texId > 0)
    {
        glDeleteTextures(1, &m_texId);
    }

    if (m_eglPbufferSurface != EGL_NO_SURFACE)
    {
        eglReleaseTexImage(m_eglDisplay, m_eglPbufferSurface, EGL_BACK_BUFFER);
        eglDestroySurface(m_eglDisplay, m_eglPbufferSurface);
    }

    m_texId = 0;
    m_eglPbufferSurface = EGL_NO_SURFACE;
}

void GLESGraphicsystem::renderTempTexture()
{
    const FloatRectangle layerSourceRegion      = m_currentLayer->getSourceRegion();
    const FloatRectangle layerDestinationRegion = m_currentLayer->getDestinationRegion();
    float textureCoordinates[4];

    ViewportTransform::transformRectangleToTextureCoordinates(
        layerSourceRegion,
        m_currentLayer->OriginalSourceWidth,
        m_currentLayer->OriginalSourceHeight,
        textureCoordinates);

    ShaderProgram::CommonUniforms uniforms;
    IlmMatrix layerMatrix;
    IlmMatrixIdentity(layerMatrix);

    uniforms.x = layerDestinationRegion.x / m_displayWidth;
    uniforms.y = 1.0f - (layerDestinationRegion.y + layerDestinationRegion.height) / m_displayHeight;
    uniforms.width   = layerDestinationRegion.width  / m_displayWidth;
    uniforms.height  = layerDestinationRegion.height / m_displayHeight;
    uniforms.opacity = m_currentLayer->getOpacity();
    uniforms.texRange[0]  = textureCoordinates[2] - textureCoordinates[0];
    uniforms.texRange[1]  = textureCoordinates[3] - textureCoordinates[1];
    uniforms.texOffset[0] = textureCoordinates[0];
    uniforms.texOffset[1] = textureCoordinates[1];
    uniforms.texUnit = 0;
    uniforms.matrix = layerMatrix.f;
    uniforms.chromaKeyEnabled = m_currentLayer->getChromaKeyEnabled();
    if (uniforms.chromaKeyEnabled == true) {
        unsigned char red   = 0;
        unsigned char green = 0;
        unsigned char blue  = 0;
        m_currentLayer->getChromaKey(red, green, blue);
        uniforms.chromaKey[0] = (float)red   / 255.0f;
        uniforms.chromaKey[1] = (float)green / 255.0f;
        uniforms.chromaKey[2] = (float)blue  / 255.0f;
    }

    glEnable (GL_BLEND);

    Shader* shader = m_currentLayer->getShader();
    if (!shader) {
        shader = m_defaultShader;
    }
    shader = pickOptimizedShader(shader, uniforms);
    shader->use();
    shader->loadCommonUniforms(uniforms);
    shader->loadUniforms();

    glBindTexture(GL_TEXTURE_2D, m_texId);

    int orientation = m_currentLayer->getOrientation() % 4;
    GLint index     = orientation * 12;
    glDrawArrays(GL_TRIANGLES, index, 6);

    GLenum glErrorCode = glGetError();
    if ( GL_NO_ERROR != glErrorCode ) {
        LOG_ERROR("GLESGraphicsystem", "GL Error occured in renderTempTexture:" << glErrorCode );
    }
}
