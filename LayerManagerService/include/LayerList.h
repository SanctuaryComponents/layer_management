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

#ifndef _LAYERLIST_H_
#define _LAYERLIST_H_

#include <semaphore.h>
#include <list>
#include "Layer.h"
#include "GraphicalGroup.h"
#include "ILayerList.h"

#include "Shader.h"
#include <map>

class Command;
/*
 * Represents a list of Layers which contain the Surfaces.
 */
class LayerList : public ILayerList {
public:
	 Layer* createLayer();
	 Surface* createSurface();
	 SurfaceGroup* createSurfaceGroup();
	 LayerGroup* createlayerGroup();
	 void removeLayer(Layer* layer);
	 void removeSurface(Surface* surface);
	 void removeLayerGroup(LayerGroup* layer);
	 void removeSurfaceGroup(SurfaceGroup* surface);
	 Layer* getLayer(int id);
	 Surface* getSurface(int id);
	 SurfaceGroup* getSurfaceGroup(const int id);
	 LayerGroup* getLayerGroup(const int id);
	 void getLayerIDs(int* length, int** array);
	 void getSurfaceIDs(int* length, int** array);
	 void getLayerGroupIDs(int* length, int** array);
	 void getSurfaceGroupIDs(int* length, int** array);

	 std::list<Layer*>& getCurrentRenderOrder(){return currentRenderOrder;}
	 const std::map<int,Layer*> getAllLayers(){return allLayers;}
	 const std::map<int,Surface*> getAllSurfaces(){return allSurfaces;}
	 std::list<Command*> toBeCommittedList;
	 std::map<int,Shader*> shaderList;

private:
	std::map<int,LayerGroup*> layerGroupList;
	std::map<int,SurfaceGroup*> surfaceGroupList;
	std::map<int,Surface*> allSurfaces;
	std::map<int,Layer*> allLayers;
	std::list<Layer*> currentRenderOrder;
};

#endif /* _LAYERLIST_H_ */
