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
#include "ShaderCreateCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

ShaderCreateCommand::ShaderCreateCommand(const std::string& vertName, const std::string& fragName, unsigned int* id)
: m_vertName(vertName)
, m_fragName(fragName)
, m_returnID(id)
{
}

ExecutionResult ShaderCreateCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

    Shader* shader;

    // create new shader instance
    shader = Shader::createShader(m_vertName, m_fragName);

    if (shader)
    {
        // insert shader to shader map
        int id = shader->getId();
        ShaderMap shaderMap = scene.m_shaderMap;
        shaderMap[id] = shader;
        *m_returnID = id;
        result = ExecutionSuccessRedraw;
    }

    return result;
}

const std::string ShaderCreateCommand::getVertName() const
{
    return m_vertName;
}

const std::string ShaderCreateCommand::getFragName() const
{
    return m_fragName;
}

const std::string ShaderCreateCommand::getString()
{
    std::stringstream description;
    description << "ShaderCreateCommand("
                << "vertName=" << m_vertName
                << ", fragName=" << m_fragName
                << ")";
    return description.str();
}

