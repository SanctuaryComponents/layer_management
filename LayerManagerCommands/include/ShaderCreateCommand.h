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

#ifndef _SHADERCREATECOMMAND_H_
#define _SHADERCREATECOMMAND_H_

#include "BaseCommandSynchronous.h"
#include "Shader.h"
#include <string>

class ShaderCreateCommand : public BaseCommandSynchronous
{
public:
    ShaderCreateCommand(const std::string& vertName, const std::string& fragName, unsigned int* id);
    virtual ~ShaderCreateCommand() {}

    virtual ExecutionResult execute(ICommandExecutor* executor);
    virtual const std::string getString();

    const std::string getVertName() const;
    const std::string getFragName() const;

private:
    const std::string m_vertName;
    const std::string m_fragName;
    unsigned int* m_returnID;
};

#endif /* _SHADERCREATECOMMAND_H_ */
