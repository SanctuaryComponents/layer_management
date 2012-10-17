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
#include "SurfacegroupRemoveSurfaceCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

ExecutionResult SurfacegroupRemoveSurfaceCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

    SurfaceGroup* surfacegroup = scene.getSurfaceGroup(m_surfacegroupid);
    Surface* surface = scene.getSurface(m_surfaceid);

    if (surfacegroup && surface)
    {
        surfacegroup->removeElement(surface);
        result = ExecutionSuccess;
    }

    return result;
}

const std::string SurfacegroupRemoveSurfaceCommand::getString()
{
    std::stringstream description;
    description << "SurfacegroupRemoveSurfaceCommand("
                << "surfacegroupid=" << m_surfacegroupid
                << ", surfaceid=" << m_surfaceid
                << ")";
    return description.str();
}
