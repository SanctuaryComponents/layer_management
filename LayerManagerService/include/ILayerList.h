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
		virtual Layer* createLayer(int id)=0;
		virtual Surface* createSurface(int id)=0;
                virtual LayerGroup* createLayerGroup(int id)=0;
                virtual SurfaceGroup* createSurfaceGroup(int id)=0;

		virtual void removeLayer(Layer* layer)=0;
		virtual void removeSurface(Surface* surface)=0;
		virtual Layer* getLayer(int id)=0;
		virtual Surface* getSurface(int id)=0;
		virtual SurfaceGroup* getSurfaceGroup(const int id)=0;
		virtual LayerGroup* getLayerGroup(const int id)=0;

		/**
			 * Get list of ids of all layers currently existing.
			 * @return list of ids of all currently know layers
			 */
		virtual void getLayerIDs(int* length, int** array)=0;
		virtual void getSurfaceIDs(int* length, int** array)=0;

		/**
		 * Get list of ids of all layers currently existing.
		 * @return list of ids of all currently know layers
		 */
		virtual void getLayerGroupIDs(int* length, int* array[])=0;
	        /**
                 * Get list of ids of all layers currently existing.
                 * @return list of ids of all currently know layers
                 */
                virtual void getSurfaceGroupIDs(int* length, int* array[])=0;
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
