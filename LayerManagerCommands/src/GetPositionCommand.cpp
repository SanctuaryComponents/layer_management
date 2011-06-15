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
#include "GetPositionCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

GetPositionCommand::GetPositionCommand(int id, ObjectType type, unsigned int* xRet, unsigned int* yRet)
: m_id(id)
, m_type(type)
, m_pX(xRet)
, m_pY(yRet)
{
}

ExecutionResult GetPositionCommand::execute(ICommandExecutor* execute)
{
    Scene& scene = *(execute->getScene());

    ExecutionResult result = ExecutionFailed;

    GraphicalSurface* go = 0;

    switch(m_type)
    {
        case TypeSurface:
            go = scene.getSurface(m_id);
            break;

        case TypeLayer:
            go = scene.getLayer(m_id);
            break;

        case TypeSurfaceGroup:
        case TypeLayerGroup:
        default:
            break;
    }

    if (go)
    {
        Vector2 dim = go->getPosition();
        *m_pX = dim.val1;
        *m_pY = dim.val2;
        result = ExecutionSuccess;
    }
    else
    {
        *m_pX = 0;
        *m_pY = 0;
    }

    return result;
}

const std::string GetPositionCommand::getString()
{
    std::stringstream description;
    description << "GetPositionCommand("
                << "id=" << m_id
                << ", type=" << m_type
                << ", pX=" << m_pX << "=" << *m_pX
                << ", pY=" << m_pY << "=" << *m_pY
                << ")";
    return description.str();
}

