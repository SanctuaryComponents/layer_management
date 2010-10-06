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

#ifndef _SETLAYERORDERCOMMAND_H_
#define _SETLAYERORDERCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class SetLayerOrderCommand : public Command{
public:
	SetLayerOrderCommand(int* array, int length) : Command(SetLayerRenderOrder), array(array), length(length){};
	int* array;
	const int length;

	virtual void execute(LayerList& layerlist){
		layerlist.getCurrentRenderOrder().clear();
		for (int i=0;i<length;i++){
			Layer* l = layerlist.getLayer(array[i]);
			layerlist.getCurrentRenderOrder().push_back(l);
		}
	};
};


#endif /* _SETLAYERORDERCOMMAND_H_ */
