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

#include "LayerList.h"
#include "Layermanager.h"
#include "Log.h"

LayerList::LayerList()
{
  layerListMutex = new pthread_mutex_t;
  pthread_mutex_init(layerListMutex,NULL);
}

LayerList::~LayerList()
{
  pthread_mutex_destroy(layerListMutex);
  delete layerListMutex;
}

Layer* LayerList::createLayer(unsigned int id)
{
  Layer* l = NULL;
  if ( id == GraphicalObject::INVALID_ID )
    {
      l = new Layer();
      allLayers.insert(std::pair<unsigned int,Layer*>(l->getID(),l));
    } else {
      l =  getLayer(id);
      if (l == NULL)
        {
          l = new Layer(id);
          allLayers.insert(std::pair<unsigned int,Layer*>(l->getID(),l));
        }else{
        	// id already exists return NUll to indicate error
        	LOG_DEBUG("LayerList","Layer with id [ " << id << " ] " << " already exists ");
        	l= NULL;
        }
    }
  return l;
}

Surface* LayerList::createSurface(unsigned int id){
  Surface* s = NULL;
  if ( id == GraphicalObject::INVALID_ID )
    {
      s = new Surface();
      allSurfaces.insert(std::pair<unsigned int,Surface*>(s->getID(),s));
    } else {
      s =  getSurface(id);
      if (s == NULL)
        {
          s = new Surface(id);
          allSurfaces.insert(std::pair<unsigned int,Surface*>(s->getID(),s));
        }else{
        	// id already exists return NUll to indicate error
        	LOG_DEBUG("LayerList","Surface with id [ " << id << " ] " << " already exists " );
        	s= NULL;
        }
    }
  return s;
}

SurfaceGroup* LayerList::createSurfaceGroup(unsigned int id){
  SurfaceGroup* s = NULL;
  if ( id == GraphicalObject::INVALID_ID )
    {
      s = new SurfaceGroup();
      surfaceGroupList.insert(std::pair<unsigned int,SurfaceGroup*>(s->getID(),s));
    } else {
      s =  getSurfaceGroup(id);
      if (s == NULL)
        {
          s = new SurfaceGroup(id);
          surfaceGroupList.insert(std::pair<unsigned int,SurfaceGroup*>(s->getID(),s));
        }else{
        	// id already exists return NUll to indicate error
        	LOG_DEBUG("LayerList","SurfaceGroup with id [ " << id << " ] " << " already exists ");
        	s= NULL;
        }
    }
  return s;
}

LayerGroup* LayerList::createLayerGroup(unsigned int id)
{
  LayerGroup* s = NULL;
  if ( id == GraphicalObject::INVALID_ID )
    {
      s = new LayerGroup();
      layerGroupList.insert(std::pair<unsigned int,LayerGroup*>(s->getID(),s));
    } else {
      s =  getLayerGroup(id);
      if (s == NULL)
        {
          s = new LayerGroup(id);
          layerGroupList.insert(std::pair<unsigned int,LayerGroup*>(s->getID(),s));
        }else{
        	// id already exists return NUll to indicate error
        	LOG_DEBUG("LayerList","LayerGroup with id [ " << id << " ] " << " already exists " );
        	s= NULL;
        }
    }
  return s;
}

Layer* LayerList::getLayer(unsigned int id){
  Layer* l = NULL;
  if (allLayers.count(id) > 0)
    {
	  std::map<uint,Layer*>::iterator it = allLayers.find(id);
	  if (it != allLayers.end())
		  l = it->second;
    } else {
      LOG_WARNING("LayerList","layer not found : id [ " << id << " ]");
    }
  return l;
}

Surface* LayerList::getSurface(unsigned int id){
  Surface* s = NULL;
  if ( allSurfaces.count(id) > 0 )
    {
	  std::map<uint,Surface*>::iterator it = allSurfaces.find(id);
	  if (it != allSurfaces.end())
		  s = it->second;

    } else {
      LOG_WARNING("LayerList","surface not found : id [ " << id << " ]");
    }
  return s;
}

