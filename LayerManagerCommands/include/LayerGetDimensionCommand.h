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

#ifndef _LAYERGETDIMENSIONCOMMAND_H_
#define _LAYERGETDIMENSIONCOMMAND_H_

#include "BaseCommandSynchronous.h"

class LayerGetDimensionCommand: public BaseCommandSynchronous
{
public:
    LayerGetDimensionCommand(int id, unsigned int* widthRet, unsigned int* heightRet);
    virtual ~LayerGetDimensionCommand() {}

    virtual ExecutionResult execute(ICommandExecutor* executor);
    virtual const std::string getString();

private:
    const unsigned m_id;
    unsigned int* m_pWidth;
    unsigned int* m_pHeight;

    // for unit testing
    template <typename id_type> friend class LayerGetDimensionCommandEqMatcherP;
};

#endif /* _LAYERGETDIMENSIONCOMMAND_H_ */
