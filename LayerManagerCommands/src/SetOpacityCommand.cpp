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
#include "SetOpacityCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

SetOpacityCommand::SetOpacityCommand(unsigned int id, ObjectType type, double Opacity)
: m_id(id)
, m_type(type)
, m_opacity(Opacity)
{
}

ExecutionResult SetOpacityCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

    GraphicalObject* gs = 0;

    switch(m_type)
    {
        case TypeSurface:
            gs = scene.getSurface(m_id);
            break;

        case TypeLayer:
            gs = scene.getLayer(m_id);
            break;

        case TypeSurfaceGroup:
            gs = scene.getSurfaceGroup(m_id);
            break;

        case TypeLayerGroup:
            gs = scene.getLayerGroup(m_id);
            break;

        default:
            break;
    }

    if (gs)
    {
        LOG_DEBUG("SetOpacityCommand","new opacity " << m_opacity << " for id: " << m_id);
        gs->setOpacity(m_opacity);
        result = ExecutionSuccessRedraw;
    }

    return result;
}

const std::string SetOpacityCommand::getString()
{
    std::stringstream description;
    description << "SetOpacityCommand("
                << "id=" << m_id
                << ", type=" << m_type
                << ", opacity=" << m_opacity
                << ")";
    return description.str();
}
