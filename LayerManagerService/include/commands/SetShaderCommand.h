/***************************************************************************
*
* Copyright 2010,2011 BMW Car IT GmbH
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

#ifndef _SETSHADERCOMMAND_H_
#define _SETSHADERCOMMAND_H_

#include <commands/Command.h>

class SetShaderCommand : public Command
{
public:
	SetShaderCommand(unsigned int id,unsigned  int shaderid)
		: Command(SetShader)
		, _id(id)
		, _shaderid(shaderid)
	{
		// void
	};

	const unsigned int getID(){ return _id; }
	const unsigned int getShaderID(){ return _shaderid; }

	bool execute(LayerList& layerlist){
		std::map<unsigned int,Shader*>::iterator it;
		GraphicalObject* object = layerlist.getSurface(_id);
		Shader* shader = NULL;

		if (object)
		{
			// get shader by its ID
			it = layerlist.shaderList.find(_shaderid);
			if (it!=layerlist.shaderList.end())
			{
				shader = (*it).second;
			}

			if (shader!=NULL || _shaderid==0)
			{
				// shaderId==0 detaches the custom shader
				// (shader is supposed to be NULL in this case)

				object->setShader(shader);
			}
			else
			{
				// shader not found
				LOG_ERROR("Layermanager", "shader ID "<<_shaderid<<" not found");
			}
		}
		else
		{
			// object not found
			LOG_ERROR("Layermanager", "object ID "<<_id<<" not found");
			return false;
		}
		return true;
	}

private:
	const unsigned int _id;
	const unsigned int _shaderid;
};

#endif /* _SETSHADERCOMMAND_H_ */

