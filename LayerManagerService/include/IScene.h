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

/**
 * \defgroup SceneAPI Layer Management Scene API
 */

/**
 * \brief Represents the scene, which is a tree of scene objects
 *
 * Represents a scene with screens which have Layers which contain the Surfaces.
 * Sorting is based upon z-order of the contained layers.
 * Additionally surface and layer group can be used to apply attributes to multiple surfaces or layers at once.
 */
class IScene
{
public:
    /**
     * \brief default destructor
     * \ingroup SceneAPI
     */
    virtual ~IScene() {}

    /**
     * \brief Creates a new layer within the scene.
     * \ingroup SceneAPI
     * \param[in] id id of layer
     * \return pointer to layer
     */
    virtual Layer* createLayer(const uint id) = 0;

    /**
     * \brief Creates a new surface within the scene.
     * \ingroup SceneAPI
     * \param[in] id id of surface
     * \return pointer to surface
     */
    virtual Surface* createSurface(const uint id) = 0;

    /**
     * \brief Create a new layer group within the scene.
     * \ingroup SceneAPI
     * \param[in] id id of the layer group
     * \return pointer to layer group
     */
    virtual LayerGroup* createLayerGroup(const uint id) = 0;

    /**
     * \brief Create a new surface group within the scene.
     * \ingroup SceneAPI
     * \param[in] id id of the new surface group
     * \return pointer to surface group
     */
    virtual SurfaceGroup* createSurfaceGroup(const uint id) = 0;

    /**
     * \brief Remove a layer from the scene.
     * \ingroup SceneAPI
     * \param[in] layer pointer to layer
     */
    virtual bool removeLayer(Layer* layer) = 0;

    /**
     * \brief Remove surface from scene.
     * \ingroup SceneAPI
     * \param[in] surface pointer to surface
     */
    virtual bool removeSurface(Surface* surface) = 0;

    /**
     * \brief Get a layer of the scene by id.
     * \ingroup SceneAPI
     * \param[in] id id of the layer
     * \return pointer to the layer with id
     */
    virtual Layer* getLayer(const uint id) const = 0;

    /**
     * \brief Get a surface of the scene by id.
     * \ingroup SceneAPI
     * \param[in] id id of the surface
     * \return pointer to the surface with id
     */
    virtual Surface* getSurface(const uint id) const = 0;

    /**
     * \brief Get a surface group by id
     * \ingroup SceneAPI
     * \param[in] id id of surface group
     * \return pointer to surface group with id
     */
    virtual SurfaceGroup* getSurfaceGroup(const uint id) const = 0;

    /**
     * \brief Get a layer group by id
     * \ingroup SceneAPI
     * \param[in] id id of the layer group
     * \return pointer to the layer group with id
     */
    virtual LayerGroup* getLayerGroup(const uint id) const = 0;

    /**
     * \brief Get list of ids of all layers currently existing.
     * \ingroup SceneAPI
     * \param[out] length length of array returned in array
     * \param[out] array array containing the ids of all layers
     * \return list of ids of all currently know layers
     */
    virtual void getLayerIDs(uint* length, uint** array) const = 0;

    /**
     * \brief Get list of ids of all layers currently existing.
     * \ingroup SceneAPI
     * \param[in] screenID id of screen
     * \param[out] length length of array returned in array
     * \param[out] array array containing the ids of all layers on screen
     * \return list of ids of all currently know layers
     */
    virtual bool getLayerIDsOfScreen(const uint screenID, uint* length, uint** array) const = 0;

    /**
     * \brief Get list of ids of all surfaces currently existing.
     * \ingroup SceneAPI
     * \param[out] length length of array returned in array
     * \param[out] array array containing the ids of all surfaces
     * \return list of ids of all currently know surfaces
     */
    virtual void getSurfaceIDs(uint* length, uint** array) const = 0;

    /**
     * \brief Get list of ids of all layers currently existing.
     * \ingroup SceneAPI
     * \param[out] length length of array returned in array
     * \param[out] array array containing the ids of all layer groups
     * \return list of ids of all currently know layers
     */
    virtual void getLayerGroupIDs(uint* length, uint* array[]) const = 0;

    /**
     * \brief Get list of ids of all layers currently existing.
     * \ingroup SceneAPI
     * \param[out] length length of array returned in array
     * \param[out] array array containing the ids of all surface groups
     * \return list of ids of all currently know layers
     */
    virtual void getSurfaceGroupIDs(uint* length, uint* array[]) const = 0;

    /**
     * \brief Lock the list for read and write access
     * \ingroup SceneAPI
     */
    virtual void lockScene() = 0;

    /**
     * \brief Unlock the list for read and write access
     * \ingroup SceneAPI
     */
    virtual void unlockScene() = 0;

    /**
     * \brief Get the current render order of the scene.
     * \ingroup SceneAPI
     * \return reference to render order
     */
    virtual LayerList& getCurrentRenderOrder() = 0;

    /**
     * \brief Remove a surface group from scene.
     * \ingroup SceneAPI
     * \param[in] surface pointer to surface to be removed
     */
    virtual void removeSurfaceGroup(SurfaceGroup *surface) = 0;

    /**
     * \brief Remove a layer group from scene.
     * \ingroup SceneAPI
     * \param[in] layer pointer to layer group to be removed
     */
    virtual void removeLayerGroup(LayerGroup *layer) = 0;

    /**
     * \brief Get a map of all surface from the scene.
     * \ingroup SceneAPI
     * \return Map holding all surfaces.
     * \todo return SurfaceMap& instead?
     */
    virtual const SurfaceMap getAllSurfaces() const = 0;

    /**
     * \brief Get the topmost, visible surface at screen position x,y in scene.
     * \ingroup SceneAPI
     * \param[in] x x position in screen coordinates.
     * \param[out] x x position in surface coordinates
     * \param[in] y y position in screen coordinates.
     * \param[out] y y position in surface coordinates
     * \param[in] minOpacity minimum opacity for surface to be considered
     * \return pointer to surface
     */
    virtual Surface* getSurfaceAt(unsigned int *x, unsigned int *y, double minOpacity) = 0;

    /**
     * \brief Check, if layer is in render order.
     * \ingroup SceneAPI
     * \param[in] id layer id
     * \return TRUE: layer is in render order
     * \return FALSE: layer is not in render order
     */
    virtual bool isLayerInCurrentRenderOrder(const uint id) = 0;

    bool debugMode;

};

#endif /* _ISCENE_H_ */
