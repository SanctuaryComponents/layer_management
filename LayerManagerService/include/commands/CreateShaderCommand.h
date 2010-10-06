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

#ifndef _CREATESHADERCOMMAND_H_
#define _CREATESHADERCOMMAND_H_

#include <commands/Command.h>
#include <string>
#include "Shader.h"

class CreateShaderCommand : public Command
{
public:
	CreateShaderCommand(const std::string& vertName, const std::string& fragName, uint* id)
		: Command(CreateShader)
		, _vertName(vertName)
		, _fragName(fragName)
		, returnID(id)
	{
		// void
	};

	void execute(LayerList& layerlist){
		Shader* shader;
			int id=0;

			// create new shader instance
			shader = Shader::createShader(_vertName, _fragName);

			if (shader)
			{
				// insert shader to shader map
				id = shader->getId();
				layerlist.shaderList.insert(std::pair<int,Shader*>(id, shader));
			}

			*returnID = id;
	}
private:
	const std::string _vertName;
	const std::string _fragName;
	uint* returnID;
};

#endif /* _CREATESHADERCOMMAND_H_ */

