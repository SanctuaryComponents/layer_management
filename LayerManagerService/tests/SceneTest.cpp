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

#include "IScene.h"
#include "Scene.h"
#include "SurfaceMap.h"

class SceneTest : public ::testing::Test
{
public:
    void SetUp()
    {
        m_pScene = new Scene();
        ASSERT_TRUE(m_pScene);
    }

    void TearDown()
    {
        if (m_pScene)
        {
            delete m_pScene;
            m_pScene = 0;
        }
    }

    IScene* m_pScene;
};

TEST_F(SceneTest, createLayer)
{
    uint size;
    uint* array;
    Layer* pLayer;
    int expected = 91;

    /// make sure, scene contains no layers
    m_pScene->getLayerIDs(&size, &array);
    ASSERT_EQ(0, size) << "Scene should contain 0 layers";

    /// create layer with expected id
    pLayer = m_pScene->createLayer(expected);
    ASSERT_TRUE(pLayer) << "Layer was not created.";

    /// make sure layer has expected id
    EXPECT_EQ(expected, pLayer->getID());

    /// make sure, scene contains one layer
    m_pScene->getLayerIDs(&size, &array);
    ASSERT_EQ(1, size) << "Scene should contain 1 layer";

    /// make sure, layer contains no surfaces
    SurfaceList& slist = pLayer->getAllSurfaces();
    EXPECT_EQ(0, slist.size());
}

TEST_F(SceneTest, createLayer_twice)
{
    uint size;
    uint* array;
    int expected = 55;
    double expectedOpacity = 0.322;
    Layer* pLayer1;
    Layer* pLayer2;

    /// create Layer with id 55
    pLayer1 = m_pScene->createLayer(expected);
    ASSERT_EQ(expected, pLayer1->getID());

    /// make sure, scene contains one layer
    m_pScene->getLayerIDs(&size, &array);
    ASSERT_EQ(1, size) << "Scene should contain 1 layer";

    /// try to create existing layer 55, handle to existing layer should be returned
    pLayer2 = m_pScene->createLayer(expected);
    ASSERT_EQ(expected, pLayer2->getID());

    /// make sure, scene still contains one layer
    m_pScene->getLayerIDs(&size, &array);
    ASSERT_EQ(1, size) << "Scene should contain 1 layer";

    /// change opacity using first layer handle
    pLayer1->setOpacity(expectedOpacity);

    /// check opacity of layer using second handle
    EXPECT_DOUBLE_EQ(expectedOpacity, pLayer2->getOpacity());
}

TEST_F(SceneTest, createSurface)
{
    uint size;
    uint* array;
    Surface* pSurface;
    int expected = 131;

    /// make sure, scene contains no surfaces
    m_pScene->getSurfaceIDs(&size, &array);
    ASSERT_EQ(0, size) << "Scene should contain 0 surfaces";

    /// create surface with expected id
    pSurface = m_pScene->createSurface(expected);
    ASSERT_TRUE(pSurface) << "Surface was not created.";

    /// make sure surface has expected id
    EXPECT_EQ(expected, pSurface->getID());

    /// make sure, scene contains one surface
    m_pScene->getSurfaceIDs(&size, &array);
    ASSERT_EQ(1, size) << "Scene should contain 1 surface";

    /// make sure, surface was not added to any layer
    EXPECT_EQ(GraphicalObject::INVALID_ID, pSurface->getContainingLayerId());
}

