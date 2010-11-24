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

#ifndef _GETPIXELFORMATCOMMAND_H_
#define _GETPIXELFORMATCOMMAND_H_

#include "Command.h"
#include "ObjectType.h"
#include "PixelFormat.h"

class GetPixelformatCommand : public Command{
public:
	GetPixelformatCommand(int id, ObjectType type, PixelFormat* f) : Command(GetPixelformat), id(id), type(type), formatreturn(f){};
	const int id;
	const ObjectType type;
	PixelFormat* formatreturn;
	virtual void execute(LayerList& layerlist){
		switch(type){
			case TypeSurface: {
				Surface* s = layerlist.getSurface(id);
				if ( s==NULL ) break;
				*formatreturn = s->getPixelFormat();
				break;
				}
			case TypeLayer:{
				//Layer* l = layerlist.getLayer(id);
				//*formatreturn = l->getPixelFormat();
				break;
				}
			case TypeSurfaceGroup: break;
			case TypeLayerGroup: break;
                        default : { break; }
		}
	}
};

#endif /* _GETPIXELFORMATCOMMAND_H_ */
