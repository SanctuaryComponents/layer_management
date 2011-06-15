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
#include "GetVisibilityCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

GetVisibilityCommand::GetVisibilityCommand(int id, ObjectType type, bool* visibility)
: m_id(id)
, m_type(type)
, m_pVisibility(visibility)
{
}

ExecutionResult GetVisibilityCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    LOG_DEBUG("GetVisibilityCommand", "start of");

    ExecutionResult result = ExecutionFailed;

    GraphicalObject* go = 0;

    switch(m_type)
    {
        case TypeSurface:
            go = scene.getSurface(m_id);
            break;

        case TypeLayer:
            go = scene.getLayer(m_id);
            break;

        case TypeSurfaceGroup:
            go = scene.getSurfaceGroup(m_id);
            break;

        case TypeLayerGroup:
            go = scene.getLayerGroup(m_id);
            break;

        default:
            break;
    }

    if (go)
    {
        *m_pVisibility = go->getVisibility();
        LOG_DEBUG("GetVisibilityCommand", "returned:" << *m_pVisibility);
        result = ExecutionSuccess;
    }
    return result;
}

const std::string GetVisibilityCommand::getString()
{
    std::stringstream description;
    description << "GetVisibilityCommand("
                << "id=" << m_id
                << ", type=" << m_type
                << ", pVisibility=" << m_pVisibility << "=" << *m_pVisibility
                << ")";
    return description.str();
}
