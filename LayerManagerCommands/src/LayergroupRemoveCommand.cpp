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
#include "LayergroupRemoveCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"
#include <sstream>

LayergroupRemoveCommand::LayergroupRemoveCommand(unsigned int objectID)
: m_idToRemove(objectID)
{
}

ExecutionResult LayergroupRemoveCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

	LayerGroup* layergroup = scene.getLayerGroup(m_idToRemove);
	if (layergroup)
	{
		scene.removeLayerGroup(layergroup);
		result = ExecutionSuccess;
	}

    return result;
}

const std::string LayergroupRemoveCommand::getString()
{
    std::stringstream description;
    description << "LayergroupRemoveCommand("
                << "idToRemove=" << m_idToRemove
                << ")";
    return description.str();
}
