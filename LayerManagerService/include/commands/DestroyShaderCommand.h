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

#ifndef _DESTROYSHADERCOMMAND_H_
#define _DESTROYSHADERCOMMAND_H_

#include <commands/Command.h>
#include "LogicalGraphicsObject.h"

class DestroyShaderCommand : public Command
{
public:
	DestroyShaderCommand(int shaderid)
		: Command(DestroyShader)
		, _id(shaderid)
	{
		// void
	};

	void execute(LayerList& layerlist){
		std::map<int,Shader*>::iterator it;

		// get shader by its ID
		it = layerlist.shaderList.find(_id);
		if (it!=layerlist.shaderList.end())
		{
			Shader* shader = (*it).second;
			if (shader)
			{
				// detach shader from all surfaces, surface groups, etc...
				// from surfaces
				for (std::map<int,Surface*>::const_iterator surfit = layerlist.getAllSurfaces().begin();surfit!=layerlist.getAllSurfaces().end();surfit++)
				{
					GraphicalObject* obj = surfit->second;
					if (obj && obj->getShader()==shader)
					{
						obj->setShader(NULL);
					}
				}
				// from layers
				for (std::map<int,Layer*>::const_iterator layerit = layerlist.getAllLayers().begin();layerit!=layerlist.getAllLayers().end();layerit++)
				{
					GraphicalObject* obj = layerit->second;
					if (obj && obj->getShader()==shader)
					{
						obj->setShader(NULL);
					}
				}

				// remove shader from global shader list
				layerlist.shaderList.erase(it);

				// finally delete it
				delete shader;
			}
		}
		else
		{
			// shader not found
			LOG_ERROR("Layermanager", "shader ID "<<_id<<" not found");
		}
	}

private:
	const int _id;
};

#endif /* _DESTROYSHADERCOMMAND_H_ */
