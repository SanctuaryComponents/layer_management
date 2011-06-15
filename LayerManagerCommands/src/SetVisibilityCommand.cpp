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
#include "SetVisibilityCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

SetVisibilityCommand::SetVisibilityCommand(const uint givenid, ObjectType Objecttype, bool newvisibility)
: m_idtoSet(givenid)
, m_otype(Objecttype)
, m_visibility(newvisibility)
{
}

ExecutionResult SetVisibilityCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

    GraphicalObject* go = NULL;

    switch(m_otype)
    {
        case TypeSurface:
            go = scene.getSurface(m_idtoSet);
            break;

        case TypeLayer:
            go = scene.getLayer(m_idtoSet);
            break;

        case TypeSurfaceGroup:
            go = scene.getSurfaceGroup(m_idtoSet);
            break;

        case TypeLayerGroup:
            go = scene.getLayerGroup(m_idtoSet);
            break;

        default:
            break;
    }

    if (go)
    {
        LOG_DEBUG("SetVisibilityCommand", "setting visibility: " << m_visibility << " of id " << m_idtoSet);
        go->setVisibility(m_visibility);
        result = ExecutionSuccessRedraw;
    }

    return result;
}

const std::string SetVisibilityCommand::getString()
{
    std::stringstream description;
    description << "SetVisibilityCommand("
                << "idtoSet=" << m_idtoSet
                << ", otype=" << m_otype
                << ", visibility=" << m_visibility
                << ")";
    return description.str();
}
