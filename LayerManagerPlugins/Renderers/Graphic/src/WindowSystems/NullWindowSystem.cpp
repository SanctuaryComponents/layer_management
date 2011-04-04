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

#include "WindowSystems/NullWindowSystem.h"
#include "Log.h"
#include "Layer.h"
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iomanip>


bool NullWindowSystem::m_initialized= false;


bool NullWindowSystem::m_success= false;


bool NullWindowSystem::debugMode= false;


void NullWindowSystem::printDebug(int posx,int posy){
	// print stuff about layerlist
	std::stringstream debugmessage;
	debugmessage << "Layer:  ID |   X  |   Y  |   W  |   H  | Al. \n";
	// loop the layers
	std::list<Layer*> list = layerlist->getCurrentRenderOrder();
	for(std::list<Layer*>::const_iterator currentLayer = list.begin();
	currentLayer != list.end(); currentLayer++)
	{
		Rectangle dest = (*currentLayer)->getDestinationRegion();
		debugmessage << "            " << std::setw(4) << (*currentLayer)->getID() << " " << std::setw(3) << dest.x << " " << std::setw(3) << dest.y << " " << std::setw(3) << dest.width << " " << std::setw(3) << dest.height << " " << std::setw(3) << (*currentLayer)->opacity << "\n";

		debugmessage << "    Surface:  ID |Al.|  SVP: X |  Y |  W |  H     DVP:  X |  Y |  W |  H \n";
		// loop the surfaces of within each layer
		for(std::list<Surface*>::iterator current = (*currentLayer)->surfaces.begin();
		current != (*currentLayer)->surfaces.end(); current++)
		{
			Rectangle src = (*current)->getSourceRegion();
			Rectangle dest = (*current)->getDestinationRegion();
			debugmessage << "                        " << std::setw(4) << (*current)->getID() << " " << std::setprecision(3) << (*current)->opacity<< " " << std::setw(3) << src.x << " " << std::setw(3) << src.y << " " << std::setw(3) << src.width << " " << std::setw(3) << src.height << " " << std::setw(3) << dest.x << " " << std::setw(3) << dest.y << " " << std::setw(3) << dest.width << " " << std::setw(3) << dest.height  << "\n";
		}
	}
	LOG_INFO("NullWindowSystem",debugmessage.str());
}

float			TimeCounter, LastFrameTimeCounter, DT, prevTime = 0.0, FPS;
struct timeval		tv, tv0;
int			Frame = 1, FramesPerFPS;

void UpdateTimeCounter() {
	LastFrameTimeCounter = TimeCounter;
	gettimeofday(&tv, NULL);
	TimeCounter = (float)(tv.tv_sec-tv0.tv_sec) + 0.000001*((float)(tv.tv_usec-tv0.tv_usec));
	DT = TimeCounter - LastFrameTimeCounter; }

void CalculateFPS() {
	Frame ++;

	if((Frame%FramesPerFPS) == 0) {
		FPS = ((float)(FramesPerFPS)) / (TimeCounter-prevTime);
		prevTime = TimeCounter; } }


void NullWindowSystem::Redraw()
{
//	LOG_INFO("NullWindowSystem","redraw");
	// draw all the layers
	graphicSystem->clearBackground();
	std::list<Layer*> layers = layerlist->getCurrentRenderOrder();
	for(std::list<Layer*>::const_iterator current = layers.begin(); current != layers.end(); current++){
		Layer* currentLayer = (Layer*)*current;
		if ((*currentLayer).visibility && (*currentLayer).opacity>0.0f){
			// glPushMatrix();
			//   glRotated(-90*currentLayer->getOrientation(),0,0,1.0);
			// draw all the surfaces of the layer
			std::list<Surface*> surfaces = currentLayer->surfaces;
			for(std::list<Surface*>::const_iterator currentS = surfaces.begin(); currentS != surfaces.end(); currentS++){
				if ((*currentS)->visibility && (*currentS)->opacity>0.0f){
					Surface* currentSurface = (Surface*)*currentS;
					PlatformSurface* nativeSurface = (PlatformSurface*)currentSurface->platform;
					if (NULL==nativeSurface)
					{
						LOG_INFO("NullWindowSystem","creating native surface for new window");
						// this surface does not have a native platform surface attached yet!
						currentSurface->platform = graphicSystem->m_binder->createPlatformSurface(currentSurface);
						currentSurface->OriginalSourceWidth=256;
						currentSurface->OriginalSourceHeight=256;
						graphicSystem->m_binder->createClientBuffer(currentSurface);
						LOG_INFO("NullWindowSystem","created native surface for new window");
					}
					graphicSystem->drawSurface(currentLayer,currentSurface);

				}
			}
		}
	}

	if (debugMode)
	{
		printDebug(200,windowHeight-40);
	}
	UpdateTimeCounter();
	CalculateFPS();
	char floatStringBuffer[256];
	sprintf(floatStringBuffer, "FPS: %f", FPS);
	if ((Frame % 1000 ) == 0)
	{
		LOG_INFO("NullWindowSystem",floatStringBuffer);
	}

	graphicSystem->swapBuffers();

}



void* NullWindowSystem::EventLoop(void * ptr)
{
	LOG_DEBUG("NullWindowSystem", "Enter thread");
	NullWindowSystem *windowsys = (NullWindowSystem *) ptr;

	// init own stuff
	m_success = false;

	// then init graphiclib
	int display = 0;
	void* window = NULL;
	m_success = windowsys->graphicSystem->init(&display,&window,windowHeight,windowWidth);

	m_initialized = true;
	// start
//	// get instance of Renderer for this thread
	Layer* defaultLayer;
//	X11WindowSystem *renderer = (X11WindowSystem *) ptr;
//	if (renderer != NULL)
//	{
//	}
//	// initialize the renderer (OpenGL etc)
//	renderer->init();
//	// run the main event loop while rendering
	bool running = windowsys->m_success;
	gettimeofday(&tv0, NULL);
	FramesPerFPS = 30;
	if (debugMode)
	{
		defaultLayer = windowsys->layerlist->createLayer();
		defaultLayer->setOpacity(1.0);
		defaultLayer->setDestinationRegion(Rectangle(0,0,windowsys->resolutionWidth,windowsys->resolutionHeight));
		defaultLayer->setSourceRegion(Rectangle(0,0,windowsys->resolutionWidth,windowsys->resolutionHeight));
		windowsys->layerlist->getCurrentRenderOrder().push_back(defaultLayer);
	}
	LOG_DEBUG("NullWindowSystem", "Enter render loop");
	while (running)
	{
		windowsys->Redraw();
	}
	LOG_INFO("NullWindowSystem", "Renderer thread finished");
	return NULL;
}


bool NullWindowSystem::start(int displayWidth, int displayHeight, const char* DisplayName){
	LOG_INFO("NullWindowSystem", "Starting / Creating thread");
	pthread_t renderThread;
	NullWindowSystem *renderer = this;
	resolutionWidth = displayWidth;
	resolutionHeight = displayHeight;
	int status = pthread_create( &renderThread, NULL, NullWindowSystem::EventLoop, (void*) renderer);
	if (0 != status )
	{
		return false;
	}
	while ( m_initialized == false )
	{
		sleep(1);
		LOG_INFO("NullWindowSystem","Waiting start complete " << m_initialized);
	}
	LOG_INFO("NullWindowSystem","Start complete " << m_initialized << " success " << m_success);
	return m_success;
}
