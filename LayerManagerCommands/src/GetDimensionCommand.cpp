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
#include "GetDimensionCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

GetDimensionCommand::GetDimensionCommand(int id, ObjectType type, uint* widthRet, uint* heightRet)
: m_id(id)
, m_type(type)
, m_pWidth(widthRet)
, m_pHeight(heightRet)
{
}

ExecutionResult GetDimensionCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

    GraphicalSurface* graphicalObject = NULL;

    switch (m_type)
    {
        case TypeSurface:
            graphicalObject = scene.getSurface(m_id);
            break;

        case TypeLayer:
            graphicalObject = scene.getLayer(m_id);
            break;

        case TypeSurfaceGroup:
        case TypeLayerGroup:
        default:
            break;
    }

    if (graphicalObject)
    {
        Vector2 dim = graphicalObject->getDimension();
        *m_pWidth = dim.val1;
        *m_pHeight = dim.val2;
        result = ExecutionSuccess;
    }
    else
    {
        *m_pWidth = 0;
        *m_pHeight = 0;
    }

    return result;
}

const std::string GetDimensionCommand::getString()
{
    std::stringstream description;
    description << "GetDimensionCommand("
                << "id=" << m_id
                << ", type=" << m_type
                << ", pWidth=" << m_pWidth << "=" << *m_pWidth
                << ", m_pHeight=" << m_pHeight << "=" << *m_pHeight
                << ")";
    return description.str();
}
