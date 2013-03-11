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
#include "LayerSetPositionCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

ExecutionResult LayerSetPositionCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    ExecutionResult result = ExecutionFailed;

    Layer* layer = scene.getLayer(m_id);

    if (layer)
    {
        if (layer->setPosition(m_x, m_y))
        {
            result = ExecutionSuccessRedraw;
            executor->addClientNotification(layer, ILM_NOTIFICATION_DEST_RECT);
        }
        else
        {
            result = ExecutionSuccess;
        }
        layer->applySurfaceTransform();
    }
    return result;
}

const std::string LayerSetPositionCommand::getString()
{
    std::stringstream description;
    description << "LayerSetPositionCommand("
                << "id=" << m_id << "(0x" << std::hex << m_id << ")" << std::dec
                << ", m_x=" << m_x
                << ", m_y=" << m_y
                << ")";
    return description.str();
}
