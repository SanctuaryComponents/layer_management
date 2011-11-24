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
#ifndef _SHADERSETUNIFORMSCOMMAND_H_
#define _SHADERSETUNIFORMSCOMMAND_H_

#include "BaseCommandSynchronous.h"
#include <string>
#include <vector>

class ShaderSetUniformsCommand : public BaseCommandSynchronous
{
public:
	ShaderSetUniformsCommand(unsigned int shaderid, const std::vector<std::string>& uniforms);

    virtual ExecutionResult execute(ICommandExecutor* executor);
    virtual const std::string getString();

    const std::vector<std::string> getUniforms() const;
    unsigned int getShaderId() const;

private:
    const unsigned int m_shaderid;
    const std::vector<std::string> m_uniforms;
};

#endif /* _SHADERSETUNIFORMSCOMMAND_H_ */


