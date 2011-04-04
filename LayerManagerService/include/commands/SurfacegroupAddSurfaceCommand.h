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

#ifndef _SURFACEGROUPADDSURFACECOMMAND_H_
#define _SURFACEGROUPADDSURFACECOMMAND_H_

#include "Command.h"
#include "ObjectType.h"

class SurfacegroupAddSurfaceCommand : public Command{
public:
	SurfacegroupAddSurfaceCommand(unsigned int surfacegroupid, unsigned int surfaceid) : Command(SurfacegroupAddSurface), surfacegroupid(surfacegroupid), surfaceid(surfaceid){};
	const unsigned int surfacegroupid;
	const unsigned int surfaceid;

	virtual bool execute(LayerList& layerlist){
		SurfaceGroup* sg = layerlist.getSurfaceGroup(surfacegroupid);
		Surface* surface = layerlist.getSurface(surfaceid);
		if ( sg != NULL && surface != NULL )
		{

			// check if already a member of the group
			const std::list<Surface*> list = sg->getList();
			std::list<Surface*>::const_iterator it;
			it = std::find (list.begin(), list.end(), surface);
			if (it!=list.end()){
				LOG_ERROR("SurfacegroupAddSurfaceCommand","Surface is already a member of the group");
				return false; // already is member of the group
						}

			sg->addElement(surface);
		}else{
			return false;
		}
		return true;
	};
};


#endif /* _SURFACEGROUPADDSURFACECOMMAND_H_ */
