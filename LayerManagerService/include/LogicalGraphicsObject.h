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

#ifndef _LOGICALGRAPHICSOBJECT_H_
#define _LOGICALGRAPHICSOBJECT_H_

#include "ObjectType.h"

class Shader;
/**
 * Base class of all objects representing graphical objects within the layermanagement.
 */
class GraphicalObject{
public:

	GraphicalObject(ObjectType type, double opacity, bool visibility) : type(type),id(NextID++),shader(0),opacity(opacity),visibility(visibility)
	{};

	/**
	 * Set alpha value
	 * @param alpha The new Alpha Value between 0.0 (full transparency) and 1.0 (fully visible)
	 */
	virtual void setOpacity(double newOpacity){opacity = newOpacity;};
	double getOpacity(){return opacity;};

	/**
	 * Set the visibility
	 * @param visible set this object visible (true) or invisible (false)
	 */
	virtual void setVisibility(bool newVisibility){visibility = newVisibility;};
	bool getVisibility(){ return visibility;};

	int getID() const {return id;};

	/**
	 * Assign custom shader for rendering
	 *
	 * @param s   Custom shader. If NULL, default shader will be used.
	 */
	void setShader(Shader* s)
	{
		shader = s;
	}

	/**
	 * @return currently assigned custom shader object
	 */
	Shader* getShader()
	{
		return shader;
	}

	ObjectType type;

public:
	/**
	 * ID of this graphical object (Layerid, surfaceid etc)
	 */
	int id;
	/**
	 * Pointer to currently assigned shader. If NULL, a default shader will be used.
	 */
	Shader* shader;
	double opacity;
	bool visibility;
	static int NextID;
};

#endif /* _LOGICALGRAPHICSOBJECT_H_ */