TEST_F(SceneTest, createSurface_twice)
{
    uint size;
    uint* array;
    Surface* pSurface1;
    Surface* pSurface2;
    int expected = 135;
    double expectedOpacity = 0.718;

    /// make sure, scene contains no surfaces
    m_pScene->getSurfaceIDs(&size, &array);
    ASSERT_EQ(0, size) << "Scene should contain 0 surfaces";

    /// create surface with expected id
    pSurface1 = m_pScene->createSurface(expected);
    ASSERT_TRUE(pSurface1) << "Surface was not created.";

    /// make sure surface has expected id
    EXPECT_EQ(expected, pSurface1->getID());

    /// make sure, scene contains one surface
    m_pScene->getSurfaceIDs(&size, &array);
    ASSERT_EQ(1, size) << "Scene should contain 1 surface";

    /// create surface with expected id again
    pSurface2 = m_pScene->createSurface(expected);
    ASSERT_TRUE(pSurface2) << "Surface was not created.";

    /// make sure surface has expected id again
    EXPECT_EQ(expected, pSurface2->getID());

    /// make sure, scene still contains one surface
    m_pScene->getSurfaceIDs(&size, &array);
    ASSERT_EQ(1, size) << "Scene should contain 1 surface";

    /// change opacity using first surface handle
    pSurface1->setOpacity(expectedOpacity);

    /// check opacity of surface using second surface handle
    EXPECT_DOUBLE_EQ(expectedOpacity, pSurface2->getOpacity());
}

TEST_F(SceneTest, createLayerGroup)
{
    unsigned int expectedId = 167;
    LayerGroup* pLayerGroup;

    /// make sure, expected layer group does not exist
    pLayerGroup = m_pScene->getLayerGroup(expectedId);
    ASSERT_FALSE(pLayerGroup);

    /// create expected layer group
    m_pScene->createLayerGroup(expectedId);

    /// make sure, expected layer group does exist
    pLayerGroup = m_pScene->getLayerGroup(expectedId);
    EXPECT_TRUE(pLayerGroup);
}

TEST_F(SceneTest, createLayerGroup_twice)
{
    unsigned int expectedLayerGroupId = 169;
    unsigned int expectedLayerId = 170;
    LayerGroup* pLayerGroup1;
    LayerGroup* pLayerGroup2;
    Layer layer(expectedLayerId);

    /// make sure, expected layer group does not exist
    pLayerGroup1 = m_pScene->getLayerGroup(expectedLayerGroupId);
    ASSERT_FALSE(pLayerGroup1);

    /// create expected layer group, get 1st handle to group
    pLayerGroup1 = m_pScene->createLayerGroup(expectedLayerGroupId);
    ASSERT_TRUE(pLayerGroup1);

    /// make sure, expected layer group does exist
    EXPECT_TRUE(pLayerGroup1);

    /// create expected layer group again, get 2nd handle to group
    pLayerGroup2 = m_pScene->createLayerGroup(expectedLayerGroupId);

    /// make sure, expected layer group does exist
    ASSERT_TRUE(pLayerGroup2);

    /// add layer to layer group using 1st handle
    pLayerGroup1->addElement(&layer);

    /// make sure, layer can be accessed in layer group using 2nd handle
    const LayerList& llist = pLayerGroup2->getList();
    EXPECT_EQ(1, llist.size());
    LayerListConstIterator iter = llist.begin();
    EXPECT_EQ(expectedLayerId, (*iter)->getID());
    EXPECT_EQ(llist.end(), ++iter);
}

TEST_F(SceneTest, createSurfaceGroup)
{
    unsigned int expectedSurfaceGroupId = 172;
    SurfaceGroup* pSurfaceGroup;

    /// make sure, expected Surface group does not exist
    pSurfaceGroup = m_pScene->getSurfaceGroup(expectedSurfaceGroupId);
    ASSERT_FALSE(pSurfaceGroup);

    /// create expected Surface group, get 1st handle to group
    pSurfaceGroup = m_pScene->createSurfaceGroup(expectedSurfaceGroupId);
    ASSERT_TRUE(pSurfaceGroup);

    /// make sure, expected Surface group does exist
    EXPECT_TRUE(pSurfaceGroup);

}
TEST_F(SceneTest, createSurfaceGroup_twice)
{
    unsigned int expectedSurfaceGroupId = 172;
    unsigned int expectedSurfaceId = 173;
    SurfaceGroup* pSurfaceGroup1;
    SurfaceGroup* pSurfaceGroup2;
    Surface Surface(expectedSurfaceId);

    /// make sure, expected Surface group does not exist
    pSurfaceGroup1 = m_pScene->getSurfaceGroup(expectedSurfaceGroupId);
    ASSERT_FALSE(pSurfaceGroup1);

    /// create expected Surface group, get 1st handle to group
    pSurfaceGroup1 = m_pScene->createSurfaceGroup(expectedSurfaceGroupId);
    ASSERT_TRUE(pSurfaceGroup1);

    /// make sure, expected Surface group does exist
    EXPECT_TRUE(pSurfaceGroup1);

    /// create expected Surface group again, get 2nd handle to group
    pSurfaceGroup2 = m_pScene->createSurfaceGroup(expectedSurfaceGroupId);

    /// make sure, expected Surface group does exist
    ASSERT_TRUE(pSurfaceGroup2);

    /// add Surface to Surface group using 1st handle
    pSurfaceGroup1->addElement(&Surface);

    /// make sure, Surface can be accessed in Surface group using 2nd handle
    const SurfaceList& llist = pSurfaceGroup2->getList();
    EXPECT_EQ(1, llist.size());
    SurfaceListConstIterator iter = llist.begin();
    EXPECT_EQ(expectedSurfaceId, (*iter)->getID());
    EXPECT_EQ(llist.end(), ++iter);
}

