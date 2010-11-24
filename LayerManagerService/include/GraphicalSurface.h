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

#ifndef _GRAPHICALSURFACE_H_
#define _GRAPHICALSURFACE_H_

#include "LogicalGraphicsObject.h"
#include "Rectangle.h"
#include "Vector2.h"
#include "Orientation.h"

/**
 * Abstract Type representing a graphical surface.
 */
class GraphicalSurface : public GraphicalObject{
public:
	GraphicalSurface(ObjectType type) : GraphicalObject(type,1.0,true),orientation(Zero),SourceViewport(0,0,0,0),DestinationViewport(0,0,0,0)  {
	};

	GraphicalSurface(int externalId,ObjectType type) : GraphicalObject(externalId,type,1.0,true),orientation(Zero),SourceViewport(0,0,0,0),DestinationViewport(0,0,0,0)  {
        };
		/**
		 * Set Orientation value
		 * @param orientation the new value. Multiples of 90 degrees. (0->0°, 1->90°, 2->180°,3->279°)
		 */
		void setOrientation(OrientationType newOrientation){orientation = newOrientation;};
		const OrientationType getOrientation(){return orientation;};

		/**
		 * Set Source Viewport (only use portion of source graphics)
		 * @param x Horizontal x position within source (clip from the left)
		 * @param y Vertical y position within source (clip from the top)
		 * @param width Width within source (can be used to clip from the right)
		 * @param height Height within source (can be used to clip fromt he bottom)
		 */
		void setSourceRegion(const Rectangle& newSource){SourceViewport = newSource;};
		const Rectangle& getSourceRegion(){ return SourceViewport;};

		/**
		 * Set Destination Viewport (Scale output)
		 * @param x Horizontal x position of destination
		 * @param y Vertical y position of destination
		 * @param width Width of destination
		 * @param height Height of destination
		 */
		void setDestinationRegion(const Rectangle& newDestination){DestinationViewport = newDestination;};

		void setPosition(const int& x, const int& y){DestinationViewport.x = x; DestinationViewport.y = y;};
		Vector2 getPosition(){ return Vector2(DestinationViewport.x,DestinationViewport.y);}
		void setDimension(const int& width, const int& height){DestinationViewport.width = width; DestinationViewport.height = height;};
		const Rectangle& getDestinationRegion(){ return DestinationViewport;};
		Vector2 getDimension(){ return Vector2(DestinationViewport.width, DestinationViewport.height);};
private:
	OrientationType orientation; // Rotation of the graphical content
	Rectangle SourceViewport;
	Rectangle DestinationViewport;

};

#endif /* _GRAPHICALSURFACE_H_ */
