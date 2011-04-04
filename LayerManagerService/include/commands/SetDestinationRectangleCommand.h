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

#ifndef _SETDESTINATIONRECTANGLECOMMAND_H_
#define _SETDESTINATIONRECTANGLECOMMAND_H_

#include "Command.h"
#include "ObjectType.h"
#include <vector>

class SetDestinationRectangleCommand : public Command{
public:
	SetDestinationRectangleCommand(int id, ObjectType typeToSetOn,unsigned int x,unsigned int y,unsigned int width,unsigned int height) : Command(SetDestinationRectangle),  id(id), typeToSet(typeToSetOn), x(x),y(y),width(width),height(height){};
	const unsigned int id;
	const ObjectType typeToSet;
	const unsigned int x;
	const unsigned int y;
	const unsigned int width;
	const unsigned int height;

	virtual bool execute(LayerList& layerlist){
		GraphicalSurface* go = NULL;
		switch(typeToSet){
			case TypeSurface: {go = layerlist.getSurface(id); break;}
			case TypeLayer: {go = layerlist.getLayer(id); break;}
			case TypeSurfaceGroup: {break;}
			case TypeLayerGroup: { break;}
                        default : { break; }
		}
		const Rectangle r(x,y,width,height);
		if (NULL != go )
		{
			go->setDestinationRegion(r);
		}else{
			return false;
		}
		return true;
	}
};


#endif /* _SETDESTINATIONRECTANGLECOMMAND_H_ */
