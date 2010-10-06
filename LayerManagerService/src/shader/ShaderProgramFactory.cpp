/***************************************************************************
*
* Copyright 2010 BMW Car IT GmbH
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*		http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
****************************************************************************/

#include <ShaderProgramFactory.h>

// pointer to shader program creator function
PfnShaderProgramCreator ShaderProgramFactory::_creator = 0;

ShaderProgram* ShaderProgramFactory::createProgram(const std::string& vertName, const std::string& fragName)
{
	if (_creator!=0)
		return _creator(vertName, fragName);
	else
		return 0;
}

void ShaderProgramFactory::setCreatorFunc(PfnShaderProgramCreator creatorFunc)
{
	_creator = creatorFunc;
}

