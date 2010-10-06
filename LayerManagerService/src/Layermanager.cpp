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

#include "Layermanager.h"
#include "Log.h"
#include <iomanip>

Layermanager* Layermanager::instance = new Layermanager();

Layermanager::Layermanager() {
	// default layer
	//Layer* defaultLayer = layerlist.createLayer();
}


uint Layermanager::getLayerTypeCapabilities(LayerType layertype){
	// get the first renderer for now TODO
	IRenderer* renderer = *compositingControllers.begin();
	if (NULL!=renderer)
		return renderer->getLayerTypeCapabilities(layertype);
	else
		return 0;
}

void Layermanager::addRenderer(IRenderer* renderer){
	compositingControllers.push_back(renderer);
}
void Layermanager::RemoveRenderer(IRenderer* renderer){
	compositingControllers.remove(renderer);
}
void Layermanager::addCommunicator(BaseCommunicator* communicator){
	communicationControllers.push_back(communicator);
}

void Layermanager::printDebugInformation(){
	// print stuff about layerlist
	LOG_INFO("LayerManager", "Layer: ID |  X |  Y |  W |  H |Al.| Z \n");
	// loop the layers
	int length;
	int* LayerIDs;
	layerlist.getLayerIDs(&length,&LayerIDs);
	for (int i=0;i<length;i++)
//	for(std::list<int>::iterator currentLayerID = LayerIDs.begin();
//		currentLayerID != LayerIDs.end(); currentLayerID++)
	{
		Layer* currentLayer = layerlist.getLayer(LayerIDs[i]);
		LOG_INFO("LayerManager", "      " << std::setw(4) << currentLayer->getID()   << "\n");

		LOG_INFO("LayerManager", "    Surface:  ID |Al.| Z |  SVP: X |  Y |  W |  H     DVP:  X |  Y |  W |  H \n");
		// loop the surfaces of within each layer
		for(std::list<Surface*>::iterator current = currentLayer->surfaces.begin();
			current != currentLayer->surfaces.end(); current++)
		{
			LOG_INFO("LayerManager", "            " << std::setw(4) << (*current)->getID()  << std::setw(4) << std::setprecision(3) << (*current)->opacity << "\n");
		}
	}

}


void Layermanager::execute(Command*  commandToBeExecuted){
	LOG_DEBUG("Layermanager", "executing a command");
	switch(commandToBeExecuted->commandType){
		case Command::Remove:
		case Command::SetVisibility:
		case Command::SetOpacity:
		case Command::SetSourceRectangle:
		case Command::SetDestinationRectangle:
		case Command::SetPosition:
		case Command::SetDimension:
		case Command::SetOrientation:
		case Command::LayerAddSurface:
		case Command::LayerRemoveSurface:
		case Command::LayergroupAddLayer:
		case Command::LayergroupRemoveLayer:
		case Command::SurfacegroupAddSurface:
		case Command::SurfacegroupRemoveSurface:
		case Command::SetLayerRenderOrder:
		case Command::SetSurfaceRenderOrderWithinLayer:
					{
						layerlist.toBeCommittedList.push_back(commandToBeExecuted);
						break;
					}
		case Command::Create:
		default: {
					commandToBeExecuted->execute(layerlist);
					delete commandToBeExecuted;
					break;
				}
	}
}


bool Layermanager::startManagement(int width,int height,const char* displayName)
{
	bool result = false;

	for( std::list<IRenderer*>::iterator list_iter = compositingControllers.begin();
		 list_iter != compositingControllers.end(); list_iter++)
	{
		if ( (NULL != *list_iter) && ( true==(*list_iter)->start(width, height, displayName) )  )
		{
				result = true;
		}
		else
		{
			result = false;
			break;
		}
	}
	if (result == false)
	{
		LOG_ERROR("LayerManager","Could not start Compositing Controllers");
	}
	else
	{
		for(std::list<BaseCommunicator*>::iterator list_iter = communicationControllers.begin();
				list_iter != communicationControllers.end(); list_iter++)
		{
			if ( (NULL != *list_iter) && ( true==(*list_iter)->start() )  )
			{
					result = true;
			}
			else
			{
				result = false;
				break;
			}
		}
		if (result == false)
		{
			LOG_ERROR("LayerManager","Could not start Communication Controllers");
		}
	}

	return result;
}
