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

#ifndef _LAYERMANAGER_H_
#define _LAYERMANAGER_H_

#include "ObjectType.h"
#include "IRenderer.h"
#include "Surface.h"
#include "Layer.h"
#include "CommandExecutor.h"
#include "LayerList.h"
#include "BaseCommunicator.h"
#include "LogicalGraphicsObject.h"
#include "GraphicalSurface.h"
#include "GraphicalGroup.h"
#include "commands/Command.h"
#include <vector>
#include "LayerType.h"

class Layermanager : public CommandExecutor{
public:
	// the singleton reference
	static Layermanager* instance;
	void execute(Command* commandToBeExecuted);
	uint getLayerTypeCapabilities(LayerType layertype);

	LayerList layerlist;

	void addRenderer(IRenderer* renderer);
	void RemoveRenderer(IRenderer* renderer);
	void addCommunicator(BaseCommunicator* communicator);
	bool startManagement( int width,int height,const char* displayName );
	bool stopManagement();

private:
	Layermanager();

	std::list<IRenderer*> compositingControllers;
	std::list<BaseCommunicator*> communicationControllers;
	void printDebugInformation();
};

#endif /* _LAYERMANAGER_H_ */
