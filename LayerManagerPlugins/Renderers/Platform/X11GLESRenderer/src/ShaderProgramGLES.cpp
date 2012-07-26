/***************************************************************************
*
* Copyright 2010 BMW Car IT GmbH
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

#include "ShaderProgramGLES.h"
#include "config.h"
#include <GLES2/gl2ext.h>
#include <RenderUtil.h>
#include <Log.h>

ShaderProgram* ShaderProgramGLES::createProgram(const std::string& vertName, const std::string& fragName)
{
    GLuint progHandle;
    ShaderProgramGLES* program = 0;
    char defaultShaderDir[1024];
    char fragmentShaderLocation[1024];
    char vertexShaderLocation[1024];
    
    const char* pluginLookupPath = getenv("LM_PLUGIN_PATH");
    if  (pluginLookupPath != NULL ) 
    {
        strncpy(defaultShaderDir, pluginLookupPath, sizeof(defaultShaderDir) - 1);
    }
    else
    {
        strncpy(defaultShaderDir, CMAKE_INSTALL_PREFIX"/lib/layermanager", sizeof(defaultShaderDir));
    }
    strcat(defaultShaderDir,"/renderer");

    if (vertName=="default" && fragName=="default")
    {
        strcpy(vertexShaderLocation,defaultShaderDir);
        strcpy(fragmentShaderLocation,defaultShaderDir);
        strcat(vertexShaderLocation,"/renderer_vert.glslv");
        strcat(fragmentShaderLocation,"/renderer_frag.glslf");
        // load default shader from binary data
        progHandle = RenderUtilLoadShaderSources(vertexShaderLocation,fragmentShaderLocation, GL_TRUE);
    }
	else if (vertName=="default" && fragName=="default_no_uniform_alpha")
    {
        strcpy(vertexShaderLocation,defaultShaderDir);
        strcpy(fragmentShaderLocation,defaultShaderDir);
        strcat(vertexShaderLocation,"/renderer_vert.glslv");
        strcat(fragmentShaderLocation,"/renderer_frag_no_ualpha.glslf");
        progHandle = RenderUtilLoadShaderSources(vertexShaderLocation,fragmentShaderLocation, GL_TRUE);
    }
    else if (vertName=="default" && fragName=="default_add_uniform_chromakey")
    {
        strcpy(vertexShaderLocation,defaultShaderDir);
        strcpy(fragmentShaderLocation,defaultShaderDir);
        strcat(vertexShaderLocation,"/renderer_vert.glslv");
        strcat(fragmentShaderLocation,"/renderer_frag_add_uchromakey.glslf");
        progHandle = RenderUtilLoadShaderSources(vertexShaderLocation,fragmentShaderLocation, GL_TRUE);
    }
	else
	{
		// load shader sources from file, compile and link them:
		progHandle = RenderUtilLoadShaderSources(vertName.c_str(), fragName.c_str(), GL_TRUE);
	}

    if (progHandle != 0)
    {
        // bind attrib locations for vertex positions and texture coordinates
        glBindAttribLocation(progHandle, 0, "aPosition");
        glBindAttribLocation(progHandle, 1, "aTexCoords");

        // re-link the program as we have changed the attrib bindings
        glLinkProgram(progHandle);

        program = new ShaderProgramGLES(vertName, fragName, progHandle);
    }
    else
    {
        LOG_ERROR("ShaderProgramGLES", "Failed to create and link shader program");
    }

    return program;
}

ShaderProgramGLES::ShaderProgramGLES(const std::string& vertName, const std::string& fragName, GLuint handle)
: ShaderProgram(vertName, fragName)
, m_progHandle(handle)
{
    // Update the uniform locations.
    // Don't move this call to the base class constructor as we need
    // to set the OpenGL program handle first.
    updateCommonUniformLocations();
}

ShaderProgramGLES::~ShaderProgramGLES(void)
{
    if (m_progHandle)
    {
        glDeleteProgram(m_progHandle);
    }
}

void ShaderProgramGLES::use(void) const
{
    glUseProgram(m_progHandle);
}

