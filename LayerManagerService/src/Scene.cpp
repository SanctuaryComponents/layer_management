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

#include "LayerList.h"
#include "Layermanager.h"
#include "Log.h"

#include "Layer.h"
#include "GraphicalGroup.h"
#include "Shader.h"

Scene::Scene()
{
    pthread_mutex_init(&m_layerListMutex, NULL);
}

Scene::~Scene()
{
    pthread_mutex_destroy(&m_layerListMutex);
}

Layer* Scene::createLayer(const uint layerId)
{
    Layer* newLayer = NULL;
    if (layerId == GraphicalObject::INVALID_ID)
    {
        LOG_DEBUG("Scene", "createLayer: creating new layer");
        newLayer = new Layer();
        uint newLayerId = newLayer->getID();
        m_layerMap[newLayerId] = newLayer;
    }
    else
    {
        if (0 == m_layerMap.count(layerId))
        {
            LOG_DEBUG("Scene", "createLayer: unknown layer id provided; creating new layer");
            newLayer = new Layer(layerId);
            uint newLayerId = newLayer->getID();
            m_layerMap[newLayerId] = newLayer;
        }
        else
        {
            LOG_DEBUG("Scene","createLayer: Layer with id " << layerId << " already exists ");
            newLayer = m_layerMap.at(layerId);
        }
    }
    return newLayer;
}

Surface* Scene::createSurface(const uint surfaceId)
{
    Surface* newSurface = NULL;
    if (surfaceId == GraphicalObject::INVALID_ID)
    {
        newSurface = new Surface();
        uint newSurfaceId = newSurface->getID();
        m_surfaceMap[newSurfaceId] = newSurface;
    }
    else
    {
        if (0 == m_surfaceMap.count(surfaceId))
        {
            newSurface = new Surface(surfaceId);
            uint newSurfaceId = newSurface->getID();
            m_surfaceMap[newSurfaceId] = newSurface;
        }
        else
        {
            LOG_DEBUG("Scene","Surface with id [ " << surfaceId << " ] " << " already exists " );
        }
    }
    return newSurface;
}

SurfaceGroup* Scene::createSurfaceGroup(const uint surfaceGroupId)
{
    SurfaceGroup* newSurfaceGroup = NULL;
    if (surfaceGroupId == GraphicalObject::INVALID_ID)
    {
        newSurfaceGroup = new SurfaceGroup();
        uint newSurfaceGroupId = newSurfaceGroup->getID();
        m_surfaceGroupMap[newSurfaceGroupId] = newSurfaceGroup;
    }
    else
    {
        if (0 == m_surfaceGroupMap.count(surfaceGroupId))
        {
            newSurfaceGroup = new SurfaceGroup(surfaceGroupId);
            uint newSurfaceGroupId = newSurfaceGroup->getID();
            m_surfaceGroupMap[newSurfaceGroupId] = newSurfaceGroup;
        }
        else
        {
            LOG_DEBUG("Scene","SurfaceGroup with id [ " << surfaceGroupId << " ] " << " already exists ");
        }
    }
    return newSurfaceGroup;
}

LayerGroup* Scene::createLayerGroup(const uint layerGroupId)
{
    LayerGroup* newLayerGroup = NULL;
    if (layerGroupId == GraphicalObject::INVALID_ID)
    {
        newLayerGroup = new LayerGroup();
        uint newLayerGroupId = newLayerGroup->getID();
        m_layerGroupMap[newLayerGroupId] = newLayerGroup;
    }
    else
    {
        if (0 == m_layerGroupMap.count(layerGroupId))
        {
            newLayerGroup = new LayerGroup(layerGroupId);
            uint newLayerGroupId = newLayerGroup->getID();
            m_layerGroupMap[newLayerGroupId] = newLayerGroup;
        }
        else
        {
            LOG_DEBUG("Scene","LayerGroup with id [ " << layerGroupId << " ] " << " already exists " );
        }
    }
    return newLayerGroup;
}

Layer* Scene::getLayer(const uint layerId) const
{
    Layer* layer = NULL;
    if (m_layerMap.count(layerId) > 0)
    {
        layer = m_layerMap.at(layerId);
    }
    else
    {
        LOG_WARNING("Scene","layer not found : id [ " << layerId << " ]");
    }
    return layer;
}

Surface* Scene::getSurface(const uint surfaceId) const
{
    Surface* surface = NULL;
    if (m_surfaceMap.count(surfaceId) > 0)
    {
        surface = m_surfaceMap.at(surfaceId);
    }
    else
    {
        LOG_WARNING("Scene","surface not found : id [ " << surfaceId << " ]");
    }
    return surface;
}

