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
#include "RemoveCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"
#include <sstream>

RemoveCommand::RemoveCommand(unsigned int objectID, ObjectType typeToRemove)
: m_idToRemove(objectID)
, m_typeToRemove(typeToRemove)
{
}

ExecutionResult RemoveCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

    switch(m_typeToRemove)
    {
        case TypeSurface:
        {
            Surface* surface = scene.getSurface(m_idToRemove);
            if (surface)
            {
                scene.removeSurface(surface);
                result = ExecutionSuccessRedraw;
            }
            break;
         }

        case TypeLayer:
        {
            Layer* layer = scene.getLayer(m_idToRemove);
            if (layer)
            {
                scene.removeLayer(layer);
                result = ExecutionSuccessRedraw;
            }
            break;
        }

        case TypeSurfaceGroup:
        {
            SurfaceGroup* surfacegroup = scene.getSurfaceGroup(m_idToRemove);
            if (surfacegroup)
            {
                scene.removeSurfaceGroup(surfacegroup);
                result = ExecutionSuccessRedraw;
            }
            break;
        }

        case TypeLayerGroup:
        {
            LayerGroup* layergroup = scene.getLayerGroup(m_idToRemove);
            if (layergroup)
            {
                scene.removeLayerGroup(layergroup);
                result = ExecutionSuccessRedraw;
            }
            break;
        }

        default:
            LOG_ERROR("RemoveCommand", "could not destroy unknown type.");
            break;
    }

    return result;
}

const std::string RemoveCommand::getString()
{
    std::stringstream description;
    description << "RemoveCommand("
                << "idToRemove=" << m_idToRemove
                << ", typeToRemove=" << m_typeToRemove
                << ")";
    return description.str();
}
