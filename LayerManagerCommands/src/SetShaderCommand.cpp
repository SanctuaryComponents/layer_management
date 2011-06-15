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
#include "SetShaderCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "ShaderMap.h"
#include "Log.h"

SetShaderCommand::SetShaderCommand(unsigned int id,unsigned  int shaderid)
: m_id(id)
, m_shaderid(shaderid)
{
}

unsigned int SetShaderCommand::getID() const
{
    return m_id;
}

unsigned int SetShaderCommand::getShaderID() const
{
    return m_shaderid;
}

ExecutionResult SetShaderCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

    GraphicalObject* object = scene.getSurface(m_id);

    if (object)
    {
        Shader* shader = NULL;

        // get shader by its ID
        ShaderMap shaderMap = scene.m_shaderMap;
        ShaderMapIterator iter = shaderMap.find(m_shaderid);
        ShaderMapIterator iterEnd = shaderMap.end();
        if (iter != iterEnd)
        {
            shader = (*iter).second;
        }

        if (shader || m_shaderid == 0)
        {
            // shaderId==0 detaches the custom shader
            // (shader is supposed to be NULL in this case)

            object->setShader(shader);
            result = ExecutionSuccessRedraw;
        }
        else
        {
            // shader not found
            LOG_ERROR("Layermanager", "shader ID " << m_shaderid << " not found");
        }
    }
    else
    {
        // object not found
        LOG_ERROR("Layermanager", "object ID " << m_id << " not found");
    }

    return result;
}

const std::string SetShaderCommand::getString()
{
    std::stringstream description;
    description << "SetShaderCommand("
                << "id=" << m_id
                << ", shaderid=" << m_shaderid
                << ")";
    return description.str();
}

