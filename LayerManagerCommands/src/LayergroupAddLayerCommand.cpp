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
#include "LayergroupAddLayerCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"
#include <algorithm>

ExecutionResult LayergroupAddLayerCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());

    LayerGroup* lg = scene.getLayerGroup(m_layergroupid);
    Layer* layer = scene.getLayer(m_layerid);

    if (lg && layer)
    {
        // check if already a member of the group
        const LayerList list = lg->getList();

        LayerListConstIterator iter = std::find(list.begin(), list.end(), layer);
        LayerListConstIterator iterEnd = list.end();

        if (iter != iterEnd)
        {
            LOG_ERROR("LayergroupAddLayerCommand","layer is already a member of the group");
            return ExecutionFailed; // already is member of the group
        }

        lg->addElement(layer);
        LOG_DEBUG("LayergroupAddLayerCommand","adding layer " << m_layerid << " to group " << m_layergroupid);
        LOG_DEBUG("LayergroupAddLayerCommand","now there are " << lg->getList().size() << " elements");
    }
    else
    {
        if (!lg)
        {
            LOG_DEBUG("LayergroupAddLayerCommand","Layergroup with id " << m_layergroupid << " not found.");
            return ExecutionFailed;
        }
        if (!layer)
        {
            LOG_DEBUG("LayergroupAddLayerCommand","Layer with id " << m_layerid << " not found.");
            return ExecutionFailed;
        }
    }
    return ExecutionSuccess;
}

const std::string LayergroupAddLayerCommand::getString()
{
    std::stringstream description;
    description << "LayergroupAddLayerCommand("
                << "layergroupid=" << m_layergroupid
                << ", layerid=" << m_layerid
                << ")";
    return description.str();
}
