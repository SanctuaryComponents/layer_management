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

#ifndef _CREATECOMMAND_H_
#define _CREATECOMMAND_H_

#include "Command.h"
#include "ObjectType.h"
#include "Log.h"

class CreateCommand : public Command{
public:
	CreateCommand(int handle, ObjectType createType, PixelFormat pixelformat,uint OriginalWidth,uint OriginalHeight,int* idReturn) : Command(Create), createType(createType), nativeHandle(handle), pixelformat(pixelformat),OriginalWidth(OriginalWidth),OriginalHeight(OriginalHeight), idReturn(idReturn){};
	const ObjectType createType;
	const int nativeHandle;
	const PixelFormat pixelformat;
	unsigned int OriginalWidth;
	unsigned int OriginalHeight;
	int* idReturn;
	virtual void execute(LayerList& layerlist){
		switch(createType){
			case TypeSurface: {
							 Surface* s = layerlist.createSurface(*idReturn);
							 if ( s == NULL ) break;
							 *idReturn = s->getID();
							 s->nativeHandle = nativeHandle;
							 s->setPixelFormat(pixelformat);
							 s->OriginalSourceWidth = OriginalWidth;
							 s->OriginalSourceHeight = OriginalHeight;
							 LOG_DEBUG("CreateCommand","Created surface with new ID: " << s->getID() << " handle " << nativeHandle << " pixelformat:" << pixelformat << " originalwidth:" << OriginalWidth<< " originalheighth:" << OriginalHeight);
							 break;
							 }
			case TypeLayer: {
							Layer* l = layerlist.createLayer(*idReturn);
							if ( l == NULL ) break;
							*idReturn = l->getID();
							LOG_DEBUG("created layer with id:", l->getID());
							break;}
			case TypeSurfaceGroup: {
							SurfaceGroup* sg = layerlist.createSurfaceGroup(*idReturn);
							if ( sg == NULL ) break;
							*idReturn = sg->getID();
//							LOG_DEBUG("created surfacegroup with id:", sg->getID());
							break;}
			case TypeLayerGroup: {
							LayerGroup* lg = layerlist.createLayerGroup(*idReturn);
							if ( lg == NULL ) break;
							*idReturn = lg->getID();
//							LOG_DEBUG("created layergroup with id:", lg->getID());
							break;}
                        default : { break; }
		}
	};

};


#endif /* _CREATECOMMAND_H_ */
