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
#include "LayerList.h"
#include "Shader.h"

class GraphicalGroupTest : public ::testing::Test
{
public:
    void SetUp()
    {
        m_pLayerGroup = new GraphicalGroup<Layer, TypeLayer>(0);
        m_pSurfaceGroup = new GraphicalGroup<Surface, TypeSurface>(0);
        ASSERT_TRUE(m_pLayerGroup);
        ASSERT_TRUE(m_pSurfaceGroup);
    }

    void TearDown()
    {
        if (m_pLayerGroup)
        {
            delete m_pLayerGroup;
            m_pLayerGroup = 0;
        }
        if (m_pSurfaceGroup)
        {
            delete m_pSurfaceGroup;
            m_pSurfaceGroup = 0;
        }
    }

    GraphicalGroup<Layer, TypeLayer>* m_pLayerGroup;
    GraphicalGroup<Surface, TypeSurface>* m_pSurfaceGroup;
};

TEST_F(GraphicalGroupTest, defaultConstructor_Layer)
{
    GraphicalGroup<Layer, TypeLayer>* pLayerGroup = 0;

    /// create graphical group of type layer (= layer group)
    pLayerGroup = new GraphicalGroup<Layer, TypeLayer>(0);

    /// make sure, layer group was created
    ASSERT_TRUE(pLayerGroup);

    /// make sure, layer group is initially empty
    EXPECT_EQ(0u, pLayerGroup->getList().size());

    // cleanup
    if (pLayerGroup)
    {
        delete pLayerGroup;
    }
}

TEST_F(GraphicalGroupTest, defaultConstructor_Surface)
{
    GraphicalGroup<Surface, TypeSurface>* pSurfaceGroup = 0;

    /// create graphical group of type surface (= surface group)
    pSurfaceGroup = new GraphicalGroup<Surface, TypeSurface>(0);

    /// make sure, surface group was created
    ASSERT_TRUE(pSurfaceGroup);

    /// make sure, surface group is initially empty
    EXPECT_EQ(0u, pSurfaceGroup->getList().size());

    // cleanup
    if (pSurfaceGroup)
    {
        delete pSurfaceGroup;
    }
}

TEST_F(GraphicalGroupTest, specialConstructor_Layer)
{
    unsigned int expectedId = 433;

    GraphicalGroup<Layer, TypeLayer>* pLayerGroup = 0;

    /// create graphical group of type layer (= layer group) with expected id
    pLayerGroup = new GraphicalGroup<Layer, TypeLayer>(expectedId);

    /// make sure, layer group was created
    ASSERT_TRUE(pLayerGroup);

    /// make sure, layer group has expected id
    EXPECT_EQ(expectedId, pLayerGroup->getID());

    /// make sure, layer group is initially empty
    EXPECT_EQ(0u, pLayerGroup->getList().size());

    // cleanup
    if (pLayerGroup)
    {
        delete pLayerGroup;
    }
}

TEST_F(GraphicalGroupTest, specialConstructor_Surface)
{
    unsigned int expectedId = 436;

    GraphicalGroup<Surface, TypeSurface>* pSurfaceGroup = 0;

    /// create graphical group of type surface (= surface group) with expected id
    pSurfaceGroup = new GraphicalGroup<Surface, TypeSurface>(expectedId);

    /// make sure, surface group was created
    ASSERT_TRUE(pSurfaceGroup);

    /// make sure, surface group has expected id
    EXPECT_EQ(expectedId, pSurfaceGroup->getID());

    /// make sure, surface group is initially empty
    EXPECT_EQ(0u, pSurfaceGroup->getList().size());

    // cleanup
    if (pSurfaceGroup)
    {
        delete pSurfaceGroup;
    }
}