SurfaceGroup* Scene::getSurfaceGroup(const uint surfaceGroupId) const
{
    SurfaceGroup* surfaceGroup = NULL;
    if (m_surfaceGroupMap.count(surfaceGroupId) > 0)
    {
        surfaceGroup = m_surfaceGroupMap.at(surfaceGroupId);
    }
    else
    {
        LOG_WARNING("Scene","SurfaceGroup not found : id [ " << surfaceGroupId << " ]");
    }
    return surfaceGroup;
}

LayerGroup* Scene::getLayerGroup(const uint layerGroupId) const
{
    LayerGroup* layerGroup = NULL;
    if (m_layerGroupMap.count(layerGroupId) > 0)
    {
        layerGroup = m_layerGroupMap.at(layerGroupId);
    }
    else
    {
        LOG_WARNING("Scene","LayerGroup not found : id [ " << layerGroupId << " ]");
    }
    return layerGroup;
}

/// \brief remove layer from all layergroups it might be part of
void Scene::removeLayerFromAllLayerGroups(Layer* layer)
{
    LayerGroupMapIterator iter = m_layerGroupMap.begin();
    LayerGroupMapIterator iterEnd = m_layerGroupMap.end();

    for (; iter != iterEnd; ++iter)
    {
        LayerGroup *lg = iter->second;
        lg->removeElement(layer);
    }
}

/// \brief take layer out of list of layers
void Scene::removeLayer(Layer* layer)
{
    if (layer != NULL)
    {
        m_currentRenderOrder.remove(layer);
        m_layerMap.erase(layer->getID());
        removeLayerFromAllLayerGroups(layer);
        delete layer;
    }
}

/// \brief remove surface from all surfacegroups it might be part of
void Scene::removeSurfaceFromAllSurfaceGroups(Surface* surface)
{
    SurfaceGroupMapIterator iter = m_surfaceGroupMap.begin();
    SurfaceGroupMapIterator iterEnd = m_surfaceGroupMap.end();

    for (; iter != iterEnd; ++iter)
    {
        SurfaceGroup *sg = iter->second;
        sg->removeElement(surface);
    }
}

/// \brief remove surface from all layers it might be on
void Scene::removeSurfaceFromAllLayers(Surface* surface)
{
    LayerMapIterator iter = m_layerMap.begin();
    LayerMapIterator iterEnd = m_layerMap.end();

    for (; iter != iterEnd; ++iter)
    {
        Layer *l = iter->second;
        l->removeSurface(surface);
    }
}

/// \brief take surface out of list of surfaces
void Scene::removeSurface(Surface* surface)
{
    if (surface != NULL)
    {
        uint surfaceId = surface->getID();

        m_surfaceMap.erase(surfaceId);

        removeSurfaceFromAllLayers(surface);
        removeSurfaceFromAllSurfaceGroups(surface);

        delete surface;
    }
}

void Scene::removeLayerGroup(LayerGroup* layerGroup)
{
    if (layerGroup != NULL)
    {
        uint layerGroupId = layerGroup->getID();
        m_layerGroupMap.erase(layerGroupId);
        delete layerGroup;
    }
}

void Scene::removeSurfaceGroup(SurfaceGroup* surfaceGroup)
{
    if (surfaceGroup != NULL)
    {
        uint surfaceGroupId = surfaceGroup->getID();
        m_surfaceGroupMap.erase(surfaceGroupId);
        delete surfaceGroup;
    }
}

void Scene::getLayerIDs(uint* length, uint** array) const
{
    uint numOfLayers = m_layerMap.size();
    uint arrayPos = 0;

    *length = numOfLayers;
    *array = new uint[numOfLayers]; // TODO: safe, if size = 0?

    LayerMapConstIterator iter = m_layerMap.begin();
    LayerMapConstIterator iterEnd = m_layerMap.end();

    for (; iter != iterEnd; ++iter)
    {
        (*array)[arrayPos] = (*iter).first;
        arrayPos++;
    }
}

bool Scene::getLayerIDsOfScreen(const uint screenID, uint* length,
        uint** array) const
{
    // check if screen is valid, currently all layers are only on one screen
    if (screenID != 0) // TODO: remove hard-coded value
    {
        return false;
    }

    uint numOfLayers = m_currentRenderOrder.size();
    uint arrayPos = 0;

    *length = numOfLayers;
    *array = new uint[numOfLayers]; // TODO: safe, if size = 0?

    LayerListConstIterator iter = m_currentRenderOrder.begin();
    LayerListConstIterator iterEnd = m_currentRenderOrder.end();

    for (; iter != iterEnd; ++iter)
    {
        (*array)[arrayPos] = (*iter)->getID();
        ++arrayPos;
    }
    return true;
}

