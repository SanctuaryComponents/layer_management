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

    /// make sure, layer contains no surfaces (TODO)
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
    //EXPECT_EQ(INVALID_ID, pSurface->getContainingLayerId());
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
}

TEST_F(SceneTest, createSurfaceGroup)
{
}

TEST_F(SceneTest, removeLayer)
{
}

TEST_F(SceneTest, removeSurface)
{
}

TEST_F(SceneTest, getLayer)
{
}

TEST_F(SceneTest, getSurface)
{
}

TEST_F(SceneTest, getSurfaceGroup)
{
}

TEST_F(SceneTest, getLayerGroup)
{
}

TEST_F(SceneTest, getLayerIDs)
{
}

TEST_F(SceneTest, getLayerIDsOfScreen)
{
}

TEST_F(SceneTest, getSurfaceIDs)
{
}

TEST_F(SceneTest, getLayerGroupIDs)
{
}

TEST_F(SceneTest, getSurfaceGroupIDs)
{
}

TEST_F(SceneTest, lockScene)
{
}

TEST_F(SceneTest, unlockScene)
{
}
