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

#ifndef _GRAPHICALSURFACE_H_
#define _GRAPHICALSURFACE_H_

#include "LogicalGraphicsObject.h"
#include "OrientationType.h"
#include "Rectangle.h"
#include "Vector2.h"

/**
 * Abstract Type representing a graphical surface.
 */
class GraphicalSurface : public GraphicalObject
{
public:
    GraphicalSurface(ObjectType type);

	GraphicalSurface(int externalId,ObjectType type);

		/**
		 * Set Orientation value
		 * @param orientation the new value. Multiples of 90 degrees. (0->0°, 1->90°, 2->180°,3->279°)
		 */
		void setOrientation(OrientationType newOrientation);

		OrientationType getOrientation() const;

		/**
		 * Set Source Viewport (only use portion of source graphics)
		 * @param x Horizontal x position within source (clip from the left)
		 * @param y Vertical y position within source (clip from the top)
		 * @param width Width within source (can be used to clip from the right)
		 * @param height Height within source (can be used to clip fromt he bottom)
		 */
		void setSourceRegion(const Rectangle& newSource);

		const Rectangle& getSourceRegion() const;

		/**
		 * Set Destination Viewport (Scale output)
		 * @param x Horizontal x position of destination
		 * @param y Vertical y position of destination
		 * @param width Width of destination
		 * @param height Height of destination
		 */
		void setDestinationRegion(const Rectangle& newDestination);

		void setPosition(const unsigned int& x, const unsigned int& y);

		Vector2 getPosition();
		void setDimension(const unsigned int& width, const unsigned int& height);

		const Rectangle& getDestinationRegion() const;
		Vector2 getDimension();
private:
    OrientationType m_orientation; // Rotation of the graphical content
    Rectangle m_sourceViewport;
    Rectangle m_destinationViewport;

};


inline GraphicalSurface::GraphicalSurface(ObjectType type)
: GraphicalObject(type, 1.0, false)
, m_orientation(Zero)
, m_sourceViewport(0,0,0,0)
, m_destinationViewport(0,0,0,0)
{
}

inline GraphicalSurface::GraphicalSurface(int externalId,ObjectType type)
: GraphicalObject(externalId,type,1.0,false)
, m_orientation(Zero)
, m_sourceViewport(0,0,0,0)
, m_destinationViewport(0,0,0,0)
{
}

inline void GraphicalSurface::setOrientation(OrientationType newOrientation)
{
	if (m_orientation != newOrientation)
	{
		renderPropertyChanged = true;
	}
	m_orientation = newOrientation;
}

inline OrientationType GraphicalSurface::getOrientation() const
{
    return m_orientation;
}

inline void GraphicalSurface::setSourceRegion(const Rectangle& newSource)
{
	if (!(m_sourceViewport == newSource))
	{
		renderPropertyChanged = true;
	}
	m_sourceViewport = newSource;}

inline const Rectangle& GraphicalSurface::getSourceRegion() const
{
    return m_sourceViewport;
}

inline void GraphicalSurface::setDestinationRegion(const Rectangle& newDestination)
{
	if (!(m_destinationViewport == newDestination))
	{
		renderPropertyChanged = true;
	}
	m_destinationViewport = newDestination;}

inline void GraphicalSurface::setPosition(const unsigned int& x, const unsigned int& y)
{
	if (m_destinationViewport.x != x || m_destinationViewport.y != y)
	{
		renderPropertyChanged = true;
	}
	m_destinationViewport.x = x; m_destinationViewport.y = y;
}

inline Vector2 GraphicalSurface::getPosition()
{
    return Vector2(m_destinationViewport.x,m_destinationViewport.y);
}

inline void GraphicalSurface::setDimension(const unsigned int& width, const unsigned int& height)
{
	if (m_destinationViewport.width != width || m_destinationViewport.height != height)
	{
		renderPropertyChanged = true;
	}
	m_destinationViewport.width = width; m_destinationViewport.height = height;
}

inline const Rectangle& GraphicalSurface::getDestinationRegion() const
{
    return m_destinationViewport;
}

inline Vector2 GraphicalSurface::getDimension()
{
    return Vector2(m_destinationViewport.width, m_destinationViewport.height);
}

#endif /* _GRAPHICALSURFACE_H_ */
