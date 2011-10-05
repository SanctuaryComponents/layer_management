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
#include "Rectangle.h"
#include "Surface.h"
#include "Layer.h"
#include "Transformation/ViewportTransform.h"
#include "Scene.h"

class ViewportTransformTest : public ::testing::Test {
public:

	ViewportTransformTest()
    {
    }

    virtual ~ViewportTransformTest()
    {
    }
    void SetUp() {
        m_scene = new Scene();
    }
    void TearDown() {
        delete m_scene;
    }
    Scene* m_scene;
};

// LAYER SOURCE TRANSFORMATION

TEST_F(ViewportTransformTest, doLayerSRCSurfaceCompletelyWithin){
    Rectangle targetSurfaceSource(0,0,10,10);
    Rectangle targetSurfaceDest(20,20,10,10);
    Rectangle layerSRC(10,10,30,30);

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSource, targetSurfaceDest);
    ASSERT_EQ(10u, targetSurfaceDest.x);
    ASSERT_EQ(10u, targetSurfaceDest.width);
    ASSERT_EQ(10u, targetSurfaceDest.y);
    ASSERT_EQ(10u, targetSurfaceDest.height);

    ASSERT_EQ(0u, targetSurfaceSource.x);
    ASSERT_EQ(10u, targetSurfaceSource.width);
    ASSERT_EQ(0u, targetSurfaceSource.y);
    ASSERT_EQ(10u, targetSurfaceSource.height);

}

TEST_F(ViewportTransformTest, doLayerSRCSurfaceCroppedFromLeft){
    uint surfaceOriginalWidth = 20;
    uint surfaceOriginalHeight = 20;
    Rectangle targetSurfaceSrc(0,0,20,20);
    Rectangle targetSurfaceDest(20,20,20,20);
    Rectangle layerSRC(30,30,20,20);
    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_EQ(0.5,textureCoordinates[0]);
    ASSERT_EQ(1.0,textureCoordinates[1]);
    ASSERT_EQ(0.5,textureCoordinates[2]);
    ASSERT_EQ(1.0,textureCoordinates[3]);
    ASSERT_EQ(0u, targetSurfaceDest.x);
    ASSERT_EQ(10u, targetSurfaceDest.width);
    ASSERT_EQ(0u, targetSurfaceDest.y);
    ASSERT_EQ(10u, targetSurfaceDest.height);
}

TEST_F(ViewportTransformTest, doLayerSRCSurfaceCroppedFromRight){
    uint surfaceOriginalWidth = 20;
    uint surfaceOriginalHeight = 20;
    Rectangle targetSurfaceSrc(0,0,20,20);
    Rectangle targetSurfaceDest(20,20,20,20);
    Rectangle layerSRC(0,0,35,35);

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_EQ(0,textureCoordinates[0]);
    ASSERT_EQ(0.75,textureCoordinates[1]);
    ASSERT_EQ(0,textureCoordinates[2]);
    ASSERT_EQ(0.75,textureCoordinates[3]);
    ASSERT_EQ(20u, targetSurfaceDest.x);
    ASSERT_EQ(15u, targetSurfaceDest.width);
    ASSERT_EQ(20u, targetSurfaceDest.y);
    ASSERT_EQ(15u, targetSurfaceDest.height);
}

TEST_F(ViewportTransformTest, doLayerSRCSurfaceCroppedFromBothSides){
    uint surfaceOriginalWidth = 20;
    uint surfaceOriginalHeight = 20;
    Rectangle targetSurfaceSrc(0,0,20,20);
    Rectangle targetSurfaceDest(20,20,20,20);
    Rectangle layerSRC(25,25,10,10);

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_EQ(0.25,textureCoordinates[0]);
    ASSERT_EQ(0.75,textureCoordinates[1]);
    ASSERT_EQ(0.25,textureCoordinates[2]);
    ASSERT_EQ(0.75,textureCoordinates[3]);
    ASSERT_EQ(0u, targetSurfaceDest.x);
    ASSERT_EQ(10u, targetSurfaceDest.width);
    ASSERT_EQ(0u, targetSurfaceDest.y);
    ASSERT_EQ(10u, targetSurfaceDest.height);
}

	// LAYER DESTINATION TRANSFORMATION

