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

    /// make sure, scene contains no layers
    m_pScene->getLayerIDs(&size, &array);
    ASSERT_EQ(0, size) << "Scene should contain 0 layers";

    /// create layer with id 91
    int expected = 91;
    m_pScene->createLayer(expected);

    /// make sure, scene contains one layer
    m_pScene->getLayerIDs(&size, &array);
    ASSERT_EQ(1, size) << "Scene should contain 1 layer";

    /// make sure layer has id 91
    pLayer = m_pScene->getLayer(expected);
    ASSERT_TRUE(pLayer) << "Layer was not found.";
    EXPECT_EQ(expected, pLayer->getID());

    /// make sure, layer has default size
    EXPECT_EQ(0, pLayer->OriginalSourceHeight);
    EXPECT_EQ(0, pLayer->OriginalSourceWidth);

    /// make sure, layer has default visibility
    EXPECT_EQ(false, pLayer->visibility);

    /// make sure, layer has default opacity
    EXPECT_DOUBLE_EQ(1.0, pLayer->opacity);

    /// make sure, layer has default source rectangle
    const Rectangle& srcRect = pLayer->getSourceRegion();
    EXPECT_EQ(0, srcRect.height);
    EXPECT_EQ(0, srcRect.width);
    EXPECT_EQ(0, srcRect.x);
    EXPECT_EQ(0, srcRect.y);

    /// make sure, layer has default destination rectangle
    const Rectangle& destRect = pLayer->getDestinationRegion();
    EXPECT_EQ(0, destRect.height);
    EXPECT_EQ(0, destRect.width);
    EXPECT_EQ(0, destRect.x);
    EXPECT_EQ(0, destRect.y);

    /// make sure, layer layer has default type
    EXPECT_EQ(Software_2D, pLayer->getLayerType());

    /// make sure, layer contains no surfaces (TODO)
}

TEST_F(SceneTest, createLayer_InvalidInput)
{
    /// create Layer with id 55 (TODO)

    /// try to create existing layer 55 (TODO)
}

TEST_F(SceneTest, createSurface)
{
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
