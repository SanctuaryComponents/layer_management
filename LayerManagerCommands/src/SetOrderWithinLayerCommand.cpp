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
#include "SetOrderWithinLayerCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

SetOrderWithinLayerCommand::SetOrderWithinLayerCommand(unsigned int layerid,unsigned  int* array,unsigned  int length)
: m_layerid(layerid)
, m_array(array)
, m_length(length)
{
}

ExecutionResult SetOrderWithinLayerCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

    Layer* layer = scene.getLayer(m_layerid);

    if (layer)
    {
        layer->getAllSurfaces().clear();

        for (unsigned int surfaceIndex = 0; surfaceIndex < m_length; ++surfaceIndex)
        {
            Surface* surface = scene.getSurface(m_array[surfaceIndex]);

            if (surface)
            {
                LOG_DEBUG("SetOrderWithinLayerCommand","add surface " << surface->getID() << " to renderorder of layer " << m_layerid);
                layer->getAllSurfaces().push_back(surface);
                result = ExecutionSuccessRedraw;
            }
        }
    }
    delete[] m_array;

    return result;
}

const std::string SetOrderWithinLayerCommand::getString()
{
    std::stringstream description;
    description << "SetOrderWithinLayerCommand("
                << "layerid=" << m_layerid
                << ", m_array=[";

    for (unsigned int i = 0; i < m_length; ++i)
    {
        description << m_array[i] << ",";
    }
    description << "], m_length=" << m_length
                << ")";
    return description.str();
}