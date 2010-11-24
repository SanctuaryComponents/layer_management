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

#ifndef _GETDIMENSIONCOMMAND_H_
#define _GETDIMENSIONCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class GetDimensionCommand : public Command{
public:
	GetDimensionCommand(int id, ObjectType type, uint* widthRet, uint* heightRet) : Command(GetDimension), id(id), type(type), width(widthRet), height(heightRet){};
	const int id;
	const ObjectType type;
	uint* width;
	uint* height;
	virtual void execute(LayerList& layerlist){
			GraphicalSurface* go = NULL;
			switch(type){
				case TypeSurface: {go = layerlist.getSurface(id); break;}
				case TypeLayer: {go = layerlist.getLayer(id); break;}
				case TypeSurfaceGroup: {break;}
				case TypeLayerGroup: { break;}
                                default : { break; }

			}
			if ( NULL != go )
			{
				Vector2 dim = go->getDimension();
				*width = dim.val1;
				*height = dim.val2;
			} else {
				*width = 0;
				*height = 0;
			}
		}
};


#endif /* _GETDIMENSIONCOMMAND_H_ */
