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

#ifndef _LAYERGROUPADDLAYERCOMMAND_H_
#define _LAYERGROUPADDLAYERCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"
#include <algorithm>

class LayergroupAddLayerCommand : public Command{
public:
	LayergroupAddLayerCommand(unsigned int layergroupid,unsigned int layerid) : Command(LayergroupAddLayer), layergroupid(layergroupid), layerid(layerid){};
	const unsigned  layergroupid;
	const unsigned  layerid;

	virtual bool execute(LayerList& layerlist){
		LayerGroup* lg = layerlist.getLayerGroup(layergroupid);
		Layer* layer = layerlist.getLayer(layerid);
		if ( lg != NULL && layer != NULL )
		{
			// check if already a member of the group
			const std::list<Layer*> list = lg->getList();
			std::list<Layer*>::const_iterator it;
			it = std::find (list.begin(), list.end(), layer);
			if (it!=list.end()){
				LOG_ERROR("LayergroupAddLayerCommand","layer is already a member of the group");
				return false; // already is member of the group
			}

			lg->addElement(layer);
			LOG_DEBUG("LayergroupAddLayerCommand","adding layer " << layerid << " to group " << layergroupid);
			LOG_DEBUG("LayergroupAddLayerCommand","now there are " << lg->getList().size() << " elements");
		}else{
			if (lg==NULL){
				LOG_DEBUG("LayergroupAddLayerCommand","Layergroup with id " << layergroupid << " not found.");
				return false;
			}
			if (layer==NULL){
				LOG_DEBUG("LayergroupAddLayerCommand","Layer with id " << layerid << " not found.");
				return false;
			}
		}
		return true;
	};
};


#endif /* _LAYERGROUPADDLAYERCOMMAND_H_ */
