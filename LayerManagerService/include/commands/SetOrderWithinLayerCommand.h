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

#ifndef _SETORDERWITHINLAYERCOMMAND_H_
#define _SETORDERWITHINLAYERCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class SetOrderWithinLayerCommand : public Command{
public:
	SetOrderWithinLayerCommand(unsigned int layerid,unsigned  int* array,unsigned  int length) : Command(SetSurfaceRenderOrderWithinLayer), layerid(layerid), array(array), length(length){};
	const unsigned int layerid;
	unsigned int* array;
	unsigned int length;

	virtual bool execute(LayerList& layerlist){
		LOG_DEBUG("SetOrderWithinLayerCommand","trying to add surfaces to layer" << layerid);
		Layer* l = layerlist.getLayer(layerid);
		if (l != NULL)
		{
			l->getAllSurfaces().clear();
			for (unsigned int i=0;i<length;i++)
			{
				LOG_DEBUG("SetOrderWithinLayerCommand","trying to add surfaces " << array[i]);
				Surface* s = layerlist.getSurface(array[i]);
				if ( s != NULL )
				{
					l->getAllSurfaces().push_back(s);
					LOG_DEBUG("SetOrderWithinLayerCommand","add surface " << s->getID() << " to renderorder of layer "<< layerid);
				}
			}
		}else{
			return false;
		}
		return true;
		delete[] array;
	};
};


#endif /* _SETORDERWITHINLAYERCOMMAND_H_ */
