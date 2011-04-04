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

#ifndef _SETVISIBILITYCOMMAND_H_
#define _SETVISIBILITYCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class SetVisibilityCommand : public Command{
public:
	SetVisibilityCommand(const uint givenid, ObjectType Objecttype, bool newvisibility) : Command(SetVisibility), idtoSet(givenid), otype(Objecttype), visibility(newvisibility){};
	const uint idtoSet;
	const ObjectType otype;
	const bool visibility;

	virtual bool execute(LayerList& layerlist){
		GraphicalObject* go = NULL;
		switch(otype){
			case TypeSurface: go = layerlist.getSurface(idtoSet);
							break;
			case TypeLayer: go = layerlist.getLayer(idtoSet);
							break;
			case TypeSurfaceGroup: go = layerlist.getSurfaceGroup(idtoSet);
									break;
			case TypeLayerGroup: go = layerlist.getLayerGroup(idtoSet);
									break;
                        default : { break; }
		}
		if ( go != NULL )
		{
			LOG_DEBUG("SetVisibilityCommand", "setting visibility: " << visibility << " of id " << idtoSet);
			go->setVisibility(visibility);
		}else{
			return false;
		}
		return true;
	}
};


#endif /* _SETVISIBILITYCOMMAND_H_ */
