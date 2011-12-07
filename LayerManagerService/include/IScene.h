/***************************************************************************
 *
 * Copyright 2010,2011 BMW Car IT GmbH
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

#ifndef _ISCENE_H_
#define _ISCENE_H_

#include <semaphore.h>
#include "Layer.h"
#include "LayerGroup.h"
#include "Surface.h"
#include "SurfaceGroup.h"
#include "LayerList.h"
#include "SurfaceMap.h"

/*
 * Represents a scene with screens which have Layers which contain the Surfaces.
 * Sorting is based upon z-order of the contained layers.
 * Additionally surface and layer group can be used to apply attributes to multiple surfaces or layers at once.
 */
class IScene
{
public:
    virtual ~IScene() {}

    virtual Layer* createLayer(const uint id) = 0;
    virtual Surface* createSurface(const uint id) = 0;
    virtual LayerGroup* createLayerGroup(const uint id) = 0;
    virtual SurfaceGroup* createSurfaceGroup(const uint id) = 0;

    virtual void removeLayer(Layer* layer) = 0;
    virtual void removeSurface(Surface* surface) = 0;
    virtual Layer* getLayer(const uint id) const = 0;
    virtual Surface* getSurface(const uint id) const = 0;
    virtual SurfaceGroup* getSurfaceGroup(const uint id) const = 0;
    virtual LayerGroup* getLayerGroup(const uint id) const = 0;

    /**
     * Get list of ids of all layers currently existing.
     * @return list of ids of all currently know layers
     */
    virtual void getLayerIDs(uint* length, uint** array) const = 0;

    /**
     * Get list of ids of all layers currently existing.
     * @return list of ids of all currently know layers
     */
    virtual bool getLayerIDsOfScreen(const uint screenID, uint* length, uint** array) const = 0;

    /**
     * Get list of ids of all surfaces currently existing.
     * @return list of ids of all currently know surfaces
     */
    virtual void getSurfaceIDs(uint* length, uint** array) const = 0;

    /**
     * Get list of ids of all layers currently existing.
     * @return list of ids of all currently know layers
     */
    virtual void getLayerGroupIDs(uint* length, uint* array[]) const = 0;

    /**
     * Get list of ids of all layers currently existing.
     * @return list of ids of all currently know layers
     */
    virtual void getSurfaceGroupIDs(uint* length, uint* array[]) const = 0;

    /**
     * Lock the list for read and write access
     */
    virtual void lockScene() = 0;

    /**
     * Unlock the list for read and write access
     */
    virtual void unlockScene() = 0;

    virtual LayerList& getCurrentRenderOrder() = 0;

    virtual void removeSurfaceGroup(SurfaceGroup *surface) = 0;

    virtual void removeLayerGroup(LayerGroup *layer) = 0;

    virtual const SurfaceMap getAllSurfaces() const = 0;

    virtual Surface* getSurfaceAt(unsigned int *x, unsigned int *y, double minOpacity) = 0;

    virtual bool isLayerInCurrentRenderOrder(const uint id) = 0;

    bool debugMode;

};

#endif /* _ISCENE_H_ */