TEST_F(GraphicalGroupTest, setVisibility_Layer)
{
    /// create 3 layers
    Layer l1(0), l2(0), l3(0);

    /// set 2 layers to visible, one to invisible
    l1.setVisibility(true);
    l2.setVisibility(false);
    l3.setVisibility(true);

    /// make sure, two layers are visible, one is not
    EXPECT_TRUE(l1.getVisibility());
    EXPECT_FALSE(l2.getVisibility());
    EXPECT_TRUE(l3.getVisibility());

    /// add the 3 layers to layer group
    m_pLayerGroup->addElement(&l1);
    m_pLayerGroup->addElement(&l2);
    m_pLayerGroup->addElement(&l3);

    /// set layer group to invisible
    m_pLayerGroup->setVisibility(false);

    /// make sure, all layers are invisible
    EXPECT_FALSE(l1.getVisibility());
    EXPECT_FALSE(l2.getVisibility());
    EXPECT_FALSE(l3.getVisibility());

    /// set layer group to visible
    m_pLayerGroup->setVisibility(true);

    /// make sure, all layers are visible
    EXPECT_TRUE(l1.getVisibility());
    EXPECT_TRUE(l2.getVisibility());
    EXPECT_TRUE(l3.getVisibility());

    /// remove 2 layers from layer group
    m_pLayerGroup->removeElement(&l1);
    m_pLayerGroup->removeElement(&l2);

    /// set layer group to invisible
    m_pLayerGroup->setVisibility(false);

    /// make sure, only the visibility of one layer was changed
    EXPECT_TRUE(l1.getVisibility());
    EXPECT_TRUE(l2.getVisibility());
    EXPECT_FALSE(l3.getVisibility());
}

TEST_F(GraphicalGroupTest, setVisibility_Surface)
{
    /// create 3 surfaces
    Surface l1(0), l2(0), l3(0);

    /// set 2 surfaces to visible, one to invisible
    l1.setVisibility(true);
    l2.setVisibility(false);
    l3.setVisibility(true);

    /// make sure, two surfaces are visible, one is not
    EXPECT_TRUE(l1.getVisibility());
    EXPECT_FALSE(l2.getVisibility());
    EXPECT_TRUE(l3.getVisibility());

    /// add the 3 surfaces to surface group
    m_pSurfaceGroup->addElement(&l1);
    m_pSurfaceGroup->addElement(&l2);
    m_pSurfaceGroup->addElement(&l3);

    /// set surface group to invisible
    m_pSurfaceGroup->setVisibility(false);

    /// make sure, all surfaces are invisible
    EXPECT_FALSE(l1.getVisibility());
    EXPECT_FALSE(l2.getVisibility());
    EXPECT_FALSE(l3.getVisibility());

    /// set surface group to visible
    m_pSurfaceGroup->setVisibility(true);

    /// make sure, all surfaces are visible
    EXPECT_TRUE(l1.getVisibility());
    EXPECT_TRUE(l2.getVisibility());
    EXPECT_TRUE(l3.getVisibility());

    /// remove 2 surfaces from surface group
    m_pSurfaceGroup->removeElement(&l1);
    m_pSurfaceGroup->removeElement(&l2);

    /// set surface group to invisible
    m_pSurfaceGroup->setVisibility(false);

    /// make sure, only the visibility of one surface was changed
    EXPECT_TRUE(l1.getVisibility());
    EXPECT_TRUE(l2.getVisibility());
    EXPECT_FALSE(l3.getVisibility());
}

TEST_F(GraphicalGroupTest, setOpacity_Layer)
{
    /// create 3 layers
    Layer l1(0), l2(0), l3(0);

    /// set different opacity for each of the 3 layers
    l1.setOpacity(0.3);
    l2.setOpacity(0.6);
    l3.setOpacity(0.9);

    /// make sure, all layers have expected opacity
    EXPECT_DOUBLE_EQ(0.3, l1.getOpacity());
    EXPECT_DOUBLE_EQ(0.6, l2.getOpacity());
    EXPECT_DOUBLE_EQ(0.9, l3.getOpacity());

    /// add the 3 layers to layer group
    m_pLayerGroup->addElement(&l1);
    m_pLayerGroup->addElement(&l2);
    m_pLayerGroup->addElement(&l3);

    /// set layer group to expected opacity
    m_pLayerGroup->setOpacity(0.5);

    /// make sure, all layers have expected opacity now
    EXPECT_DOUBLE_EQ(0.5, l1.getOpacity());
    EXPECT_DOUBLE_EQ(0.5, l2.getOpacity());
    EXPECT_DOUBLE_EQ(0.5, l3.getOpacity());

    /// remove 2 layers from layer group
    m_pLayerGroup->removeElement(&l1);
    m_pLayerGroup->removeElement(&l2);

    /// set opacity of layer group to different value
    m_pLayerGroup->setOpacity(0.8);

    /// make sure, only the opacity of one layer was changed
    EXPECT_DOUBLE_EQ(0.5, l1.getOpacity());
    EXPECT_DOUBLE_EQ(0.5, l2.getOpacity());
    EXPECT_DOUBLE_EQ(0.8, l3.getOpacity());
}