TEST_F(ViewportTransformTest, doLayerDESTScaleUp){
    uint surfaceOriginalWidth = 20;
    uint surfaceOriginalHeight = 20;
    Rectangle targetSurfaceSrc(0,0,20,20);
    Rectangle targetSurfaceDest(10,10,100,100);
    Rectangle layerSrc(0,0,200,200);
    Rectangle layerDest(50,50,600,600);

    ViewportTransform::applyLayerDestination(layerDest,layerSrc,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_EQ(0.0,textureCoordinates[0]);
    ASSERT_EQ(1.0,textureCoordinates[1]);
    ASSERT_EQ(0.0,textureCoordinates[2]);
    ASSERT_EQ(1.0,textureCoordinates[3]);
    ASSERT_EQ(30u+50u, targetSurfaceDest.x);
    ASSERT_EQ(300u, targetSurfaceDest.width);
    ASSERT_EQ(30u+50u, targetSurfaceDest.y);
    ASSERT_EQ(300u, targetSurfaceDest.height);
}

TEST_F(ViewportTransformTest, doLayerDESTScaleDown){
    uint surfaceOriginalWidth = 20;
    uint surfaceOriginalHeight = 20;
    Rectangle targetSurfaceSrc(0,0,20,20);
    Rectangle targetSurfaceDest(10,10,100,100);
    Rectangle layerSRC(0,0,200,200);
    Rectangle layerDest(50,50,20,20);

    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_EQ(0.0,textureCoordinates[0]);
    ASSERT_EQ(1.0,textureCoordinates[1]);
    ASSERT_EQ(0.0,textureCoordinates[2]);
    ASSERT_EQ(1.0,textureCoordinates[3]);
    ASSERT_EQ(1u+50u, targetSurfaceDest.x);
    ASSERT_EQ(10u, targetSurfaceDest.width);
    ASSERT_EQ(1u+50u, targetSurfaceDest.y);
    ASSERT_EQ(10u, targetSurfaceDest.height);
}


TEST_F(ViewportTransformTest, doLayerSRCTransformationTest1){
    uint surfaceOriginalWidth = 10;
    uint surfaceOriginalHeight = 10;
    Rectangle targetSurfaceSrc(0,0,10,10);
    Rectangle targetSurfaceDest(20,20,10,10);
    Rectangle layerSRC(25,25,75,75);

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_NEAR(0.5,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_NEAR(0.5,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
    ASSERT_EQ(0u, targetSurfaceDest.x);
    ASSERT_EQ(5u, targetSurfaceDest.width);
    ASSERT_EQ(0u, targetSurfaceDest.y);
    ASSERT_EQ(5u, targetSurfaceDest.height);
}

TEST_F(ViewportTransformTest, doLayerSRCTransformationTest2){
    uint surfaceOriginalWidth = 20;
    uint surfaceOriginalHeight = 20;
    Rectangle targetSurfaceSrc(0,0,20,20);
    Rectangle targetSurfaceDest(30,30,30,30);
    Rectangle layerSRC(25,25,75,75);

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_NEAR(0.0,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_NEAR(0.0,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
    ASSERT_EQ(5u, targetSurfaceDest.x);
    ASSERT_EQ(30u, targetSurfaceDest.width);
    ASSERT_EQ(5u, targetSurfaceDest.y);
    ASSERT_EQ(30u, targetSurfaceDest.height);
}

TEST_F(ViewportTransformTest, doLayerSRCTransformationTest3){
    uint surfaceOriginalWidth = 10;
    uint surfaceOriginalHeight = 10;
    Rectangle targetSurfaceSrc(0,0,10,10);
    Rectangle targetSurfaceDest(70,70,10,10);
    Rectangle layerSRC(25,25,50,50);

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_NEAR(0.0,textureCoordinates[0],0.01);
    ASSERT_NEAR(0.5,textureCoordinates[1],0.01);
    ASSERT_NEAR(0.0,textureCoordinates[2],0.01);
    ASSERT_NEAR(0.5,textureCoordinates[3],0.01);
    ASSERT_EQ(45u, targetSurfaceDest.x);
    ASSERT_EQ(5u, targetSurfaceDest.width);
    ASSERT_EQ(45u, targetSurfaceDest.y);
    ASSERT_EQ(5u, targetSurfaceDest.height);
}

TEST_F(ViewportTransformTest, doLayerSRCTransformationTest4){
    uint surfaceOriginalWidth = 100;
    uint surfaceOriginalHeight = 100;
    Rectangle targetSurfaceSrc(0,0,100,100);
    Rectangle targetSurfaceDest(0,0,100,100);
    Rectangle layerSRC(25,25,50,50);

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_NEAR(0.25,textureCoordinates[0],0.01);
    ASSERT_NEAR(0.75,textureCoordinates[1],0.01);
    ASSERT_NEAR(0.25,textureCoordinates[2],0.01);
    ASSERT_NEAR(0.75,textureCoordinates[3],0.01);
    ASSERT_EQ(0u, targetSurfaceDest.x);
    ASSERT_EQ(50u, targetSurfaceDest.width);
    ASSERT_EQ(0u, targetSurfaceDest.y);
    ASSERT_EQ(50u, targetSurfaceDest.height);
}

TEST_F(ViewportTransformTest, doLayerSRCTransformationTest5){
    uint surfaceOriginalWidth = 10;
    uint surfaceOriginalHeight = 10;
    Rectangle targetSurfaceSrc(0,0,10,10);
    Rectangle targetSurfaceDest(30,30,10,10);
    Rectangle layerSRC(20,20,20,20);
    Rectangle layerDest(0,0,40,40);

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_NEAR(0.0,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_NEAR(0.0,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
    ASSERT_EQ(20u, targetSurfaceDest.x);
    ASSERT_EQ(20u, targetSurfaceDest.width);
    ASSERT_EQ(20u, targetSurfaceDest.y);
    ASSERT_EQ(20u, targetSurfaceDest.height);
}

TEST_F(ViewportTransformTest, doLayerDESTTransformationTest1){
    uint surfaceOriginalWidth = 10;
    uint surfaceOriginalHeight = 10;
    Rectangle targetSurfaceSrc(0,0,10,10);
    Rectangle targetSurfaceDest(0,0,100,100);
    Rectangle layerSRC(0,0,100,100);
    Rectangle layerDest(0,0,100,100);

    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_NEAR(0.0,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_NEAR(0.0,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
    ASSERT_EQ(0u, targetSurfaceDest.x);
    ASSERT_EQ(100u, targetSurfaceDest.width);
    ASSERT_EQ(0u, targetSurfaceDest.y);
    ASSERT_EQ(100u, targetSurfaceDest.height);
}

TEST_F(ViewportTransformTest, doLayerDESTTransformationTest2){
    uint surfaceOriginalWidth = 100;
    uint surfaceOriginalHeight = 100;
    Rectangle targetSurfaceSrc(0,0,100,100);
    Rectangle targetSurfaceDest(0,0,100,100);
    Rectangle layerSRC(0,0,100,100);
    Rectangle layerDest(0,0,200,200);

    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_NEAR(0.0,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_NEAR(0.0,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
    ASSERT_EQ(0u, targetSurfaceDest.x);
    ASSERT_EQ(200u, targetSurfaceDest.width);
    ASSERT_EQ(0u, targetSurfaceDest.y);
    ASSERT_EQ(200u, targetSurfaceDest.height);
}

TEST_F(ViewportTransformTest, doLayerDESTTransformationTest3){
    uint surfaceOriginalWidth = 100;
    uint surfaceOriginalHeight = 100;
    Rectangle targetSurfaceSrc(0,0,100,100);
    Rectangle targetSurfaceDest(0,0,100,100);
    Rectangle layerSRC(0,0,100,100);
    Rectangle layerDest(50,50,200,200);

    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_NEAR(0.0,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_EQ(50u, targetSurfaceDest.x);
    ASSERT_EQ(200u, targetSurfaceDest.width);
    ASSERT_NEAR(0.0,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
    ASSERT_EQ(50u, targetSurfaceDest.y);
    ASSERT_EQ(200u, targetSurfaceDest.height);
}

TEST_F(ViewportTransformTest, doLayerDESTTransformationTest4){
    uint surfaceOriginalWidth = 50;
    uint surfaceOriginalHeight = 50;
    Rectangle targetSurfaceSrc(0,0,50,50);
    Rectangle targetSurfaceDest(50,50,50,50);
    Rectangle layerSrc(50,50,100,100);
    Rectangle layerDest(50,50,200,200);

    ViewportTransform::applyLayerDestination(layerDest,layerSrc,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_NEAR(0.0,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_EQ(150u, targetSurfaceDest.x);
    ASSERT_EQ(100u, targetSurfaceDest.width);
    ASSERT_NEAR(0.0,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
    ASSERT_EQ(150u, targetSurfaceDest.y);
    ASSERT_EQ(100u, targetSurfaceDest.height);
}

TEST_F(ViewportTransformTest, completeExample1){
    int surfaceOriginalWidth =60;
    int surfaceOriginalHeight = 60;
    Rectangle surfaceSRC(20,20,20,20);
    Rectangle surfaceDEST(20,20,60,60);

    Rectangle layerSRC(50,50,50,50);
    Rectangle layerDest(0,0,200,200);

    Rectangle targetSurfaceSrc = surfaceSRC;
    Rectangle targetSurfaceDest = surfaceDEST;

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);

    ASSERT_EQ(30u,targetSurfaceSrc.x);
    ASSERT_EQ(10u,targetSurfaceSrc.width);
    ASSERT_EQ(30u,targetSurfaceSrc.y);
    ASSERT_EQ(10u,targetSurfaceSrc.height);
    ASSERT_NEAR(0.5,textureCoordinates[0],0.01);
    ASSERT_NEAR(0.66,textureCoordinates[1],0.01);
    ASSERT_NEAR(0.5,textureCoordinates[2],0.01);
    ASSERT_NEAR(0.66,textureCoordinates[3],0.01);

    ASSERT_EQ(0u,targetSurfaceDest.x);
    ASSERT_EQ(30u,targetSurfaceDest.width);
    ASSERT_EQ(0u,targetSurfaceDest.y);
    ASSERT_EQ(30u,targetSurfaceDest.height);

    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurfaceDest);
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_EQ(120u,targetSurfaceDest.width);
    ASSERT_EQ(0u,targetSurfaceDest.x);
    ASSERT_NEAR(0.5,textureCoordinates[0],0.01);
    ASSERT_NEAR(0.66,textureCoordinates[1],0.01);
    ASSERT_EQ(120u,targetSurfaceDest.height);
    ASSERT_EQ(0u,targetSurfaceDest.y);
    ASSERT_NEAR(0.5,textureCoordinates[2],0.01);
    ASSERT_NEAR(0.66,textureCoordinates[3],0.01);

}

TEST_F(ViewportTransformTest, completeExample2){
    int surfaceOriginalWidth =100;
    int surfaceOriginalHeight = 100;
    Rectangle surfaceSRC(0,0,100,100);
    Rectangle surfaceDEST(100,100,100,100);

    Rectangle layerDest(0,0,200,200);
    Rectangle layerSRC(100,100,100,100);

    Rectangle targetSurfaceSrc = surfaceSRC;
    Rectangle targetSurfaceDest = surfaceDEST;

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);

    ASSERT_EQ(200u,targetSurfaceDest.width);
    ASSERT_EQ(0u,targetSurfaceDest.x);
    ASSERT_NEAR(0.0,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_EQ(200u,targetSurfaceDest.height);
    ASSERT_EQ(0u,targetSurfaceDest.y);
    ASSERT_NEAR(0.0,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
}

TEST_F(ViewportTransformTest, completeExample3){
    int surfaceOriginalWidth =100;
    int surfaceOriginalHeight = 100;
    Rectangle surfaceSRC(50,50,50,50);
    Rectangle surfaceDEST(100,100,100,100);

    Rectangle layerDest(0,0,200,200);
    Rectangle layerSRC(100,100,100,100);

    Rectangle targetSurfaceSrc = surfaceSRC;
    Rectangle targetSurfaceDest = surfaceDEST;

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);

    ASSERT_EQ(200u,targetSurfaceDest.width);
    ASSERT_EQ(0u,targetSurfaceDest.x);
    ASSERT_NEAR(0.5,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_EQ(200u,targetSurfaceDest.height);
    ASSERT_EQ(0u,targetSurfaceDest.y);
    ASSERT_NEAR(0.5,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
    
}

TEST_F(ViewportTransformTest, completeExample4){
    int surfaceOriginalWidth =100;
    int surfaceOriginalHeight = 100;
    Rectangle surfaceSRC(50,50,50,50);
    Rectangle surfaceDEST(100,100,100,100);

    Rectangle layerDest(50,50,200,200);
    Rectangle layerSRC(100,100,100,100);

    Rectangle targetSurfaceSrc = surfaceSRC;
    Rectangle targetSurfaceDest = surfaceDEST;

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);

    ASSERT_EQ(200u,targetSurfaceDest.width);
    ASSERT_EQ(50u,targetSurfaceDest.x);
    ASSERT_NEAR(0.5,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_EQ(200u,targetSurfaceDest.height);
    ASSERT_EQ(50u,targetSurfaceDest.y);
    ASSERT_NEAR(0.5,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
}

TEST_F(ViewportTransformTest, completeExample5){
    int surfaceOriginalWidth =320;
    int surfaceOriginalHeight = 320;

    Rectangle surfaceSRC(0,0,320,320);
    Rectangle surfaceDEST(0,0,320,320);

    Rectangle layerSRC(100,100,1280,1280);
    Rectangle layerDest(0,0,1280,1280);

    Rectangle targetSurfaceSrc = surfaceSRC;
    Rectangle targetSurfaceDest = surfaceDEST;

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurfaceDest);
    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);

    ASSERT_EQ(0u,targetSurfaceDest.x);
    ASSERT_EQ(220u,targetSurfaceDest.width);
    ASSERT_NEAR(0.3125,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_EQ(0u,targetSurfaceDest.y);
    ASSERT_EQ(220u,targetSurfaceDest.height);
    ASSERT_NEAR(0.3125,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
}

TEST_F(ViewportTransformTest, completeExample6){
    int surfaceOriginalWidth =320;
    int surfaceOriginalHeight =320;
    Rectangle surfaceSRC(0,0,320,320);
    Rectangle surfaceDEST(320,320,320,320);

    Rectangle layerSRC(100,100,1280,1280);
    Rectangle layerDest(0,0,1280,1280);

    Rectangle targetSurfaceSrc = surfaceSRC;
    Rectangle targetSurfaceDest = surfaceDEST;

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurfaceDest);

    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_EQ(220u,targetSurfaceDest.x);
    ASSERT_EQ(320u,targetSurfaceDest.width);
    ASSERT_EQ(220u,targetSurfaceDest.y);
    ASSERT_EQ(320u,targetSurfaceDest.height);

    ASSERT_EQ(0u,targetSurfaceSrc.x);
    ASSERT_EQ(320u,targetSurfaceSrc.width);
    ASSERT_EQ(0u,targetSurfaceSrc.y);
    ASSERT_EQ(320u,targetSurfaceSrc.height);

    ASSERT_NEAR(0.0,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_NEAR(0.0,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
}


TEST_F(ViewportTransformTest, completeExample7){
    int surfaceOriginalWidth =320;
    int surfaceOriginalHeight =240;
    Rectangle surfaceSRC(0,0,320,240);
    Rectangle surfaceDEST(0,0,320,240);

    Rectangle layerSRC(100,0,80,240);
    Rectangle layerDest(100,0,640,480);

    Rectangle targetSurfaceSrc = surfaceSRC;
    Rectangle targetSurfaceDest = surfaceDEST;

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurfaceDest);

    ASSERT_EQ(100u,targetSurfaceSrc.x);
    ASSERT_EQ(80u,targetSurfaceSrc.width);
    ASSERT_EQ(0u,targetSurfaceSrc.y);
    ASSERT_EQ(240u,targetSurfaceSrc.height);

    ASSERT_EQ(100u,targetSurfaceDest.x);
    ASSERT_EQ(640u,targetSurfaceDest.width);
    ASSERT_EQ(0u,targetSurfaceDest.y);
    ASSERT_EQ(480u,targetSurfaceDest.height);

    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_NEAR(0.3125,textureCoordinates[0],0.01);
    ASSERT_NEAR(0.5625,textureCoordinates[1],0.01);
    ASSERT_NEAR(0.0,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
}

TEST_F(ViewportTransformTest, completeExample8){
    int surfaceOriginalWidth =320;
    int surfaceOriginalHeight =240;
    Rectangle surfaceSRC(1,0,320,240);
    Rectangle surfaceDEST(0,0,320,240);

    Rectangle layerSRC(0,0,1280,480);
    Rectangle layerDest(0,0,1280,480);

    Rectangle targetSurfaceSrc = surfaceSRC;
    Rectangle targetSurfaceDest = surfaceDEST;

    ViewportTransform::applyLayerSource(layerSRC,targetSurfaceSrc,targetSurfaceDest);
    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurfaceDest);

    ASSERT_EQ(1u,targetSurfaceSrc.x);
    ASSERT_EQ(320u,targetSurfaceSrc.width);
    ASSERT_EQ(0u,targetSurfaceSrc.y);
    ASSERT_EQ(240u,targetSurfaceSrc.height);

    ASSERT_EQ(0u,targetSurfaceDest.x);
    ASSERT_EQ(320u,targetSurfaceDest.width);
    ASSERT_EQ(0u,targetSurfaceDest.y);
    ASSERT_EQ(240u,targetSurfaceDest.height);

    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurfaceSrc, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_NEAR(0.003125,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_NEAR(0.0,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);
}

TEST_F(ViewportTransformTest, layersourceZoomOnTwoSurfaces){
    int surfaceOriginalWidth = 800;
    int surfaceOriginalHeight = 480;
    Rectangle layerSRC(100, 0, 600, 480);
    Rectangle layerDest(0, 0, 800, 480);

    Rectangle surface1SRC(0, 0, 800, 480);
    Rectangle surface1DEST(0, 0, 400, 480);
    Rectangle targetSurface1Src = surface1SRC;
    Rectangle targetSurface1Dest = surface1DEST;

    ViewportTransform::applyLayerSource(layerSRC,targetSurface1Src,targetSurface1Dest);
    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurface1Dest);

    ASSERT_EQ(200u, targetSurface1Src.x);
    ASSERT_EQ(600u, targetSurface1Src.width);

    ASSERT_EQ(0u,targetSurface1Dest.x);
    ASSERT_EQ(400u,targetSurface1Dest.width);

    float* textureCoordinates = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurface1Src, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates);
    ASSERT_NEAR(0.25,textureCoordinates[0],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[1],0.01);
    ASSERT_NEAR(0.0,textureCoordinates[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates[3],0.01);

    Rectangle surface2SRC(0, 0, 800, 480);
    Rectangle surface2DEST(400, 0, 400, 480);
    Rectangle targetSurface2Src = surface2SRC;
    Rectangle targetSurface2Dest = surface2DEST;

    ViewportTransform::applyLayerSource(layerSRC,targetSurface2Src,targetSurface2Dest);
    ViewportTransform::applyLayerDestination(layerDest,layerSRC,targetSurface2Dest);

    ASSERT_EQ(0u, targetSurface2Src.x);
    ASSERT_EQ(600u, targetSurface2Src.width);

    ASSERT_EQ(400u,targetSurface2Dest.x);
    ASSERT_EQ(400u,targetSurface2Dest.width);

    float* textureCoordinates2 = new float[4];
    ViewportTransform::transformRectangleToTextureCoordinates(targetSurface2Src, surfaceOriginalWidth, surfaceOriginalHeight, textureCoordinates2);
    ASSERT_NEAR(0,textureCoordinates2[0],0.01);
    ASSERT_NEAR(0.75,textureCoordinates2[1],0.01);
    ASSERT_NEAR(0.0,textureCoordinates2[2],0.01);
    ASSERT_NEAR(1.0,textureCoordinates2[3],0.01);
}

TEST_F(ViewportTransformTest, IsFullyCroppedLeft){
    Rectangle surfaceDestination(5,30,5,50);
    Rectangle layerSource(20,0,20,100);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_TRUE(result);
}

TEST_F(ViewportTransformTest, IsFullyCroppedRight){
    Rectangle surfaceDestination(30,30,50,50);
    Rectangle layerSource(100,0,20,100);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_TRUE(result);
}

TEST_F(ViewportTransformTest, IsFullyCroppedAbove){
    Rectangle surfaceDestination(0,5,100,15);
    Rectangle layerSource(0,25,100,80);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_TRUE(result);
}

TEST_F(ViewportTransformTest, IsFullyCroppedBelow){
    Rectangle surfaceDestination(0,110,100,50);
    Rectangle layerSource(0,50,100,50);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_TRUE(result);
}

TEST_F(ViewportTransformTest, IsFullyCroppedLeftBoundary){
    Rectangle surfaceDestination(5,30,5,50);
    Rectangle layerSource(10,0,20,100);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_TRUE(result);
}

TEST_F(ViewportTransformTest, IsFullyCroppedRightBoundary){
    Rectangle surfaceDestination(30,30,50,50);
    Rectangle layerSource(80,0,20,100);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_TRUE(result);
}

TEST_F(ViewportTransformTest, IsFullyCroppedAboveBoundary){
    Rectangle surfaceDestination(0,5,100,15);
    Rectangle layerSource(0,20,100,80);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_TRUE(result);
}

TEST_F(ViewportTransformTest, IsFullyCroppedBelowBoundary){
    Rectangle surfaceDestination(0,100,100,50);
    Rectangle layerSource(0,50,100,50);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_TRUE(result);
}

TEST_F(ViewportTransformTest, IsNotFullyCroppedLeft){
    Rectangle surfaceDestination(0, 20, 20, 10);
    Rectangle layerSource(10, 10, 30, 30);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_FALSE(result);
}

TEST_F(ViewportTransformTest, IsNotFullyCroppedRight){
    Rectangle surfaceDestination(30, 20, 20, 10);
    Rectangle layerSource(10, 10, 30, 30);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_FALSE(result);
}

TEST_F(ViewportTransformTest, IsNotFullyCroppedTop){
    Rectangle surfaceDestination(20, 0, 10, 20);
    Rectangle layerSource(10, 10, 30, 30);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_FALSE(result);
}

TEST_F(ViewportTransformTest, IsNotFullyCroppedBottom){
    Rectangle surfaceDestination(20, 30, 10, 20);
    Rectangle layerSource(10, 10, 30, 30);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_FALSE(result);
}

TEST_F(ViewportTransformTest, IsNotFullyCropped_SurfaceWithinLayerSource){
    Rectangle surfaceDestination(12,12,5,5);
    Rectangle layerSource(10, 10, 10, 10);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_FALSE(result);
}

TEST_F(ViewportTransformTest, IsNotFullyCropped_SurfaceLargerThanLayersource){
    Rectangle surfaceDestination(5,5,10,10);
    Rectangle layerSource(10, 10, 10, 10);

    bool result = ViewportTransform::isFullyCropped(surfaceDestination,layerSource);

    ASSERT_FALSE(result);
}
