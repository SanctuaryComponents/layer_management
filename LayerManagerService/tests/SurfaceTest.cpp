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

#include "Surface.h"

class SurfaceTest : public ::testing::Test
{
public:
    void SetUp()
    {
        m_pSurface = new Surface();
        ASSERT_TRUE(m_pSurface);
    }

    void TearDown()
    {
        if (m_pSurface)
        {
            delete m_pSurface;
            m_pSurface = 0;
        }
    }

    Surface* m_pSurface;
};

TEST_F(SurfaceTest, defaultConstructor)
{
    Surface surface;

    /// make sure, surface was not added to any layer
    //EXPECT_EQ(INVALID_ID, surface.getContainingLayerId());

    /// make sure, surface has default size
    EXPECT_EQ(0, surface.OriginalSourceHeight);
    EXPECT_EQ(0, surface.OriginalSourceWidth);

    /// make sure, surface has default visibility
    EXPECT_EQ(false, surface.getVisibility());

    /// make sure, surface has default opacity
    EXPECT_DOUBLE_EQ(1.0, surface.getOpacity());

    /// make sure, surface has default orientation
    EXPECT_EQ(Zero, surface.getOrientation());

    /// make sure, surface has default pixel format
    EXPECT_EQ(PIXELFORMAT_UNKNOWN, surface.getPixelFormat());

    /// make sure, surface has default source rectangle
    const Rectangle& srcRect = surface.getSourceRegion();
    EXPECT_EQ(0, srcRect.height);
    EXPECT_EQ(0, srcRect.width);
    EXPECT_EQ(0, srcRect.x);
    EXPECT_EQ(0, srcRect.y);

    /// make sure, surface has default destination rectangle
    const Rectangle& destRect = surface.getDestinationRegion();
    EXPECT_EQ(0, destRect.height);
    EXPECT_EQ(0, destRect.width);
    EXPECT_EQ(0, destRect.x);
    EXPECT_EQ(0, destRect.y);
}

TEST_F(SurfaceTest, specialConstructor)
{
    unsigned int expectedId = 144;
    Surface surface(expectedId);

    /// make sure surface has specified id
    EXPECT_EQ(expectedId, surface.getID());

    /// make sure, surface was not added to any layer
    //EXPECT_EQ(INVALID_ID, surface.getContainingLayerId());

    /// make sure, surface has default size
    EXPECT_EQ(0, surface.OriginalSourceHeight);
    EXPECT_EQ(0, surface.OriginalSourceWidth);

    /// make sure, surface has default visibility
    EXPECT_EQ(false, surface.getVisibility());

    /// make sure, surface has default opacity
    EXPECT_DOUBLE_EQ(1.0, surface.getOpacity());

    /// make sure, surface has default orientation
    EXPECT_EQ(Zero, surface.getOrientation());

    /// make sure, surface has default pixel format
    EXPECT_EQ(PIXELFORMAT_UNKNOWN, surface.getPixelFormat());

    /// make sure, surface has default source rectangle
    const Rectangle& srcRect = surface.getSourceRegion();
    EXPECT_EQ(0, srcRect.height);
    EXPECT_EQ(0, srcRect.width);
    EXPECT_EQ(0, srcRect.x);
    EXPECT_EQ(0, srcRect.y);

    /// make sure, surface has default destination rectangle
    const Rectangle& destRect = surface.getDestinationRegion();
    EXPECT_EQ(0, destRect.height);
    EXPECT_EQ(0, destRect.width);
    EXPECT_EQ(0, destRect.x);
    EXPECT_EQ(0, destRect.y);
}

TEST_F(SurfaceTest, getPixelFormat)
{

}

TEST_F(SurfaceTest, setPixelFormat)
{

}

TEST_F(SurfaceTest, getContainingLayerId)
{

}

TEST_F(SurfaceTest, setContainingLayerId)
{

}

TEST_F(SurfaceTest, hasNativeContent)
{

}

TEST_F(SurfaceTest, removeNativeContent)
{

}

TEST_F(SurfaceTest, setNativeContent)
{

}

TEST_F(SurfaceTest, getNativeContent)
{

}