void Scene::getSurfaceIDs(uint* length, uint** array) const
{
    uint numOfSurfaces = m_surfaceMap.size();
    uint arrayPos = 0;

    *length = numOfSurfaces;
    *array = new uint[numOfSurfaces]; // TODO: safe, if size = 0?

    SurfaceMapConstIterator iter = m_surfaceMap.begin();
    SurfaceMapConstIterator iterEnd = m_surfaceMap.end();

    for (; iter != iterEnd; ++iter)
    {
        (*array)[arrayPos] = (*iter).first;
        ++arrayPos;
    }
}

void Scene::getSurfaceGroupIDs(uint* length, uint** array) const
{
    uint numOfSurfaceGroups = m_surfaceGroupMap.size();
    uint arrayPos = 0;

    *length = numOfSurfaceGroups;
    *array = new uint[numOfSurfaceGroups]; // TODO: safe, if size = 0?

    SurfaceGroupMapConstIterator iter = m_surfaceGroupMap.begin();
    SurfaceGroupMapConstIterator iterEnd = m_surfaceGroupMap.end();

    for (; iter != iterEnd; ++iter)
    {
        (*array)[arrayPos] = (*iter).first;
        ++arrayPos;
    }
}

void Scene::getLayerGroupIDs(uint* length, uint** array) const
{
    uint numOfLayergroups = m_layerGroupMap.size();
    uint arrayPos = 0;

    *length = numOfLayergroups;
    *array = new uint[numOfLayergroups]; // TODO: safe, if size = 0?

    LayerGroupMapConstIterator iter = m_layerGroupMap.begin();
    LayerGroupMapConstIterator iterEnd = m_layerGroupMap.end();

    for (; iter != iterEnd; ++iter)
    {
        (*array)[arrayPos] = (*iter).first;
        ++arrayPos;
    }
}

/**
 * @Description:
 * Return the first Surface located below a specific coordinate, and for which
 * the opacity is above a certain level. Also translate the input coordinates
 * which are display wide into surface wide coordinates.
 *
 * This function is mainly used to dispatch mouse events to the underlying
 * window. For this, we need to know to what is the layer / surface under the
 * (x,y) mouse pointer.
 *
 *
 *
 * @param x in/out : IN    x position in the scene
 *                   OUT   x position in the surface coordinate system
 * @param y in/out : IN    y position in the scene
 *                   OUT   y position in the surface coordinate system
 * @param minOpacity Minimal opacity that a surface should have to be elected
 */
Surface* Scene::getSurfaceAt(unsigned int *x, unsigned int *y, double minOpacity)
{
	Surface* surf;
	SurfaceListConstIterator currentSurf;
	LayerListConstReverseIterator currentLayer;
	unsigned int x_SurfCoordinate, y_SurfCoordinate;

	surf = NULL;

	/* Need to browse for all layers. 1st layer of m_currentRenderOrder is rendered
	 * on bottom, last one is rendrered on top. So we have to reverse iterate */
	for (currentLayer = m_currentRenderOrder.rbegin();
	     currentLayer != m_currentRenderOrder.rend() && surf == NULL;
	     currentLayer++)
	{
		if ( ((*currentLayer)->visibility) && ((*currentLayer)->getOpacity() >= minOpacity) )
		{
			if ((*currentLayer)->isInside(*x, *y))
			{
				x_SurfCoordinate = *x;
				y_SurfCoordinate = *y;
				(*currentLayer)->DestToSourceCoordinates(&x_SurfCoordinate, &y_SurfCoordinate, false);
				/* Need to browse for all surfaces */
				for (currentSurf = (*currentLayer)->getAllSurfaces().begin();
				     currentSurf != (*currentLayer)->getAllSurfaces().end() && surf == NULL;
				     currentSurf++)
				{
					if ( ((*currentSurf)->visibility) && ((*currentSurf)->getOpacity() >= minOpacity) )
					{
						if ((*currentSurf)->isInside(x_SurfCoordinate, y_SurfCoordinate))
						{
							surf = *currentSurf;
							(*currentSurf)->DestToSourceCoordinates(&x_SurfCoordinate, &y_SurfCoordinate, false);
							*x = x_SurfCoordinate;
							*y = y_SurfCoordinate;
						}
					}
				}
			}
		}
	}
	return surf;
}

