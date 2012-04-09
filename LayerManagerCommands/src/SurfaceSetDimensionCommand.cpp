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
#include "SurfaceSetDimensionCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

SurfaceSetDimensionCommand::SurfaceSetDimensionCommand(int id, unsigned int width,unsigned  int height)
: m_id(id)
, m_width(width)
, m_height(height)
{
}

ExecutionResult SurfaceSetDimensionCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

    GraphicalSurface* surface = scene.getSurface(m_id);

    if (surface)
    {
        result = surface->setDimension(m_width, m_height) ? ExecutionSuccessRedraw : ExecutionSuccess;
    }

    return result;
}

const std::string SurfaceSetDimensionCommand::getString()
{
    std::stringstream description;
    description << "SurfaceSetDimensionCommand("
                << "id=" << m_id
                << ", width=" << m_width
                << ", height=" << m_height
                << ")";
    return description.str();
}
