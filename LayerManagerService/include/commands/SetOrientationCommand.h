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

#ifndef _SETORIENTATIONCOMMAND_H_
#define _SETORIENTATIONCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"
#include "Orientation.h"

class SetOrientationCommand : public Command{
public:
	SetOrientationCommand(int id, ObjectType type, OrientationType Orientation) : Command(SetOrientation), id(id), type(type), Orientation(Orientation){};
	const int id;
	const ObjectType type;
	const OrientationType Orientation;

	virtual void execute(LayerList& layerlist){
		GraphicalSurface* gs = NULL;
		switch(type){
			case TypeSurface: {gs = layerlist.getSurface(id); break;}
			case TypeLayer:{gs = layerlist.getLayer(id); break;}
			case TypeSurfaceGroup:{ break;}
			case TypeLayerGroup:{break;}
		}
		if (NULL != gs)
			gs->setOrientation(Orientation);
	}
};


#endif /* _SETORIENTATIONCOMMAND_H_ */
