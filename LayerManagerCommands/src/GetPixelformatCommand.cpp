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
#include "GetPixelformatCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

GetPixelformatCommand::GetPixelformatCommand(int id, ObjectType type, PixelFormat* f)
: m_id(id)
, m_type(type)
, m_pFormatreturn(f)
{
}

ExecutionResult GetPixelformatCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

    switch(m_type)
    {
        case TypeSurface:
        {
            Surface* surface = scene.getSurface(m_id);
            if (surface)
            {
                *m_pFormatreturn = surface->getPixelFormat();
                result = ExecutionSuccess;
            }
            break;
        }

        case TypeLayer:
        case TypeSurfaceGroup:
        case TypeLayerGroup:
        default:
            break;
    }

    return result;
}

const std::string GetPixelformatCommand::getString()
{
    std::stringstream description;
    description << "GetPixelformatCommand("
                << "id=" << m_id
                << ", type=" << m_type
                << ", pFormatreturn=" << m_pFormatreturn << "=" << *m_pFormatreturn
                << ")";
    return description.str();
}
