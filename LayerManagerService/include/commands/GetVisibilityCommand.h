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

#ifndef _GETVISIBILITYCOMMAND_H_
#define _GETVISIBILITYCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class GetVisibilityCommand : public Command{
public:
	GetVisibilityCommand(int id, ObjectType type, bool* visibility) : Command(GetVisibility), id(id), type(type), visibility(visibility){};
	const unsigned  id;
	const ObjectType type;
	bool * visibility;
	bool execute(LayerList& layerlist){
		LOG_DEBUG("GetVisibilityCommand", "start of");
			GraphicalObject* go;
			switch(type){
				case TypeSurface: go = layerlist.getSurface(id);
								break;
				case TypeLayer: go = layerlist.getLayer(id);
								break;
				case TypeSurfaceGroup: go = layerlist.getSurfaceGroup(id);
										break;
				case TypeLayerGroup: go = layerlist.getLayerGroup(id);
										break;
                                default : { break; }
			}
			if (go != NULL)
			{
				bool temp = go->getVisibility();
				LOG_DEBUG("GetVisibilityCommand", "returning:" << temp);
				*visibility = temp;
			}else{
				LOG_DEBUG("GetVisibilityCommand", "id not found, return false");
				return false;
			}
			LOG_DEBUG("GetVisibilityCommand", "end of");
			return true;
		}
};


#endif /* _GETVISIBILITYCOMMAND_H_ */