TEST_F(SceneTest, removeLayer)
{
    unsigned int expectedLayerId = 188;
    Layer* pLayer;

    /// create layer
    pLayer = m_pScene->createLayer(expectedLayerId);
    ASSERT_TRUE(pLayer);

    /// make sure, layer exists
    ASSERT_EQ(pLayer, m_pScene->getLayer(expectedLayerId));

    /// remove layer
    m_pScene->removeLayer(pLayer);

    /// make sure, layer does not exist
    ASSERT_FALSE(m_pScene->getLayer(expectedLayerId));
}

TEST_F(SceneTest, removeSurface)
{
    unsigned int expectedSurfaceId = 189;
    Surface* pSurface;

    /// create Surface
    pSurface = m_pScene->createSurface(expectedSurfaceId);
    ASSERT_TRUE(pSurface);

    /// make sure, Surface exists
    ASSERT_EQ(pSurface, m_pScene->getSurface(expectedSurfaceId));

    /// remove Surface
    m_pScene->removeSurface(pSurface);

    /// make sure, Surface does not exist
    ASSERT_FALSE(m_pScene->getSurface(expectedSurfaceId));
}

TEST_F(SceneTest, getLayer)
{
    unsigned int expectedLayerId = 198;
    Layer* pLayer;

    /// try to get non existing layer
    EXPECT_FALSE(m_pScene->getLayer(expectedLayerId));

    /// create layer
    pLayer = m_pScene->createLayer(expectedLayerId);
    ASSERT_TRUE(pLayer);

    /// get layer
    ASSERT_EQ(pLayer, m_pScene->getLayer(expectedLayerId));

    /// remove layer
    m_pScene->removeLayer(pLayer);

    /// try to get removed layer
    ASSERT_FALSE(m_pScene->getLayer(expectedLayerId));
}

TEST_F(SceneTest, getSurface)
{
    unsigned int expectedSurfaceId = 198;
    Surface* pSurface;

    /// try to get non existing Surface
    EXPECT_FALSE(m_pScene->getSurface(expectedSurfaceId));

    /// create Surface
    pSurface = m_pScene->createSurface(expectedSurfaceId);
    ASSERT_TRUE(pSurface);

    /// get Surface
    ASSERT_EQ(pSurface, m_pScene->getSurface(expectedSurfaceId));

    /// remove Surface
    m_pScene->removeSurface(pSurface);

    /// try to get removed Surface
    ASSERT_FALSE(m_pScene->getSurface(expectedSurfaceId));
}

TEST_F(SceneTest, getSurfaceGroup)
{
    unsigned int expectedSurfaceGroupId = 198;
    SurfaceGroup* pSurfaceGroup;

    /// try to get non existing SurfaceGroup
    EXPECT_FALSE(m_pScene->getSurfaceGroup(expectedSurfaceGroupId));

    /// create SurfaceGroup
    pSurfaceGroup = m_pScene->createSurfaceGroup(expectedSurfaceGroupId);
    ASSERT_TRUE(pSurfaceGroup);

    /// get SurfaceGroup
    ASSERT_EQ(pSurfaceGroup, m_pScene->getSurfaceGroup(expectedSurfaceGroupId));

    /// remove SurfaceGroup
    m_pScene->removeSurfaceGroup(pSurfaceGroup);

    /// try to get removed SurfaceGroup
    ASSERT_FALSE(m_pScene->getSurfaceGroup(expectedSurfaceGroupId));
}