TEST_F(GraphicalGroupTest, setOpacity_Surface)
{
    /// create 3 surfaces
    Surface l1(0), l2(0), l3(0);

    /// set different opacity for each of the 3 surfaces
    l1.setOpacity(0.3);
    l2.setOpacity(0.6);
    l3.setOpacity(0.9);

    /// make sure, all surfaces have expected opacity
    EXPECT_DOUBLE_EQ(0.3, l1.getOpacity());
    EXPECT_DOUBLE_EQ(0.6, l2.getOpacity());
    EXPECT_DOUBLE_EQ(0.9, l3.getOpacity());

    /// add the 3 surfaces to surface group
    m_pSurfaceGroup->addElement(&l1);
    m_pSurfaceGroup->addElement(&l2);
    m_pSurfaceGroup->addElement(&l3);

    /// set surface group to expected opacity
    m_pSurfaceGroup->setOpacity(0.5);

    /// make sure, all surfaces have expected opacity now
    EXPECT_DOUBLE_EQ(0.5, l1.getOpacity());
    EXPECT_DOUBLE_EQ(0.5, l2.getOpacity());
    EXPECT_DOUBLE_EQ(0.5, l3.getOpacity());

    /// remove 2 surfaces from surface group
    m_pSurfaceGroup->removeElement(&l1);
    m_pSurfaceGroup->removeElement(&l2);

    /// set opacity of surface group to different value
    m_pSurfaceGroup->setOpacity(0.8);

    /// make sure, only the opacity of one surface was changed
    EXPECT_DOUBLE_EQ(0.5, l1.getOpacity());
    EXPECT_DOUBLE_EQ(0.5, l2.getOpacity());
    EXPECT_DOUBLE_EQ(0.8, l3.getOpacity());
}

TEST_F(GraphicalGroupTest, getList_Layer)
{
    /// make sure, list is empty
    EXPECT_EQ(0u, m_pLayerGroup->getList().size());

    /// add 3 layers to graphical group
    Layer l1(0), l2(0), l3(0);
    m_pLayerGroup->addElement(&l1);
    m_pLayerGroup->addElement(&l2);
    m_pLayerGroup->addElement(&l3);

    /// make sure, list not contains 3 elements
    EXPECT_EQ(3u, m_pLayerGroup->getList().size());

    /// remove 2 layers from graphical group
    m_pLayerGroup->removeElement(&l2);
    m_pLayerGroup->removeElement(&l3);

    /// make sure, list not contains 1 element
    EXPECT_EQ(1u, m_pLayerGroup->getList().size());

    /// remove last layers from graphical group
    m_pLayerGroup->removeElement(&l1);

    /// make sure, list is empty
    EXPECT_EQ(0u, m_pLayerGroup->getList().size());
}

TEST_F(GraphicalGroupTest, getList_Surface)
{
    /// make sure, list is empty
    EXPECT_EQ(0u, m_pSurfaceGroup->getList().size());

    /// add 3 surfaces to graphical group
    Surface l1(0), l2(0), l3(0);
    m_pSurfaceGroup->addElement(&l1);
    m_pSurfaceGroup->addElement(&l2);
    m_pSurfaceGroup->addElement(&l3);

    /// make sure, list not contains 3 elements
    EXPECT_EQ(3u, m_pSurfaceGroup->getList().size());

    /// remove 2 surfaces from graphical group
    m_pSurfaceGroup->removeElement(&l2);
    m_pSurfaceGroup->removeElement(&l3);

    /// make sure, list not contains 1 element
    EXPECT_EQ(1u, m_pSurfaceGroup->getList().size());

    /// remove last surfaces from graphical group
    m_pSurfaceGroup->removeElement(&l1);

    /// make sure, list is empty
    EXPECT_EQ(0u, m_pSurfaceGroup->getList().size());
}

