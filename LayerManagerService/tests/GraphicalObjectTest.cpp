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

#include <gtest/gtest.h>

#include "GraphicalObject.h"

class GraphicalObjectTest : public ::testing::Test
{
public:
    void SetUp()
    {
        m_pGraphicalObject = new GraphicalObject(TypeSurface, 1.0, true);
        ASSERT_TRUE(m_pGraphicalObject);
    }

    void TearDown()
    {
        if (m_pGraphicalObject)
        {
            delete m_pGraphicalObject;
            m_pGraphicalObject = 0;
        }
    }

    GraphicalObject* m_pGraphicalObject;
};

TEST_F(GraphicalObjectTest, DISABLED_construction)
{
    //GraphicalObject(ObjectType type, double opacity, bool visibility);
    //GraphicalObject(int externalId,ObjectType type, double opacity, bool visibility);
    //virtual ~GraphicalObject() {}
}

TEST_F(GraphicalObjectTest, DISABLED_setOpacity)
{

}

TEST_F(GraphicalObjectTest, DISABLED_getOpacity)
{

}

TEST_F(GraphicalObjectTest, DISABLED_setVisibility)
{

}

TEST_F(GraphicalObjectTest, DISABLED_getVisibility)
{

}

TEST_F(GraphicalObjectTest, DISABLED_getID)
{

}

TEST_F(GraphicalObjectTest, DISABLED_setShader)
{

}

TEST_F(GraphicalObjectTest, DISABLED_getShader)
{

}