TEST_F(SceneTest, getLayerGroup)
{
    unsigned int expectedLayerGroupId = 203;
    LayerGroup* pLayerGroup;

    /// try to get non existing LayerGroup
    EXPECT_FALSE(m_pScene->getLayerGroup(expectedLayerGroupId));

    /// create LayerGroup
    pLayerGroup = m_pScene->createLayerGroup(expectedLayerGroupId);
    ASSERT_TRUE(pLayerGroup);

    /// get LayerGroup
    ASSERT_EQ(pLayerGroup, m_pScene->getLayerGroup(expectedLayerGroupId));

    /// remove LayerGroup
    m_pScene->removeLayerGroup(pLayerGroup);

    /// try to get removed LayerGroup
    ASSERT_FALSE(m_pScene->getLayerGroup(expectedLayerGroupId));
}

TEST_F(SceneTest, getLayerIDs)
{
    unsigned int layerId1 = 101;
    unsigned int layerId2 = 102;
    unsigned int layerId3 = 103;
    unsigned int layerId4 = 104;
    unsigned int size;
    unsigned int* array;

    /// make sure, scene contains no layers
    m_pScene->getLayerIDs(&size, &array);
    ASSERT_EQ(0, size);

    /// create 4 layers in scene
    m_pScene->createLayer(layerId1);
    m_pScene->createLayer(layerId2);
    m_pScene->createLayer(layerId3);
    m_pScene->createLayer(layerId4);

    /// make sure, scene contains these 4 layers
    m_pScene->getLayerIDs(&size, &array);
    ASSERT_EQ(4, size);
    EXPECT_EQ(layerId1, array[0]);
    EXPECT_EQ(layerId2, array[1]);
    EXPECT_EQ(layerId3, array[2]);
    EXPECT_EQ(layerId4, array[3]);
}

TEST_F(SceneTest, getLayerIDsOfScreen)
{
    unsigned int screenId = 0;
    unsigned int layerId1 = 121;
    unsigned int layerId2 = 122;
    unsigned int layerId3 = 123;
    unsigned int layerId4 = 124;
    unsigned int size;
    unsigned int* array;
    Layer* l1;
    Layer* l2;
    Layer* l3;
    Layer* l4;

    /// make sure, scene contains no layers
    m_pScene->getLayerIDs(&size, &array);
    ASSERT_EQ(0, size);

    /// create 4 layers in scene, but dont add them to render order
    l1 = m_pScene->createLayer(layerId1);
    l2 = m_pScene->createLayer(layerId2);
    l3 = m_pScene->createLayer(layerId3);
    l4 = m_pScene->createLayer(layerId4);

    /// make sure, scene contains these 4 layers
    m_pScene->getLayerIDs(&size, &array);
    ASSERT_EQ(4, size);
    EXPECT_EQ(layerId1, array[0]);
    EXPECT_EQ(layerId2, array[1]);
    EXPECT_EQ(layerId3, array[2]);
    EXPECT_EQ(layerId4, array[3]);

    /// make sure, screen still has no layers applied
    m_pScene->getLayerIDsOfScreen(screenId, &size, &array);
    ASSERT_EQ(0, size);

    /// add 3 layers to screen
    LayerList& llist = m_pScene->getCurrentRenderOrder();
    llist.push_back(l1);
    llist.push_back(l3);
    llist.push_back(l4);

    /// make sure, screen now has 3 layers
    m_pScene->getLayerIDsOfScreen(screenId, &size, &array);
    ASSERT_EQ(3, size);
    EXPECT_EQ(l1->getID(), array[0]);
    EXPECT_EQ(l3->getID(), array[1]);
    EXPECT_EQ(l4->getID(), array[2]);

    /// add 4th layer to screen
    llist.push_back(l2);

    /// make sure, screen now has 4 layers
    m_pScene->getLayerIDsOfScreen(screenId, &size, &array);
    ASSERT_EQ(4, size);
    EXPECT_EQ(l1->getID(), array[0]);
    EXPECT_EQ(l3->getID(), array[1]);
    EXPECT_EQ(l4->getID(), array[2]);
    EXPECT_EQ(l2->getID(), array[3]);
}

