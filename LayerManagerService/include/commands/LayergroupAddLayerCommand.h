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

#ifndef _LAYERGROUPADDLAYERCOMMAND_H_
#define _LAYERGROUPADDLAYERCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class LayergroupAddLayerCommand : public Command{
public:
	LayergroupAddLayerCommand(int layergroupid, int layerid) : Command(LayergroupAddLayer), layergroupid(layergroupid), layerid(layerid){};
	const int layergroupid;
	const int layerid;

	virtual void execute(LayerList& layerlist){
		LayerGroup* lg = layerlist.getLayerGroup(layergroupid);
		Layer* layer = layerlist.getLayer(layerid);
		if ( lg != NULL && layer != NULL )
		{
			lg->addElement(layer);
		}
	};
};


#endif /* _LAYERGROUPADDLAYERCOMMAND_H_ */
