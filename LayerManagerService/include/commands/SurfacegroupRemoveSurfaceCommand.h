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

#ifndef _SURFACEGROUPREMOVESURFACECOMMAND_H_
#define _SURFACEGROUPREMOVESURFACECOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class SurfacegroupRemoveSurfaceCommand : public Command{
public:
	SurfacegroupRemoveSurfaceCommand(int surfacegroupid, int surfaceid) : Command(SurfacegroupRemoveSurface), surfacegroupid(surfacegroupid), surfaceid(surfaceid){};
	const int surfacegroupid;
	const int surfaceid;

	virtual void execute(LayerList& layerlist){
		SurfaceGroup* sg = layerlist.getSurfaceGroup(surfacegroupid);
		Surface* surface = layerlist.getSurface(surfaceid);
		if ( sg != NULL && surface != NULL )
		{
			sg->removeElement(surface);
		}
	};
};


#endif /* _SURFACEGROUPREMOVESURFACECOMMAND_H_ */
