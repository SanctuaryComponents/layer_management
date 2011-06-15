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
#include "SetLayerOrderCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

SetLayerOrderCommand::SetLayerOrderCommand(unsigned int* array, unsigned int length)
: m_array(array)
, m_length(length)
{
}

ExecutionResult SetLayerOrderCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    // check for doubles
    for (unsigned int i = 0; i < m_length; i++)
    {
        for (unsigned int c = i + 1; c < m_length; c++)
        {
            if (m_array[i] == m_array[c])
            {
                // doubles not allowed here
                return ExecutionFailed;
            }
        }
    }

    // taken out because other software currently expects partial execution of command,
    // i.e. the layers that exist are added to the render order

    //        // check that all layers to be added exist
    //        for (unsigned int i=0;i<length;i++){
    //            Layer* l = layerlist.getLayer(array[i]);
    //            if (NULL==l)
    //                return false;
    //        } // TODO insert again later

    scene.getCurrentRenderOrder().clear();

    LOG_DEBUG("SetLayerOrderCommand", "Length to set: " << m_length);

    ExecutionResult result = ExecutionFailed;

    for (unsigned int i = 0; i < m_length; i++)
    {
        LOG_DEBUG("SetLayerOrderCommand", "Trying to add layer: " << m_array[i] << " to current render order");
        Layer* layer = scene.getLayer(m_array[i]);
        if (layer)
        {
            LOG_DEBUG("SetLayerOrderCommand", "Adding Layer: " << m_array[i] << " to current render order");
            scene.getCurrentRenderOrder().push_back(layer);
            result = ExecutionSuccessRedraw;
        }
    }
    return result;
}

const std::string SetLayerOrderCommand::getString()
{
    std::stringstream description;
    description << "SetLayerOrderCommand("
                << "m_array=[";

    for (unsigned int i = 0; i < m_length; ++i)
    {
        description << m_array[i] << ",";
    }
    description << "], m_length=" << m_length
                << ")";
    return description.str();
}