TEST_F(GraphicalGroupTest, addElement_Layer)
{
    /// make sure, list is empty
    EXPECT_EQ(0u, m_pLayerGroup->getList().size());

    /// add 1 layer to graphical group
    Layer l1(0);
    m_pLayerGroup->addElement(&l1);

    /// make sure, list not contains 1 element
    EXPECT_EQ(1u, m_pLayerGroup->getList().size());

    /// add 2 layers to graphical group
    Layer l2(0), l3(0);
    m_pLayerGroup->addElement(&l2);
    m_pLayerGroup->addElement(&l3);

    /// make sure, list contains 3 element
    EXPECT_EQ(3u, m_pLayerGroup->getList().size());
}

TEST_F(GraphicalGroupTest, addElement_Surface)
{
    /// make sure, list is empty
    EXPECT_EQ(0u, m_pSurfaceGroup->getList().size());

    /// add 1 surface to graphical group
    Surface l1(0);
    m_pSurfaceGroup->addElement(&l1);

    /// make sure, list not contains 1 element
    EXPECT_EQ(1u, m_pSurfaceGroup->getList().size());

    /// add 2 surfaces to graphical group
    Surface l2(0), l3(0);
    m_pSurfaceGroup->addElement(&l2);
    m_pSurfaceGroup->addElement(&l3);

    /// make sure, list contains 3 element
    EXPECT_EQ(3u, m_pSurfaceGroup->getList().size());
}

TEST_F(GraphicalGroupTest, removeElement_Layer)
{
    /// make sure, list is empty
    EXPECT_EQ(0u, m_pLayerGroup->getList().size());

    /// add 3 layers to graphical group
    Layer l1(0), l2(0), l3(0);
    m_pLayerGroup->addElement(&l1);
    m_pLayerGroup->addElement(&l2);
    m_pLayerGroup->addElement(&l3);

    /// make sure, list not contains 3 elements
    EXPECT_EQ(3u, m_pLayerGroup->getList().size());

    /// remove 2 layers from graphical group
    m_pLayerGroup->removeElement(&l2);
    m_pLayerGroup->removeElement(&l3);

    /// make sure, list not contains 1 element
    EXPECT_EQ(1u, m_pLayerGroup->getList().size());

    /// remove last layers from graphical group
    m_pLayerGroup->removeElement(&l1);

    /// make sure, list is empty
    EXPECT_EQ(0u, m_pLayerGroup->getList().size());
}

TEST_F(GraphicalGroupTest, removeElement_Surface)
{
    /// make sure, list is empty
    EXPECT_EQ(0u, m_pSurfaceGroup->getList().size());

    /// add 3 surfaces to graphical group
    Surface l1(0), l2(0), l3(0);
    m_pSurfaceGroup->addElement(&l1);
    m_pSurfaceGroup->addElement(&l2);
    m_pSurfaceGroup->addElement(&l3);

    /// make sure, list not contains 3 elements
    EXPECT_EQ(3u, m_pSurfaceGroup->getList().size());

    /// remove 2 surfaces from graphical group
    m_pSurfaceGroup->removeElement(&l2);
    m_pSurfaceGroup->removeElement(&l3);

    /// make sure, list not contains 1 element
    EXPECT_EQ(1u, m_pSurfaceGroup->getList().size());

    /// remove last surfaces from graphical group
    m_pSurfaceGroup->removeElement(&l1);

    /// make sure, list is empty
    EXPECT_EQ(0u, m_pSurfaceGroup->getList().size());
}

TEST_F(GraphicalGroupTest, DISABLED_getShader)
{
    // TODO: how should this be tested?
    // each surface/layer may have a differnet shader.
    // what should be returned in that case?
}
