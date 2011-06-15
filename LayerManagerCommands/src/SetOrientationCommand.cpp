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
#include "SetOrientationCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

SetOrientationCommand::SetOrientationCommand(unsigned int id, ObjectType type, OrientationType Orientation)
: m_id(id)
, m_type(type)
, m_orientation(Orientation)
{
}

ExecutionResult SetOrientationCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;
    GraphicalSurface* graphicalSurface = NULL;

    switch(m_type)
    {
        case TypeSurface:
            graphicalSurface = scene.getSurface(m_id);
            break;

        case TypeLayer:
            graphicalSurface = scene.getLayer(m_id);
            break;

        case TypeSurfaceGroup:
        case TypeLayerGroup:
        default:
            break;
    }

    if (graphicalSurface)
    {
        graphicalSurface->setOrientation(m_orientation);
        result = ExecutionSuccessRedraw;
    }

    return result;
}

const std::string SetOrientationCommand::getString()
{
    std::stringstream description;
    description << "SetOrientationCommand("
                << "id=" << m_id
                << ", type=" << m_type
                << ", orientation=" << m_orientation
                << ")";
    return description.str();
}
