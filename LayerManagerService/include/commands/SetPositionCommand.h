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

#ifndef _SETPOSITIONCOMMAND_H_
#define _SETPOSITIONCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"
#include <vector>

class SetPositionCommand : public Command{
public:
	SetPositionCommand(int id, ObjectType typeToSetOn, int x, int y ) : Command(SetPosition),  id(id), typeToSet(typeToSetOn), x(x),y(y){};
	const int id;
	const ObjectType typeToSet;
	const int x;
	const int y;

	virtual void execute(LayerList& layerlist){
		GraphicalSurface* go = NULL;
		switch(typeToSet){
			case TypeSurface: {go = layerlist.getSurface(id); break;}
			case TypeLayer: {go = layerlist.getLayer(id); break;}
			case TypeSurfaceGroup: {break;}
			case TypeLayerGroup: { break;}
		}
		if (NULL != go)
		{
			go->setPosition(x,y);
		}
	}
};


#endif /* _SETPOSITIONCOMMAND_H_ */
