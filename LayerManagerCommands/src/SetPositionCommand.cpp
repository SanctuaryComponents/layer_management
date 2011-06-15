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
#include "SetPositionCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

SetPositionCommand::SetPositionCommand(unsigned int id, ObjectType typeToSetOn, unsigned int x, unsigned int y )
: m_id(id)
, m_typeToSet(typeToSetOn)
, m_x(x)
, m_y(y)
{
}

ExecutionResult SetPositionCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

    GraphicalSurface* graphicalSurface = 0;

    switch(m_typeToSet)
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
        graphicalSurface->setPosition(m_x, m_y);
        result = ExecutionSuccessRedraw;
    }
    return result;
}

const std::string SetPositionCommand::getString()
{
    std::stringstream description;
    description << "SetPositionCommand("
                << "id=" << m_id
                << ", typeToSet=" << m_typeToSet
                << ", m_x=" << m_x
                << ", m_y=" << m_y
                << ")";
    return description.str();
}
