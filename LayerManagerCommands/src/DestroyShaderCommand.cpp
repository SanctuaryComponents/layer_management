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
#include "DestroyShaderCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"
#include "ShaderMap.h"

DestroyShaderCommand::DestroyShaderCommand(unsigned int shaderid)
: m_id(shaderid)
{
}

ExecutionResult DestroyShaderCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());
    ShaderMap& shaderMap = scene.m_shaderMap;

    // get shader by its ID
    ShaderMapIterator iter = shaderMap.find(m_id);
    ShaderMapIterator iterEnd = shaderMap.end();

    if (iter == iterEnd)
    {
        // shader not found
        LOG_ERROR("Layermanager", "shader ID " << m_id << " not found");
        return ExecutionFailed;
    }

    Shader* shader = (*iter).second;

    if (shader)
    {
        const SurfaceMap& surfaceMap = scene.getAllSurfaces();
        removeShaderFromAllSurfaces(surfaceMap, shader);

        const LayerMap& layerMap = scene.getAllLayers();
        removeShaderFromAllLayers(layerMap, shader);

        // remove shader from global shader list
        scene.m_shaderMap.erase(iter);

        // finally delete it
        delete shader;
    }

    return ExecutionSuccessRedraw;
}

/// detach shader from all surfaces, surface groups, etc... from surfaces
void DestroyShaderCommand::removeShaderFromAllSurfaces(const SurfaceMap & surfaceMap, Shader* shader)
{
    SurfaceMapConstIterator iter = surfaceMap.begin();
    SurfaceMapConstIterator iterEnd = surfaceMap.end();
    for (; iter != iterEnd; ++iter)
    {
        GraphicalObject* obj = iter->second;

        if (obj && obj->getShader() == shader)
        {
            obj->setShader(NULL);
        }
    }
}

void DestroyShaderCommand::removeShaderFromAllLayers(const LayerMap & layerMap, Shader *& shader)
{
    LayerMapConstIterator iter = layerMap.begin();
    LayerMapConstIterator iterEnd = layerMap.end();
    for (; iter != iterEnd; ++iter)
    {
        GraphicalObject* obj = iter->second;

        if (obj && obj->getShader()==shader)
        {
            obj->setShader(NULL);
        }
    }
}

unsigned int DestroyShaderCommand::getShaderID() const
{
    return m_id;
}

const std::string DestroyShaderCommand::getString()
{
    std::stringstream description;
    description << "DestroyShaderCommand("
                << "id=" << m_id
                << ")";
    return description.str();
}
