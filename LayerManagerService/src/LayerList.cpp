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

#include "LayerList.h"
#include "Layermanager.h"
#include "Log.h"

void LayerList::init()
{
  layerListMutex = new pthread_mutex_t;
  pthread_mutex_init(layerListMutex,NULL);
}

void LayerList::deinit()
{
  pthread_mutex_destroy(layerListMutex);
  delete layerListMutex;
}

Layer* LayerList::createLayer(int id)
{
  Layer* l = NULL;
  if ( id == -1 )
    {
      l = new Layer();
      allLayers.insert(std::pair<int,Layer*>(l->getID(),l));
    } else {
      l =  getLayer(id);
      if (l == NULL)
        {
          l = new Layer(id);
          allLayers.insert(std::pair<int,Layer*>(l->getID(),l));
        }
    }
  return l;
}

Surface* LayerList::createSurface(int id){
  Surface* s = NULL;
  if ( id == -1 )
    {
      s = new Surface();
      allSurfaces.insert(std::pair<int,Surface*>(s->getID(),s));
    } else {
      s =  getSurface(id);
      if (s == NULL)
        {
          s = new Surface(id);
          allSurfaces.insert(std::pair<int,Surface*>(s->getID(),s));
        }
    }
  return s;
}

SurfaceGroup* LayerList::createSurfaceGroup(int id){
  SurfaceGroup* s = NULL;
  if ( id == -1 )
    {
      s = new SurfaceGroup();
      surfaceGroupList.insert(std::pair<int,SurfaceGroup*>(s->getID(),s));
    } else {
      s =  getSurfaceGroup(id);
      if (s == NULL)
        {
          s = new SurfaceGroup(id);
          surfaceGroupList.insert(std::pair<int,SurfaceGroup*>(s->getID(),s));
        }
    }
  return s;
}

LayerGroup* LayerList::createLayerGroup(int id)
{
  LayerGroup* s = NULL;
  if ( id == -1 )
    {
      s = new LayerGroup();
      layerGroupList.insert(std::pair<int,LayerGroup*>(s->getID(),s));
    } else {
      s =  getLayerGroup(id);
      if (s == NULL)
        {
          s = new LayerGroup(id);
          layerGroupList.insert(std::pair<int,LayerGroup*>(s->getID(),s));
        }
    }
  return s;
}

Layer* LayerList::getLayer(int id){
  Layer* l = NULL;
  if (allLayers.count(id) > 0)
    {
      l = allLayers.at(id);
    } else {
      LOG_WARNING("LayerList","layer not found : id [ " << id << " ]");
    }
  return l;
}

Surface* LayerList::getSurface(int id){
  Surface* s = NULL;
  if ( allSurfaces.count(id) > 0 )
    {
      s = allSurfaces.at(id);

    } else {
      LOG_WARNING("LayerList","surface not found : id [ " << id << " ]");
    }
  return s;
}

SurfaceGroup* LayerList::getSurfaceGroup(const int id){
  return surfaceGroupList.at(id);
}

LayerGroup* LayerList::getLayerGroup(const int id){
  return layerGroupList.at(id);
}

void LayerList::removeLayer(Layer* l){
  // take layer out of list of layers
  if (l != NULL)
    {
      currentRenderOrder.remove(l);
      allLayers.erase(l->getID());
      // remove layer from all layergroups it might be part of
      for (std::map<int,LayerGroup*>::iterator groupIterator=layerGroupList.begin();groupIterator!=layerGroupList.end();groupIterator++){
        LayerGroup* lg = groupIterator->second;
        lg->removeElement(l);
      }

      delete l;
    }
}

void LayerList::removeSurface(Surface* s){
  // take surface out of list of surfaces
  if ( s!=NULL )
    {
      allSurfaces.erase(s->getID());
      // remove surface from all layers it might be on
      for (std::map<int,Layer*>::iterator layerIterator=allLayers.begin();layerIterator!=allLayers.end();layerIterator++){
        Layer* l = layerIterator->second;
        l->removeSurface(s);
      }
      // remove surface from all surfacegroups it might be part of
      for (std::map<int,SurfaceGroup*>::iterator groupIterator=surfaceGroupList.begin();groupIterator!=surfaceGroupList.end();groupIterator++){
        SurfaceGroup* sg = groupIterator->second;
        sg->removeElement(s);
      }
      delete s;
    }
}

void LayerList::removeLayerGroup(LayerGroup* l){
  if (l != NULL)
    {
      layerGroupList.erase(l->getID());
      delete l;
    }
}

void LayerList::removeSurfaceGroup(SurfaceGroup* s)
{
  if (s != NULL)
    {
      surfaceGroupList.erase(s->getID());
      delete s;
    }
}

void LayerList::getLayerIDs(int* length, int** array){
  int numOfLayers = allLayers.size();
  *length = numOfLayers;
  *array = new int[numOfLayers];
  int i=0;
  for (std::map<int,Layer*>::const_iterator it = allLayers.begin(); it != allLayers.end(); it++){
    (*array)[i] = (*it).first;
    i++;
  }
}

void LayerList::getSurfaceIDs(int* length, int** array){
  int numOfSurfaces = allSurfaces.size();
  *length = numOfSurfaces;
  *array = new int[numOfSurfaces];
  int i=0;
  for (std::map<int,Surface*>::const_iterator it = allSurfaces.begin(); it != allSurfaces.end(); it++){
    (*array)[i] = (*it).first;
    i++;
  }
}

void  LayerList::getSurfaceGroupIDs(int* length, int** array){
  int numOfSurfaceGroups = surfaceGroupList.size();

  *length = numOfSurfaceGroups;
  *array = new int[numOfSurfaceGroups];

  int i=0;
  for (std::map<int,SurfaceGroup*>::const_iterator it = surfaceGroupList.begin(); it != surfaceGroupList.end(); it++){
    (*array)[i] = (*it).first;
    i++;
  }
}

void  LayerList::getLayerGroupIDs(int* length, int** array){
  int numOfLayergroups = layerGroupList.size();
  *length = numOfLayergroups;
  *array = new int[numOfLayergroups];
  int i=0;
  for (std::map<int,LayerGroup*>::const_iterator it = layerGroupList.begin(); it != layerGroupList.end(); it++){
    (*array)[i] = (*it).first;
    i++;
  }
}


