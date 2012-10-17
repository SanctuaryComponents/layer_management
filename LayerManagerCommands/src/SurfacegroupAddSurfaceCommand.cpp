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
#include "SurfacegroupAddSurfaceCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "SurfaceList.h"
#include "Log.h"
#include <algorithm>

ExecutionResult SurfacegroupAddSurfaceCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

    SurfaceGroup* surfacegroup = scene.getSurfaceGroup(m_surfacegroupid);
    Surface* surface = scene.getSurface(m_surfaceid);

    if (surfacegroup && surface)
    {
        // check if already a member of the group
        const SurfaceList list = surfacegroup->getList();
        SurfaceListConstIterator iter = std::find (list.begin(), list.end(), surface);
        SurfaceListConstIterator iterEnd = list.end();

        if (iter == iterEnd)
        {
            surfacegroup->addElement(surface);
            result = ExecutionSuccess;
        }
        else
        {
            LOG_ERROR("SurfacegroupAddSurfaceCommand", "Surface is already a member of the group");
        }
    }

    return result;
}

const std::string SurfacegroupAddSurfaceCommand::getString()
{
    std::stringstream description;
    description << "SurfacegroupAddSurfaceCommand("
                << "surfacegroupid=" << m_surfacegroupid
                << ", surfaceid=" << m_surfaceid
                << ")";
    return description.str();
}
