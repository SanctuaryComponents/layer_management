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

#ifndef _SETDIMENSIONCOMMAND_H_
#define _SETDIMENSIONCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"
#include <vector>

class SetDimensionCommand : public Command{
public:
	SetDimensionCommand(int id, ObjectType typeToSetOn, int width, int height) : Command(SetDimension),  id(id), typeToSet(typeToSetOn), width(width),height(height){};
	const int id;
	const ObjectType typeToSet;
	const int width;
	const int height;

	virtual void execute(LayerList& layerlist){
		GraphicalSurface* go = NULL;
		switch(typeToSet){
			case TypeSurface: {go = layerlist.getSurface(id); break;}
			case TypeLayer: {go = layerlist.getLayer(id); break;}
			case TypeSurfaceGroup: {break;}
			case TypeLayerGroup: { break;}
                        default : { break; }
		}
		if ( NULL != go )
		{
			go->setDimension(width,height);
		}
	}
};


#endif /* _SETDIMENSIONCOMMAND_H_ */