TEST_F(SceneTest, getSurfaceIDs)
{
    unsigned int surfaceId1 = 141;
    unsigned int surfaceId2 = 142;
    unsigned int surfaceId3 = 143;
    unsigned int surfaceId4 = 144;
    unsigned int size;
    unsigned int* array;

    /// make sure, scene contains no surfaces
    m_pScene->getSurfaceIDs(&size, &array);
    ASSERT_EQ(0, size);

    /// create 4 surfaces in scene
    m_pScene->createSurface(surfaceId1);
    m_pScene->createSurface(surfaceId2);
    m_pScene->createSurface(surfaceId3);
    m_pScene->createSurface(surfaceId4);

    /// make sure, scene contains these 4 surfaces
    m_pScene->getSurfaceIDs(&size, &array);
    ASSERT_EQ(4, size);
    EXPECT_EQ(surfaceId1, array[0]);
    EXPECT_EQ(surfaceId2, array[1]);
    EXPECT_EQ(surfaceId3, array[2]);
    EXPECT_EQ(surfaceId4, array[3]);
}

TEST_F(SceneTest, getLayerGroupIDs)
{
    unsigned int layergroupId1 = 201;
    unsigned int layergroupId2 = 202;
    unsigned int layergroupId3 = 203;
    unsigned int layergroupId4 = 204;
    unsigned int size;
    unsigned int* array;

    /// make sure, scene contains no layergroups
    m_pScene->getLayerGroupIDs(&size, &array);
    ASSERT_EQ(0, size);

    /// create 4 layergroups in scene
    m_pScene->createLayerGroup(layergroupId1);
    m_pScene->createLayerGroup(layergroupId2);
    m_pScene->createLayerGroup(layergroupId3);
    m_pScene->createLayerGroup(layergroupId4);

    /// make sure, scene contains these 4 layergroups
    m_pScene->getLayerGroupIDs(&size, &array);
    ASSERT_EQ(4, size);
    EXPECT_EQ(layergroupId1, array[0]);
    EXPECT_EQ(layergroupId2, array[1]);
    EXPECT_EQ(layergroupId3, array[2]);
    EXPECT_EQ(layergroupId4, array[3]);
}

TEST_F(SceneTest, getSurfaceGroupIDs)
{
    unsigned int surfacegroupId1 = 201;
    unsigned int surfacegroupId2 = 202;
    unsigned int surfacegroupId3 = 203;
    unsigned int surfacegroupId4 = 204;
    unsigned int size;
    unsigned int* array;

    /// make sure, scene contains no surfacegroups
    m_pScene->getSurfaceGroupIDs(&size, &array);
    ASSERT_EQ(0, size);

    /// create 4 surfacegroups in scene
    m_pScene->createSurfaceGroup(surfacegroupId1);
    m_pScene->createSurfaceGroup(surfacegroupId2);
    m_pScene->createSurfaceGroup(surfacegroupId3);
    m_pScene->createSurfaceGroup(surfacegroupId4);

    /// make sure, scene contains these 4 surfacegroups
    m_pScene->getSurfaceGroupIDs(&size, &array);
    ASSERT_EQ(4, size);
    EXPECT_EQ(surfacegroupId1, array[0]);
    EXPECT_EQ(surfacegroupId2, array[1]);
    EXPECT_EQ(surfacegroupId3, array[2]);
    EXPECT_EQ(surfacegroupId4, array[3]);
}

TEST_F(SceneTest, DISABLED_lockScene)
{
}

TEST_F(SceneTest, DISABLED_unlockScene)
{
}

TEST_F(SceneTest, getCurrentRenderOrder)
{
    // TODO: how to test? return by typically reference can't be invalid.

    /// get render order
    LayerList& llist = m_pScene->getCurrentRenderOrder();

    /// make sure, list of layers is returned
    ASSERT_TRUE(&llist);
}

