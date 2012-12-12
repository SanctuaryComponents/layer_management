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

#ifndef _GRAPHICALGROUP_H_
#define _GRAPHICALGROUP_H_

#include <list>
#include "GraphicalObject.h"
#include "ObjectType.h"

/**
 * Containerclass grouping objects. Grouping makes it possible to call Commands on multiple targets(the group) at once.
 * @param T defines the element class. (Layergroup has Layer* as element type for instance)
 * @param FIRST_ID a group is parameterized by the first ID given to the group (counting upwards)
 */
template <class T, ObjectType thetype>
class GraphicalGroup : public GraphicalObject
{
public:
    GraphicalGroup(int creatorPid)
    : GraphicalObject(thetype, 1.0, true, creatorPid)
    {
        list = std::list<T*>();
    }

    GraphicalGroup(int externalId, int creatorPid)
    : GraphicalObject(externalId, thetype, 1.0, true, creatorPid)
    {
        list = std::list<T*>();
    }

    /**
     * Set visibility on every element of the group
     */
    virtual bool setVisibility(bool visible)
    {
        bool result = false;
        this->visibility = visible;
        for(typename std::list<T*>::iterator it = list.begin(); it != list.end(); ++it)
        {
            result |= (*it)->setVisibility(visible);
        }
        return result;
    }

    /**
     * Set opacity on every element of the group
     */
    virtual bool setOpacity(double opacity)
    {
        bool result = false;
        this->opacity = opacity;
        for(typename std::list<T*>::iterator it = list.begin(); it != list.end(); ++it)
        {
            result |= (*it)->setOpacity(opacity);
        }
        return result;
    }

    const std::list<T*> getList() const
    {
        return list;
    }

    void addElement(T* element)
    {
        list.push_back(element);
    }

    void removeElement(T* element)
    {
        list.remove(element);
    }

private:

    // the list containing the elements represented by the group
    std::list<T*> list;
};

#endif /* _GRAPHICALGROUP_H_ */
