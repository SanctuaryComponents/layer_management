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

#ifndef _COMMANDEXECUTOR_H_
#define _COMMANDEXECUTOR_H_

#include "commands/Command.h"
#include "commands/CommitCommand.h"
#include "commands/CreateCommand.h"
#include "commands/GetDimensionCommand.h"
#include "commands/GetOpacityCommand.h"
#include "commands/GetPixelformatCommand.h"
#include "commands/GetVisibilityCommand.h"
#include "commands/LayerAddSurfaceCommand.h"
#include "commands/LayerRemoveSurface.h"
#include "commands/RemoveCommand.h"
#include "commands/GetOrientationCommand.h"
#include "commands/LayergroupAddLayerCommand.h"
#include "commands/LayergroupRemoveLayerCommand.h"
#include "commands/SetDestinationRectangleCommand.h"
#include "commands/SetOpacityCommand.h"
#include "commands/SetSourceRectangleCommand.h"
#include "commands/SetOrientationCommand.h"
#include "commands/SurfacegroupAddSurfaceCommand.h"
#include "commands/SurfacegroupRemoveSurfaceCommand.h"
#include "commands/SetVisibilityCommand.h"
#include "commands/DebugCommand.h"
#include "commands/ExitCommand.h"
#include "commands/SetLayerOrderCommand.h"
#include "commands/SetOrderWithinLayerCommand.h"
#include "commands/SetDimensionCommand.h"
#include "commands/SetPositionCommand.h"
#include "commands/GetPositionCommand.h"
#include "commands/CreateShaderCommand.h"
#include "commands/DestroyShaderCommand.h"
#include "commands/SetShaderCommand.h"
#include "commands/SetUniformsCommand.h"
#include "commands/ScreenShotCommand.h"

/**
 * Objects who implement this interface can be used to have command objects executed. Communication
 * classes must only know this interface of a class to be able to pass along command objects.
 *
 * Implementation detail: This interface is used to reduce dependency of communicators on the main layermanagement component
 */
class CommandExecutor{
public:
	/**
	 * Have a command processed.
	 * @param commandToBeExecuted The command to be processed
	 */
	virtual void execute(Command* commandToBeExecuted)=0;

	virtual uint getLayerTypeCapabilities(LayerType layertype)=0;

};

#endif /* _COMMANDEXECUTOR_H_ */
