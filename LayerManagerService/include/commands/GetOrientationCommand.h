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

#ifndef _GETORIENTATIONCOMMAND_H_
#define _GETORIENTATIONCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"
#include "Orientation.h"

class GetOrientationCommand : public Command{
public:
	GetOrientationCommand(int id, ObjectType type, OrientationType* orientation) : Command(GetOrientation), id(id), type(type), returnOrientation(orientation){};
	const int id;
	const ObjectType type;
	OrientationType* returnOrientation;

	virtual void execute(LayerList& layerlist){
			switch(type){
				case TypeSurface: {
					Surface* s = layerlist.getSurface(id);
					if (s==NULL) break;
					*returnOrientation = s->getOrientation();
					break;
					}
				case TypeLayer: {
					Layer*l =layerlist.getLayer(id);
					if (l==NULL) break;
					*returnOrientation = l->getOrientation();
					break;
					}
				case TypeSurfaceGroup: {break;}
				case TypeLayerGroup: {break;}
                                default : { break; }
			}

		}
};


#endif /* _GETORIENTATIONCOMMAND_H_ */
