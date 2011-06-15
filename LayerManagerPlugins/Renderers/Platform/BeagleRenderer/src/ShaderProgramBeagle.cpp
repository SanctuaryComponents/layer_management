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

#include <ShaderProgramBeagle.h>
#include <RenderUtil.h>
#include <Log.h>


ShaderProgram* ShaderProgramBeagle::createProgram(const std::string& vertName, const std::string& fragName)
{
    GLuint progHandle;
    ShaderProgramBeagle* program = 0;

    if (vertName=="default" && fragName=="default")
    {
        progHandle = RenderUtilLoadShaderSources("renderer_vert.glslv", "renderer_frag.glslf", GL_TRUE);
    }
    else
    {
        // load shader sources from file, compile and link them:
        progHandle = RenderUtilLoadShaderSources(vertName.c_str(), fragName.c_str(), GL_TRUE);
    }

    if (progHandle!=0)
    {
        // bind attrib locations for vertex positions and texture coordinates
        glBindAttribLocation(progHandle, 0, "aPosition");
        glBindAttribLocation(progHandle, 1, "aTexCoords");

        // re-link the program as we have changed the attrib bindings
        glLinkProgram(progHandle);

        program = new ShaderProgramBeagle(vertName, fragName, progHandle);
    }
    else
    {
        LOG_ERROR("ShaderProgramBeagle", "Failed to create and link shader program");
    }

    return program;
}

ShaderProgramBeagle::ShaderProgramBeagle(const std::string& vertName, const std::string& fragName, GLuint handle)
    : ShaderProgram(vertName, fragName)
    , _progHandle(handle)
{
    // Update the uniform locations.
    // Don't move this call to the base class constructor as we need
    // to set the OpenGL program handle first.
    updateCommonUniformLocations();
}

ShaderProgramBeagle::~ShaderProgramBeagle(void)
{
    if (_progHandle)
    {
        glDeleteProgram(_progHandle);
    }
}

void ShaderProgramBeagle::use(void) const
{
    glUseProgram(_progHandle);
}

