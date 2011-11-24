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

#ifndef _LAYERCREATECOMMAND_H_
#define _LAYERCREATECOMMAND_H_

#include "BaseCommandSynchronous.h"
#include "IScene.h"

class LayerCreateCommand : public BaseCommandSynchronous
{
public:
	LayerCreateCommand(uint OriginalWidth, uint OriginalHeight, uint* idReturn);
    virtual ExecutionResult execute(ICommandExecutor* executor);
    virtual const std::string getString();

private:
    uint m_originalWidth;
    uint m_originalHeight;
    uint* m_idReturn;

    // for unit testing
    //template <typename nativeHandle_type, typename OriginalWidth_type, typename OriginalHeight_type, typename createType_type, typename pixelformat_type> friend class CreateCommandEqMatcherP5;
};


#endif /* _LAYERCREATECOMMAND_H_ */
