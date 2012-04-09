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

#ifndef _GRAPHICALOBJECT_H_
#define _GRAPHICALOBJECT_H_

#include "ObjectType.h"

class Shader;

/**
 * Base class of all objects representing graphical objects within the layermanagement.
 */
class GraphicalObject
{
public:

    GraphicalObject(ObjectType type, double opacity, bool visibility);

    GraphicalObject(int externalId,ObjectType type, double opacity, bool visibility);

    virtual ~GraphicalObject() {}

    /**
     * @brief Set alpha value
     * @param[in] newOpacity The new Alpha Value between 0.0 (full transparency) and 1.0 (fully visible)
     * @return TRUE if the new Alpha Value is not equal to the current Alpha Value
     *         FALSE if they are equal
     */
    virtual bool setOpacity(double newOpacity);

    /**
     * @brief Get alpha value
     * @return The current Alpha Value between 0.0 (full transparency) and 1.0 (fully visible)
     */
    double getOpacity() const;

    /**
     * Set the visibility
     * @param[in] newVisibility set this object visible (true) or invisible (false)
     * @return TRUE if the new visiblity value is not equal to the current visibility value
     *         FALSE if they are equal
     */
    virtual bool setVisibility(bool newVisibility);

    bool getVisibility() const;

    /**
     * @brief Get external ID for graphical object
     * @return external id of graphical object
     */
    virtual unsigned int getID();

    /**
     * Assign custom shader for rendering
     *
     * @param[in] s Custom shader. If NULL, default shader will be used.
     * @return TRUE if the new custom shader is different from the current custom shader
     *         FALSE if they are same
     */
    bool setShader(Shader* s);

    /**
     * @brief get the currently assigned custom shader object
     * @return currently assigned custom shader object
     */
    Shader* getShader();

public:
    static const unsigned int INVALID_ID;
    ObjectType type;
    bool renderPropertyChanged;
    bool damaged;

    ///     Pointer to currently assigned shader. If NULL, a default shader will be used.
    Shader* shader;
    double opacity;
    bool visibility;

protected:
    unsigned int graphicInternalId;
    unsigned int graphicExternalId;

private:
    static unsigned int nextGraphicId[TypeMax];
};

inline GraphicalObject::GraphicalObject(ObjectType type, double opacity, bool visibility)
: type(type)
, renderPropertyChanged(false)
, damaged(false)
, shader(0)
, opacity(opacity)
, visibility(visibility)
, graphicInternalId(nextGraphicId[type]++)
{
    graphicExternalId = graphicInternalId;
}

inline GraphicalObject::GraphicalObject(int externalId, ObjectType type, double opacity, bool visibility)
: type(type)
, renderPropertyChanged(false)
, damaged(false)
, shader(0)
, opacity(opacity)
, visibility(visibility)
, graphicInternalId(nextGraphicId[type]++)
, graphicExternalId(externalId)
{
}

inline bool GraphicalObject::setOpacity(double newOpacity)
{
    if (opacity != newOpacity)
    {
        opacity = newOpacity;
        renderPropertyChanged = true;
        return true;
    }
    return false;
}

inline double GraphicalObject::getOpacity() const
{
    return opacity;
}

inline bool GraphicalObject::setVisibility(bool newVisibility)
{
    if (visibility != newVisibility)
    {
        visibility = newVisibility;
        renderPropertyChanged = true;
        return true;
    }
    return false;
}

inline bool GraphicalObject::getVisibility() const
{
    return visibility;
}

inline unsigned int GraphicalObject::getID()
{
    return graphicExternalId;
}

inline bool GraphicalObject::setShader(Shader* s)
{
    if (shader != s)
    {
        shader = s;
        renderPropertyChanged = true;
        return true;
    }
    return false;
}

inline Shader* GraphicalObject::getShader()
{
    return shader;
}


#endif /* _GRAPHICALOBJECT_H_ */
