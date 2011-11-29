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
#include "LayergroupCreateCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

LayergroupCreateCommand::LayergroupCreateCommand(uint* idReturn)
: m_idReturn(idReturn)
{
}

ExecutionResult LayergroupCreateCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());
    ExecutionResult result = ExecutionFailed;

    LayerGroup *layergroup = scene.createLayerGroup(*m_idReturn);

    if (layergroup)
    {
        *m_idReturn = layergroup->getID();

        LOG_DEBUG("CreateCommand", "created layergroup with id: " << layergroup->getID());
        result = ExecutionSuccess;
    }

    return result;
}

const std::string LayergroupCreateCommand::getString()
{
    std::stringstream description;
    description << "LayergroupCreateCommand("
                << "idReturn=" << m_idReturn << "=" << *m_idReturn
                << ")";
    return description.str();
}
