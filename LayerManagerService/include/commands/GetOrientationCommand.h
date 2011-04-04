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

#ifndef _GETORIENTATIONCOMMAND_H_
#define _GETORIENTATIONCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"
#include "Orientation.h"

class GetOrientationCommand : public Command{
public:
	GetOrientationCommand(int id, ObjectType type, OrientationType* orientation) : Command(GetOrientation), id(id), type(type), returnOrientation(orientation){};
	const unsigned  id;
	const ObjectType type;
	OrientationType* returnOrientation;

	virtual bool execute(LayerList& layerlist){
			switch(type){
				case TypeSurface: {
					Surface* s = layerlist.getSurface(id);
					if (s==NULL) return false;
					*returnOrientation = s->getOrientation();
					break;
					}
				case TypeLayer: {
					Layer*l =layerlist.getLayer(id);
					if (l==NULL) return false;
					*returnOrientation = l->getOrientation();
					break;
					}
				case TypeSurfaceGroup: {return false;}
				case TypeLayerGroup: {return false;}
                                default : { return false; }
			}
			return true;

		}
};


#endif /* _GETORIENTATIONCOMMAND_H_ */
