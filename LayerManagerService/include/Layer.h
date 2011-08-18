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

    // original dimensions
    // needed for calculations when using a source viewport
    int OriginalSourceWidth;
    int OriginalSourceHeight;

private:
    SurfaceList m_surfaces;
    LayerType m_layerType;
    uint m_capabilities;
};

inline Layer::Layer()
: GraphicalSurface(TypeLayer)
, OriginalSourceWidth(0)
, OriginalSourceHeight(0)
, m_layerType(Software_2D)
, m_capabilities(0)
{
}

inline Layer::Layer(int id)
: GraphicalSurface(id, TypeLayer)
, OriginalSourceWidth(0)
, OriginalSourceHeight(0)
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
	renderPropertyChanged = true;
    m_surfaces.push_back(s);
}

inline void Layer::removeSurface(Surface* s)
{
	renderPropertyChanged = true;
    m_surfaces.remove(s);
}

inline SurfaceList& Layer::getAllSurfaces()
{
    return m_surfaces;
}

#endif /* _LAYER_H_ */
