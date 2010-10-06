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

#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <iostream>
#include <LayerList.h>

class Command {
public:
	enum CommandType{
			Create,
			Remove,
			SetVisibility,
			SetOpacity,
			GetVisibility,
			GetOpacity,
			SetSourceRectangle,
			SetDestinationRectangle,
			SetPosition,
			SetDimension,
			SetOrientation,
			GetOrientation,
			GetDimension,
			GetPosition,
			GetPixelformat,
			LayerAddSurface,
			LayerRemoveSurface,
			LayergroupAddLayer,
			LayergroupRemoveLayer,
			SurfacegroupAddSurface,
			SurfacegroupRemoveSurface,
			CommitChanges,
			SetLayerRenderOrder,
			SetSurfaceRenderOrderWithinLayer,
			Debug,
			Exit,
			CreateShader,
			DestroyShader,
			SetShader,
			SetUniforms
		};

	Command(CommandType commandType) :commandType(commandType) {};
	virtual ~Command(){};
	const CommandType commandType;
	virtual void execute(LayerList& layerlist)=0;
};

#endif /* _COMMAND_H_ */
