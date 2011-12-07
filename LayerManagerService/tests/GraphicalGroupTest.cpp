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

#include "GraphicalGroup.h"
#include "Layer.h"

class GraphicalGroupTest : public ::testing::Test
{
public:
    void SetUp()
    {
        m_pGraphicalGroup = new GraphicalGroup<Layer, TypeLayer>();
        ASSERT_TRUE(m_pGraphicalGroup);
    }

    void TearDown()
    {
        if (m_pGraphicalGroup)
        {
            delete m_pGraphicalGroup;
            m_pGraphicalGroup = 0;
        }
    }

    GraphicalGroup<Layer, TypeLayer>* m_pGraphicalGroup;
};

TEST_F(GraphicalGroupTest, DISABLED_defaultConstructor)
{

}

TEST_F(GraphicalGroupTest, DISABLED_specialConstructor)
{

}

TEST_F(GraphicalGroupTest, DISABLED_setVisibility)
{

}

TEST_F(GraphicalGroupTest, DISABLED_setOpacity)
{

}

TEST_F(GraphicalGroupTest, DISABLED_getList)
{

}

TEST_F(GraphicalGroupTest, DISABLED_addElement)
{

}

TEST_F(GraphicalGroupTest, DISABLED_removeElement)
{

}

TEST_F(GraphicalGroupTest, DISABLED_getShader)
{

}
