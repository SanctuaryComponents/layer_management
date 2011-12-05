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

#ifndef _LAYERSETPOSITIONCOMMAND_H_
#define _LAYERSETPOSITIONCOMMAND_H_

#include "BaseCommandAsynchronous.h"

class LayerSetPositionCommand : public BaseCommandAsynchronous
{
public:
    LayerSetPositionCommand(unsigned int id, unsigned int x, unsigned int y);
    virtual ~LayerSetPositionCommand() {}

    virtual ExecutionResult execute(ICommandExecutor* executor);
    virtual const std::string getString();

private:
    const unsigned int m_id;
    const unsigned int m_x;
    const unsigned int m_y;

    // for unit testing
    template <typename id_type, typename x_type, typename y_type> friend class LayerSetPositionCommandEqMatcherP3;
};


#endif /* _LAYERSETPOSITIONCOMMAND_H_ */
