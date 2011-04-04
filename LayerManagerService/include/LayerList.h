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

#ifndef _LAYERLIST_H_
#define _LAYERLIST_H_

#include <pthread.h>
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
  LayerList();
  ~LayerList();
  Layer* createLayer(unsigned int id);
  Surface* createSurface(unsigned int id);
  SurfaceGroup* createSurfaceGroup(unsigned int id);
  LayerGroup* createLayerGroup(unsigned int id);
  void removeLayer(Layer* layer);
  void removeSurface(Surface* surface);
  void removeLayerGroup(LayerGroup* layer);
  void removeSurfaceGroup(SurfaceGroup* surface);
  Layer* getLayer(unsigned int id);
  Surface* getSurface(unsigned int id);
  SurfaceGroup* getSurfaceGroup(const unsigned int id);
  LayerGroup* getLayerGroup(const unsigned int id);
  void getLayerIDs(unsigned int* length, unsigned int** array);
  bool getLayerIDsOfScreen(uint screenID, uint* length, uint** array);
  void getSurfaceIDs(unsigned int* length, unsigned int** array);
  void getLayerGroupIDs(unsigned int* length, unsigned int** array);
  void getSurfaceGroupIDs(unsigned int* length, unsigned int** array);
  void lockList(){ pthread_mutex_lock(layerListMutex); }
  void unlockList(){pthread_mutex_unlock(layerListMutex);};
  std::list<Layer*>& getCurrentRenderOrder(){return currentRenderOrder;}
  const std::map<unsigned int,Layer*> getAllLayers(){return allLayers;}
  const std::map<unsigned int,Surface*> getAllSurfaces(){return allSurfaces;}
  std::list<Command*> toBeCommittedList;
  std::map<unsigned int,Shader*> shaderList;
private:
  pthread_mutex_t *layerListMutex;
  std::map<unsigned int,LayerGroup*> layerGroupList;
  std::map<unsigned int,SurfaceGroup*> surfaceGroupList;
  std::map<unsigned int,Surface*> allSurfaces;
  std::map<unsigned int,Layer*> allLayers;
  std::list<Layer*> currentRenderOrder;
};

#endif /* _LAYERLIST_H_ */