SurfaceGroup* LayerList::getSurfaceGroup(const unsigned int id){
	SurfaceGroup* s = NULL;
	if ( surfaceGroupList.count(id) > 0 )
	{
	std::map<uint,SurfaceGroup*>::iterator it = surfaceGroupList.find(id);
		  if (it != surfaceGroupList.end())
			  s = it->second;

	} else {
	  LOG_WARNING("LayerList","SurfaceGroup not found : id [ " << id << " ]");
	}
	return s;
}

LayerGroup* LayerList::getLayerGroup(const unsigned int id){
	LayerGroup* s = NULL;
	if ( layerGroupList.count(id) > 0 )
	{
	std::map<uint,LayerGroup*>::iterator it = layerGroupList.find(id);
		  if (it != layerGroupList.end())
			  s = it->second;

	} else {
	  LOG_WARNING("LayerList","LayerGroup not found : id [ " << id << " ]");
	}
	return s;
}

void LayerList::removeLayer(Layer* l){
  // take layer out of list of layers
  if (l != NULL)
    {
      currentRenderOrder.remove(l);
      allLayers.erase(l->getID());
      // remove layer from all layergroups it might be part of
      for (std::map<unsigned int,LayerGroup*>::iterator groupIterator=layerGroupList.begin();groupIterator!=layerGroupList.end();groupIterator++){
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
      for (std::map<unsigned int,Layer*>::iterator layerIterator=allLayers.begin();layerIterator!=allLayers.end();layerIterator++){
        Layer* l = layerIterator->second;
        l->removeSurface(s);
      }
      // remove surface from all surfacegroups it might be part of
      for (std::map<unsigned int,SurfaceGroup*>::iterator groupIterator=surfaceGroupList.begin();groupIterator!=surfaceGroupList.end();groupIterator++){
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

void LayerList::getLayerIDs(unsigned int* length, unsigned int** array){
	unsigned int numOfLayers = allLayers.size();
  *length = numOfLayers;
  *array = new unsigned  int[numOfLayers];
  unsigned int i=0;
  for (std::map<unsigned int,Layer*>::const_iterator it = allLayers.begin(); it != allLayers.end(); it++){
    (*array)[i] = (*it).first;
    i++;
  }
}

bool LayerList::getLayerIDsOfScreen(uint screenID, unsigned int* length, unsigned int** array){
	// check if screen is valid, currently all layers are only on one screen
	if (screenID!=0) // TODO: remove hard-coded value
		return false;

	unsigned int numOfLayers = currentRenderOrder.size();
  *length = numOfLayers;
  *array = new unsigned  int[numOfLayers];
  unsigned int i=0;
  for (std::list<Layer*>::const_iterator it = currentRenderOrder.begin(); it != currentRenderOrder.end(); it++){
    (*array)[i] = (*it)->getID();
    i++;
  }
  return true;
}

void LayerList::getSurfaceIDs(unsigned int* length, unsigned int** array){
	unsigned int numOfSurfaces = allSurfaces.size();
  *length = numOfSurfaces;
  *array = new unsigned int[numOfSurfaces];
  unsigned int i=0;
  for (std::map<unsigned int,Surface*>::const_iterator it = allSurfaces.begin(); it != allSurfaces.end(); it++){
    (*array)[i] = (*it).first;
    i++;
  }
}

void  LayerList::getSurfaceGroupIDs(unsigned int* length, unsigned int** array){
	unsigned int numOfSurfaceGroups = surfaceGroupList.size();

  *length = numOfSurfaceGroups;
  *array = new unsigned int[numOfSurfaceGroups];

  unsigned int i=0;
  for (std::map<unsigned int,SurfaceGroup*>::const_iterator it = surfaceGroupList.begin(); it != surfaceGroupList.end(); it++){
    (*array)[i] = (*it).first;
    i++;
  }
}

void  LayerList::getLayerGroupIDs(unsigned int* length, unsigned int** array){
	unsigned int numOfLayergroups = layerGroupList.size();
  *length = numOfLayergroups;
  *array = new unsigned int[numOfLayergroups];
  unsigned int i=0;
  for (std::map<unsigned int,LayerGroup*>::const_iterator it = layerGroupList.begin(); it != layerGroupList.end(); it++){
    (*array)[i] = (*it).first;
    i++;
  }
}


