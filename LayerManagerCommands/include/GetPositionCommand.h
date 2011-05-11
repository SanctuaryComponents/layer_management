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

#ifndef _GETPOSITIONCOMMAND_H_
#define _GETPOSITIONCOMMAND_H_

#include "BaseCommandSynchronous.h"
#include "ObjectType.h"

class GetPositionCommand : public BaseCommandSynchronous
{
public:
    GetPositionCommand(int id, ObjectType type, unsigned int* xRet, unsigned int* yRet);

    virtual ExecutionResult execute(ICommandExecutor* executor);
    virtual const std::string getString();

private:
    const unsigned m_id;
    const ObjectType m_type;
    unsigned int* m_pX;
    unsigned int* m_pY;
};


#endif /* _GETPOSITIONCOMMAND_H_ */