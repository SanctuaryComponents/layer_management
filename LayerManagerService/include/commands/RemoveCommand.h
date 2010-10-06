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

#ifndef _REMOVECOMMAND_H_
#define _REMOVECOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class RemoveCommand : public Command{
public:
	RemoveCommand(int objectID, ObjectType typeToRemove) : Command(Remove), idToRemove(objectID), typeToRemove(typeToRemove){};
	const int idToRemove;
	const ObjectType typeToRemove;

	virtual void execute(LayerList& layerlist){
			switch(typeToRemove){
				case TypeSurface: {Surface* s = layerlist.getSurface(idToRemove);
									 layerlist.removeSurface(s);
								 break;}
				case TypeLayer: {Layer* l = layerlist.getLayer(idToRemove);
									layerlist.removeLayer(l);
								break;}
				case TypeSurfaceGroup: {SurfaceGroup* sg = layerlist.getSurfaceGroup(idToRemove);
									layerlist.removeSurfaceGroup(sg);
								break;}
				case TypeLayerGroup: {LayerGroup* lg = layerlist.getLayerGroup(idToRemove);
									layerlist.removeLayerGroup(lg);
								break;}
			}
		};
};


#endif /* _REMOVECOMMAND_H_ */
