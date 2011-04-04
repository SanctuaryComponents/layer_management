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

#ifndef _SETOPACITYCOMMAND_H_
#define _SETOPACITYCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class SetOpacityCommand : public Command{
public:
	SetOpacityCommand(unsigned int id, ObjectType type, double Opacity) : Command(SetOpacity), id(id), type(type), Opacity(Opacity){};
	const unsigned int id;
	const ObjectType type;
	const double Opacity;
	virtual bool execute(LayerList& layerlist){
		GraphicalObject* gs;
			switch(type){
				case TypeSurface: gs = layerlist.getSurface(id); break;
				case TypeLayer:gs = layerlist.getLayer(id); break;
				case TypeSurfaceGroup: gs = layerlist.getSurfaceGroup(id); break;
				case TypeLayerGroup: gs = layerlist.getLayerGroup(id); break;
                                default : { break; }
			}
			if ( gs != NULL )
			{
				LOG_DEBUG("SetOpacityCommand","new opacity " << Opacity << " for id: " <<id);
				gs->setOpacity(Opacity);
			}else{
				return false;
			}
			return true;
		}
};


#endif /* _SETOPACITYCOMMAND_H_ */
