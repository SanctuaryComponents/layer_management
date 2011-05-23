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

#include "Layermanager.h"
#include "Log.h"
#include <iomanip>

Layermanager* Layermanager::instance = new Layermanager();

Layermanager::Layermanager() {
	// default layer
	//Layer* defaultLayer = layerlist.createLayer();
    //    layerlist.init();
}


uint Layermanager::getLayerTypeCapabilities(LayerType layertype){
	// get the first renderer for now TODO
	// change to possibly load multiple renderers
	IRenderer* renderer = *compositingControllers.begin();
	if (NULL!=renderer)
		return renderer->getLayerTypeCapabilities(layertype);
	else
		return 0;
}

uint Layermanager::getNumberOfHardwareLayers(uint screenID){
	// get the first renderer for now TODO
	// change to possibly load multiple renderers
	IRenderer* renderer = *compositingControllers.begin();
	if (NULL!=renderer)
		return renderer->getNumberOfHardwareLayers(screenID);
	else
		return 0;
}

uint* Layermanager::getScreenResolution(uint screenID){
	// get the first renderer for now TODO
	// change to possibly load multiple renderers
	IRenderer* renderer = *compositingControllers.begin();
	if (NULL!=renderer)
		return renderer->getScreenResolution(screenID);
	else
		return 0;
}

uint* Layermanager::getScreenIDs(uint* length){
	// get the first renderer for now TODO
	// change to possibly load multiple renderers
	IRenderer* renderer = *compositingControllers.begin();
	if (NULL!=renderer)
		return renderer->getScreenIDs(length);
	else
		return 0;
}

// This function is used to propagate a signal to the window system
// via the renderer for a redraw because of a rendering property change
void Layermanager::signalRendererRedraw()
{
	IRenderer* renderer = *compositingControllers.begin();
	if (NULL!=renderer)
		renderer->signalWindowSystemRedraw();
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
	unsigned int length;
	unsigned int* LayerIDs;
	layerlist.getLayerIDs(&length,&LayerIDs);
	for (int i=0;i<length;i++)
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


bool Layermanager::execute(Command*  commandToBeExecuted){
	bool status=false;
	bool signalRenderer = false;
	static bool signalRendererAfterCommit = false;
	LOG_DEBUG("Layermanager", "executing a command, locking list");
        layerlist.lockList();
        /*LOG_DEBUG("Layermanager", "executing a command, locking list success");*/
	switch(commandToBeExecuted->commandType){
		case Command::SetVisibility:
		case Command::SetOpacity:
		case Command::SetSourceRectangle:
		case Command::SetDestinationRectangle:
		case Command::SetPosition:
		case Command::SetDimension:
		case Command::SetOrientation:
		case Command::LayerAddSurface:
		case Command::LayerRemoveSurface:
		case Command::SetLayerRenderOrder:
		case Command::SetSurfaceRenderOrderWithinLayer:
			signalRendererAfterCommit = true;
		case Command::LayergroupAddLayer:
		case Command::LayergroupRemoveLayer:
		case Command::SurfacegroupAddSurface:
		case Command::SurfacegroupRemoveSurface:
                {
                        layerlist.toBeCommittedList.push_back(commandToBeExecuted);
                        status = true;
                        break;
                }
		case Command::ScreenShot:
		{
			LOG_INFO("LayerManager","making screenshot");
			ScreenShotCommand* screenShotCommand = (ScreenShotCommand*)commandToBeExecuted;
			LOG_INFO("LayerManager","file: " << screenShotCommand->filename);
			// check if screen is valid
			if (screenShotCommand->type == ScreenshotOfDisplay && screenShotCommand->id!=0){
				status = false;
				break;
			}else if (screenShotCommand->type == ScreenshotOfLayer){
				Layer* l = layerlist.getLayer(screenShotCommand->id);
				if (l==NULL){
					status = false;
					break;
				}
			}else if (screenShotCommand->type == ScreenshotOfSurface){
				Surface* s = layerlist.getSurface(screenShotCommand->id);
								if (s==NULL){
									status = false;
									break;
								}
			}

			// call screen shot on all renderers for now TODO
			for( std::list<IRenderer*>::iterator list_iter = compositingControllers.begin();
			     list_iter != compositingControllers.end();
			     list_iter++)
			  {
					if ( NULL != *list_iter)
					  {
							if (screenShotCommand->type == ScreenshotOfDisplay)
								(*list_iter)->doScreenShot(std::string(screenShotCommand->filename));
							else if (screenShotCommand->type == ScreenshotOfLayer)
								(*list_iter)->doScreenShotOfLayer(std::string(screenShotCommand->filename),screenShotCommand->id);
							else if (screenShotCommand->type == ScreenshotOfSurface)
								(*list_iter)->doScreenShotOfSurface(std::string(screenShotCommand->filename),screenShotCommand->id);
					  }
			  }
			status = true;
			signalRendererRedraw();
			break;
		}
		// TODO: Theoretically both SetShader and SetUniforms should be considered for signaling the window system
		// But currently there is no mechanism for doing this for SetUniforms, as surface/layer doesn't
		// keep track of uniform in its shader
		case Command::SetShader:
//		case Command::SetUniforms:
			signalRenderer = true;
		case Command::Create:
		default:
		  {
                        status = commandToBeExecuted->execute(layerlist);
						if (commandToBeExecuted->commandType == Command::CommitChanges && signalRendererAfterCommit)
						{
							signalRendererRedraw();
							signalRendererAfterCommit = false;
						}
						else if (signalRenderer)
						{
							signalRendererRedraw();
						}
                        delete commandToBeExecuted;
                        break;
		  }
	}
        /* LOG_DEBUG("Layermanager", "executing a command, unlocking list");*/
        layerlist.unlockList();
        LOG_DEBUG("Layermanager", "executing a command, unlocking list success");
        return status;
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

bool Layermanager::stopManagement(){
	bool result = false;

		for( std::list<IRenderer*>::iterator list_iter = compositingControllers.begin();
			 list_iter != compositingControllers.end(); list_iter++)
		{
			if ( NULL != *list_iter)
			{
				(*list_iter)->stop();
			}
		}
			for(std::list<BaseCommunicator*>::iterator list_iter = communicationControllers.begin();
					list_iter != communicationControllers.end(); list_iter++)
			{
				if (NULL != *list_iter )
				{
					(*list_iter)->stop();
				}
			}

		return true;
}
