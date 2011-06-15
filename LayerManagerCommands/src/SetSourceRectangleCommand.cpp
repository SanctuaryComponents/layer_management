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
#include "SetSourceRectangleCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

SetSourceRectangleCommand::SetSourceRectangleCommand(int id, ObjectType type, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
: m_id(id)
, m_typeToSet(type)
, m_x(x)
, m_y(y)
, m_width(width)
, m_height(height)
{
}

ExecutionResult SetSourceRectangleCommand::execute(ICommandExecutor* executor)
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

    if (graphicalSurface )
    {
        const Rectangle rectangle(m_x, m_y, m_width, m_height);
        graphicalSurface->setSourceRegion(rectangle);
        result = ExecutionSuccessRedraw;
    }

    return result;
}

const std::string SetSourceRectangleCommand::getString()
{
    std::stringstream description;
    description << "SetSourceRectangleCommand("
                << "id=" << m_id
                << ", typeToSet=" << m_typeToSet
                << ", x=" << m_x
                << ", y=" << m_y
                << ", width=" << m_width
                << ", height=" << m_height
                << ")";
    return description.str();
}
