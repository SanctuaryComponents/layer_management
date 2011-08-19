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
#ifndef _SCENE_H_
#define _SCENE_H_

#include "IScene.h"
#include "Layermanager.h"
#include "CommandList.h"
#include "ShaderMap.h"
#include "LayerGroup.h"
#include "LayerGroupMap.h"
#include "SurfaceGroup.h"
#include "SurfaceGroupMap.h"
#include "SurfaceMap.h"
#include "LayerMap.h"
#include "ShaderMap.h"
#include "LayerList.h"
#include <pthread.h>

class Layer;
class Surface;

/*
 * Represents a list of Layers which contain the Surfaces.
 */
class Scene: public IScene
{
    // TODO: these should use public interface instead
    friend class Layermanager;
    friend class CommitCommand;
    friend class CreateShaderCommand;
    friend class DestroyShaderCommand;
    friend class SetShaderCommand;
    friend class SetUniformsCommand;

public:
    Scene();
    ~Scene();

    virtual Layer* createLayer(const uint id);
    virtual Surface *createSurface(const uint id);
    virtual LayerGroup *createLayerGroup(const uint id);
    virtual SurfaceGroup *createSurfaceGroup(const uint id);

    virtual void removeLayer(Layer* layer);
    virtual void removeSurface(Surface* surface);
    virtual Layer* getLayer(const uint id) const;
    virtual Surface* getSurface(const uint id) const;
    virtual SurfaceGroup* getSurfaceGroup(const uint id) const;
    virtual LayerGroup* getLayerGroup(const uint id) const;

    virtual void getLayerIDs(uint* length, uint** array) const;
    virtual bool getLayerIDsOfScreen(const uint screenID, uint* length, uint** array) const;
    virtual void getSurfaceGroupIDs(uint* length, uint** array) const;
    virtual void getLayerGroupIDs(uint* length, uint** array) const;
    virtual void getSurfaceIDs(uint* length, uint** array) const;

    virtual void lockScene();
    virtual void unlockScene();

public:
    // TODO: should be private -> not in ILayerList; clients should use only the interface
    LayerList& getCurrentRenderOrder();
    void removeSurfaceGroup(SurfaceGroup *surface);
    void removeLayerGroup(LayerGroup *layer);
    const SurfaceMap getAllSurfaces() const;
    Surface* getSurfaceAt(unsigned int *x, unsigned int *y, double minOpacity);

private:
    const LayerMap getAllLayers() const;
    void removeLayerFromAllLayerGroups(Layer* layer);
    void removeSurfaceFromAllLayers(Surface* surface);
    void removeSurfaceFromAllSurfaceGroups(Surface* surface);

private:
    CommandList m_toBeCommittedList;
    ShaderMap m_shaderMap;
    pthread_mutex_t m_layerListMutex;
    LayerGroupMap m_layerGroupMap;
    SurfaceGroupMap m_surfaceGroupMap;
    SurfaceMap m_surfaceMap;
    LayerMap m_layerMap;
    LayerList m_currentRenderOrder;
};

inline const LayerMap Scene::getAllLayers() const
{
    return m_layerMap;
}

inline LayerList& Scene::getCurrentRenderOrder() // TODO: const
{
    return m_currentRenderOrder;
}

inline const SurfaceMap Scene::getAllSurfaces() const
{
    return m_surfaceMap;
}

inline void Scene::lockScene()
{
    pthread_mutex_lock(&m_layerListMutex);
}

inline void Scene::unlockScene()
{
    pthread_mutex_unlock(&m_layerListMutex);
}

#endif /* _SCENE_H_ */
