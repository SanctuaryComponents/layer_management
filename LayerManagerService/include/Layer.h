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

#ifndef _LAYER_H_
#define _LAYER_H_

#include "SurfaceList.h"
#include "Surface.h"
#include "GraphicalSurface.h"
#include "LayerType.h"
#include "LayerCapabilities.h"
#include "SurfaceList.h"

/*
 * A graphical Layerobject which contains Surfaces.
 */
class Layer: public GraphicalSurface
{
    friend class Scene;

public:
    Layer();
    Layer(int id);

    LayerType getLayerType() const;
    void setLayerType(LayerType lt);
    void setLayerCapabilities(unsigned int newCapabilities);
    unsigned int getCapabilities() const;
    void addSurface(Surface* s);
    void removeSurface(Surface* s);
    SurfaceList& getAllSurfaces();
    void removeAllSurfaces();

private:
    SurfaceList m_surfaces;
    LayerType m_layerType;
    uint m_capabilities;
};

inline Layer::Layer()
: GraphicalSurface(TypeLayer)
, m_layerType(Software_2D)
, m_capabilities(0)
{
}

inline Layer::Layer(int id)
: GraphicalSurface(id, TypeLayer)
, m_layerType(Software_2D)
, m_capabilities(0)
{
}

inline LayerType Layer::getLayerType() const
{
    return m_layerType;
}

inline void Layer::setLayerType(LayerType lt)
{
	if (m_layerType != lt)
	{
		renderPropertyChanged = true;
	}
    m_layerType = lt;
}

inline void Layer::setLayerCapabilities(uint newCapabilities)
{
	if (m_capabilities != newCapabilities)
	{
		renderPropertyChanged = true;
	}
    m_capabilities = newCapabilities;
}

inline uint Layer::getCapabilities() const
{
    return m_capabilities;
}

inline void Layer::addSurface(Surface* s)
{
    if (s->getContainingLayerId() == INVALID_ID)
    {
        m_surfaces.push_back(s);
        s->setContainingLayerId(getID());
        renderPropertyChanged = true;
    }
}

inline void Layer::removeSurface(Surface* s)
{
    if (s->getContainingLayerId() == getID())
    {
        m_surfaces.remove(s);
        s->setContainingLayerId(INVALID_ID);
        renderPropertyChanged = true;
    }
}

inline SurfaceList& Layer::getAllSurfaces()
{
    return m_surfaces;
}

inline void Layer::removeAllSurfaces()
{
    SurfaceListConstIterator iter = m_surfaces.begin();
    SurfaceListConstIterator iterEnd = m_surfaces.end();
    for (; iter != iterEnd; ++iter)
    {
        (*iter)->setContainingLayerId(GraphicalObject::INVALID_ID);
    }
    m_surfaces.clear();
    renderPropertyChanged = true;
}

#endif /* _LAYER_H_ */