TEST_F(SceneTest, removeSurfaceGroup)
{
    unsigned int expectedSurfaceGroupId = 172;
    SurfaceGroup* pSurfaceGroup;

    /// create expected Surface group
    pSurfaceGroup = m_pScene->createSurfaceGroup(expectedSurfaceGroupId);
    ASSERT_TRUE(pSurfaceGroup);

    /// make sure, expected Surface group does exist
    pSurfaceGroup = m_pScene->getSurfaceGroup(expectedSurfaceGroupId);
    EXPECT_TRUE(pSurfaceGroup);

    /// remove surface group
    m_pScene->removeSurfaceGroup(pSurfaceGroup);

    /// make sure, expected Surface group does not exist
    pSurfaceGroup = m_pScene->getSurfaceGroup(expectedSurfaceGroupId);
    ASSERT_FALSE(pSurfaceGroup);
}

TEST_F(SceneTest, removeLayerGroup)
{
    unsigned int expectedLayerGroupId = 172;
    LayerGroup* pLayerGroup;

    /// create expected Layer group
    pLayerGroup = m_pScene->createLayerGroup(expectedLayerGroupId);
    ASSERT_TRUE(pLayerGroup);

    /// make sure, expected Layer group does exist
    pLayerGroup = m_pScene->getLayerGroup(expectedLayerGroupId);
    EXPECT_TRUE(pLayerGroup);

    /// remove layer group
    m_pScene->removeLayerGroup(pLayerGroup);

    /// make sure, expected Layer group does not exist
    pLayerGroup = m_pScene->getLayerGroup(expectedLayerGroupId);
    ASSERT_FALSE(pLayerGroup);
}

TEST_F(SceneTest, getAllSurfaces)
{
    unsigned int expectedId1 = 241;
    unsigned int expectedId2 = 242;
    unsigned int expectedId3 = 243;
    Surface* s1;
    Surface* s2;
    Surface* s3;

    /// try to get surfaces, when no surfaces exist in scene
    ASSERT_EQ(0, m_pScene->getAllSurfaces().size());

    /// add 3 surfaces to scene
    s1 = m_pScene->createSurface(expectedId1);
    s2 = m_pScene->createSurface(expectedId2);
    s3 = m_pScene->createSurface(expectedId3);

    /// get all surfaces
    const SurfaceMap& smap1 = m_pScene->getAllSurfaces();
    ASSERT_EQ(3, smap1.size());
    // order is undefined here, but each surface must be contained once
    EXPECT_EQ(1, smap1.count(expectedId1));
    EXPECT_EQ(1, smap1.count(expectedId2));
    EXPECT_EQ(1, smap1.count(expectedId3));

    /// remove 2 surfaces again
    m_pScene->removeSurface(s1);
    m_pScene->removeSurface(s3);

    /// check, if the remaining surface is the expected one
    const SurfaceMap& smap2 = m_pScene->getAllSurfaces();
    ASSERT_EQ(1, smap2.size());
    EXPECT_EQ(1, smap2.count(expectedId2));
}

TEST_F(SceneTest, DISABLED_getSurfaceAt)
{
}

TEST_F(SceneTest, isLayerInCurrentRenderOrder)
{
    unsigned int layerId1 = 121;
    unsigned int size;
    unsigned int* array;
    Layer* l1;

    /// create layer in scene, but dont add it to render order
    l1 = m_pScene->createLayer(layerId1);

    /// make sure, scene contains the new layers
    m_pScene->getLayerIDs(&size, &array);
    ASSERT_EQ(1, size);
    EXPECT_EQ(layerId1, array[0]);

    /// make sure, layer is not in render order
    ASSERT_FALSE(m_pScene->isLayerInCurrentRenderOrder(layerId1));

    /// add layer to render order
    LayerList& llist = m_pScene->getCurrentRenderOrder();
    llist.push_back(l1);

    /// make sure, layer is in render order
    ASSERT_TRUE(m_pScene->isLayerInCurrentRenderOrder(layerId1));

    /// remove layer from render order
    llist.remove(l1);

    /// make sure, layer is not in render order
    ASSERT_FALSE(m_pScene->isLayerInCurrentRenderOrder(layerId1));
}