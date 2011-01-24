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

#ifndef _SETUNIFORMSCOMMAND_H_
#define _SETUNIFORMSCOMMAND_H_

#include <commands/Command.h>
#include <string>
#include <vector>

class SetUniformsCommand : public Command
{
public:
	SetUniformsCommand(int shaderid, const std::vector<std::string>& uniforms)
		: Command(SetUniforms)
		, _shaderid(shaderid)
		, _uniforms(uniforms)
	{
		// void
	};

	const std::vector<std::string> getUniforms(){
		return _uniforms;
	}
	const int getShaderId(){ return _shaderid; }

	void execute(LayerList& layerlist){
		// get shader by its ID
		std::map<int,Shader*>::iterator siterator;
		siterator = layerlist.shaderList.find(_shaderid);

		if (siterator!=layerlist.shaderList.end())
		{
			Shader* shader = (*siterator).second;

			// get uniform descriptions
			const std::vector<std::string> uniformDesc = _uniforms;
			std::vector<std::string>::const_iterator uiterator;

			for (uiterator=uniformDesc.begin(); uiterator!=uniformDesc.end(); uiterator++)
			{
				// parse description string and create uniform object
				ShaderUniform* uniform = ShaderUniform::createFromStringDescription(*uiterator);
				if (uniform)
				{
					shader->setUniform(*uniform);
					delete uniform;
				}
				else
				{
					LOG_ERROR("Layermanager", "Failed to parse uniform description");
				}
			}
		}
		else
		{
			// shader not found
			LOG_ERROR("Layermanager", "shader ID "<<_shaderid<<" not found");
		}
	}
private:
	const int _shaderid;
	const std::vector<std::string> _uniforms;
};

#endif /* _SETUNIFORMSCOMMAND_H_ */


