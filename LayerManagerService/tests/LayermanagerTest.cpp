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

#include "Layermanager.h"

class LayermanagerTest : public ::testing::Test
{
public:
    void SetUp()
    {
        m_pLayermanager = new Layermanager();
        ASSERT_TRUE(m_pLayermanager);
    }

    void TearDown()
    {
        if (m_pLayermanager)
        {
            delete m_pLayermanager;
            m_pLayermanager = 0;
        }
    }

    ICommandExecutor* m_pLayermanager;
};

TEST_F(LayermanagerTest, execute)
{

}

TEST_F(LayermanagerTest, startManagement)
{

}

TEST_F(LayermanagerTest, stopManagement)
{

}

TEST_F(LayermanagerTest, getScene)
{

}

TEST_F(LayermanagerTest, getRendererList)
{

}

TEST_F(LayermanagerTest, addRenderer)
{

}

TEST_F(LayermanagerTest, removeRenderer)
{

}

TEST_F(LayermanagerTest, getCommunicatorList)
{

}

TEST_F(LayermanagerTest, addCommunicator)
{

}

TEST_F(LayermanagerTest, removeCommunicator)
{

}

TEST_F(LayermanagerTest, getSceneProviderList)
{

}

TEST_F(LayermanagerTest, addSceneProvider)
{

}

TEST_F(LayermanagerTest, removeSceneProvider)
{

}

TEST_F(LayermanagerTest, getLayerTypeCapabilities)
{

}

TEST_F(LayermanagerTest, getNumberOfHardwareLayers)
{

}

TEST_F(LayermanagerTest, getScreenResolution)
{

}

TEST_F(LayermanagerTest, getScreenIDs)
{

}
