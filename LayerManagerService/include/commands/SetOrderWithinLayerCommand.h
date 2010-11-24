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

#ifndef _SETORDERWITHINLAYERCOMMAND_H_
#define _SETORDERWITHINLAYERCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class SetOrderWithinLayerCommand : public Command{
public:
	SetOrderWithinLayerCommand(int layerid, int* array, int length) : Command(SetSurfaceRenderOrderWithinLayer), layerid(layerid), array(array), length(length){};
	const int layerid;
	int* array;
	int length;

	virtual void execute(LayerList& layerlist){
		Layer* l = layerlist.getLayer(layerid);
		if (l != NULL)
		{
			l->getAllSurfaces().clear();
			for (int i=0;i<length;i++)
			{
				Surface* s = layerlist.getSurface(array[i]);
				if ( s != NULL )
				{
					l->getAllSurfaces().push_back(s);
				}
			}
		}
	};
};


#endif /* _SETORDERWITHINLAYERCOMMAND_H_ */
