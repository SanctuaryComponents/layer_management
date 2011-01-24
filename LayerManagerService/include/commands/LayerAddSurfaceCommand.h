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

#ifndef _LAYERADDSURFACECOMMAND_H_
#define _LAYERADDSURFACECOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class LayerAddSurfaceCommand : public Command{
public:
	LayerAddSurfaceCommand(int layerid, int surfaceid) : Command(LayerAddSurface), layerid(layerid), surfaceid(surfaceid){};
	const int layerid;
	const int surfaceid;

	virtual void execute(LayerList& layerlist){
		Layer* l = layerlist.getLayer(layerid);
		Surface* s = layerlist.getSurface(surfaceid);
		if (l != NULL && s != NULL )
		{
			LOG_DEBUG("LayerAddSurfaceCommand","add surface " << surfaceid << " to layer "<< layerid);
			l->addSurface(s);
		}
	};
};


#endif /* _LAYERADDSURFACECOMMAND_H_ */
