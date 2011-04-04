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

#ifndef _GETOPACITYCOMMAND_H_
#define _GETOPACITYCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class GetOpacityCommand : public Command{
public:
	GetOpacityCommand(int id, ObjectType type, double* returnOpacity) : Command(GetOpacity), id(id), type(type), returnOpacity(returnOpacity){};
	const unsigned  id;
	const ObjectType type;
	double* returnOpacity;

	virtual bool execute(LayerList& layerlist){
		GraphicalObject* go;
		switch(type){
			case TypeSurface: {go = layerlist.getSurface(id); break;}
			case TypeLayer: {go = layerlist.getLayer(id); break;}
			case TypeSurfaceGroup: {go = layerlist.getSurfaceGroup(id); break;}
			case TypeLayerGroup: { go = layerlist.getLayerGroup(id); break;}
                        default : { break; }
		}
		if ( go!= NULL )
		{
			*returnOpacity = go->getOpacity();
		}else{
			return false;
		}
		return true;
	}
};


#endif /* _GETOPACITYCOMMAND_H_ */
