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

#ifndef _GETPOSITIONCOMMAND_H_
#define _GETPOSITIONCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class GetPositionCommand : public Command{
public:
	GetPositionCommand(int id, ObjectType type, uint* xRet, uint* yRet) : Command(GetPosition), id(id), type(type), x(xRet),y(yRet){};
	const int id;
	const ObjectType type;
	uint* x;
	uint* y;
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
			Vector2 dim = go->getPosition();
			*x = dim.val1;
			*y = dim.val2;
		} else {
			*x = 0;
			*y = 0;
		}
	}
};


#endif /* _GETPOSITIONCOMMAND_H_ */
