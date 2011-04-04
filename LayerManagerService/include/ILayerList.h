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

#ifndef _ILAYERLIST_H_
#define _ILAYERLIST_H_

#include <semaphore.h>
#include <list>
#include <map>
#include "Layer.h"
#include "GraphicalGroup.h"

typedef GraphicalGroup<Layer,TypeLayerGroup> LayerGroup;
typedef GraphicalGroup<Surface, TypeSurfaceGroup> SurfaceGroup;

/*
 * Represents a list of Layers which contain the Surfaces. Sorting is based upon zorder of the contained layers.
 */
class ILayerList{
public:
		virtual Layer* createLayer(uint id)=0;
		virtual Surface* createSurface(uint id)=0;
		virtual LayerGroup* createLayerGroup(uint id)=0;
		virtual SurfaceGroup* createSurfaceGroup(uint id)=0;

		virtual void removeLayer(Layer* layer)=0;
		virtual void removeSurface(Surface* surface)=0;
		virtual Layer* getLayer(uint id)=0;
		virtual Surface* getSurface(uint id)=0;
		virtual SurfaceGroup* getSurfaceGroup(const uint id)=0;
		virtual LayerGroup* getLayerGroup(const uint id)=0;

		/**
		 * Get list of ids of all layers currently existing.
		 * @return list of ids of all currently know layers
		 */
		virtual void getLayerIDs(uint* length, uint** array)=0;

		/**
		 * Get list of ids of all layers currently existing.
		 * @return list of ids of all currently know layers
		 */
		virtual bool getLayerIDsOfScreen(uint screenID, uint* length, uint** array)=0;

		/**
		 * Get list of ids of all surfaces currently existing.
		 * @return list of ids of all currently know surfaces
		 */
		virtual void getSurfaceIDs(uint* length, uint** array)=0;

		/**
		 * Get list of ids of all layers currently existing.
		 * @return list of ids of all currently know layers
		 */
		virtual void getLayerGroupIDs(uint* length, uint* array[])=0;

		/**
		 * Get list of ids of all layers currently existing.
		 * @return list of ids of all currently know layers
		 */
		virtual void getSurfaceGroupIDs(uint* length, uint* array[])=0;

		/**
		 * Lock the list for read and write access
		 */
		virtual void lockList()=0;

		/**
		 * Unlock the list for read and write access
		 */
		virtual void unlockList()=0;

		bool debugMode;

};

#endif /* _ILAYERLIST_H_ */
