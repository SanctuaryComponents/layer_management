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

#ifndef _SETLAYERORDERCOMMAND_H_
#define _SETLAYERORDERCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class SetLayerOrderCommand : public Command{
public:
	SetLayerOrderCommand(unsigned int* array, unsigned int length) : Command(SetLayerRenderOrder), array(array), length(length){};
	unsigned int* array;
	const unsigned int length;

	virtual bool execute(LayerList& layerlist){
		// check for doubles
		for (int i=0;i<length;i++)
			for (int c=0;c<length;c++)
				if (array[i] == array[c] && i!=c) // doubles not allowed here
					return false;

// taken out because other software currently expects partial execution of command,
// i.e. the layers that exist are added to the render order

//		// check that all layers to be added exist
//		for (unsigned int i=0;i<length;i++){
//			Layer* l = layerlist.getLayer(array[i]);
//			if (NULL==l)
//				return false;
//		} // TODO insert again later

		layerlist.getCurrentRenderOrder().clear();
		LOG_DEBUG("SetLayerOrderCommand", "Length to set: " << length);
		bool status = true;
		for (unsigned int i=0;i<length;i++){
			LOG_DEBUG("SetLayerOrderCommand", "Trying to add layer: " << array[i] << " to current render order");
			Layer* l = layerlist.getLayer(array[i]);
			if ( l != NULL )
			{
				LOG_DEBUG("SetLayerOrderCommand", "Adding Layer: " << array[i] << " to current render order");
				layerlist.getCurrentRenderOrder().push_back(l);
			}else{
				status = false;
			}
		}
		return status;
	};
};


#endif /* _SETLAYERORDERCOMMAND_H_ */
