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

// SURFACE SOURCE TRANSFORMATION

TEST_F(ViewportTransformTest, surfaceSRCNoCrop1) {
    Rectangle surfaceSRC(0,0,100,100);
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->OriginalSourceWidth = 100;
    surface->OriginalSourceHeight = 100;
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ASSERT_EQ(0.0, x1);
    ASSERT_EQ(1.0, x2);
    ASSERT_EQ(0.0, y1);
    ASSERT_EQ(1.0, y2);
    m_scene->removeSurface(surface);
}
TEST_F(ViewportTransformTest, surfaceSRCNoCrop2){
    Rectangle surfaceSRC(0,0,456,456);
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->OriginalSourceWidth = 456;
    surface->OriginalSourceHeight = 456;        
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ASSERT_EQ(0.0, x1);
    ASSERT_EQ(1.0, x2);
    ASSERT_EQ(0.0, y1);
    ASSERT_EQ(1.0, y2);
    m_scene->removeSurface(surface);
}

TEST_F(ViewportTransformTest, surfaceSRCCropLeft){
    Rectangle surfaceSRC(100,100,300,300);
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->OriginalSourceWidth = 400;
    surface->OriginalSourceHeight = 400;             
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ASSERT_EQ(0.25, x1);
    ASSERT_EQ(1.0, x2);
    ASSERT_EQ(0.25, y1);
    ASSERT_EQ(1.0, y2);
    m_scene->removeSurface(surface);
}

TEST_F(ViewportTransformTest, surfaceSRCCropRight){
    int surfaceOriginalWidth = 5;
    int surfaceOriginalHeight = 5;
    Rectangle surfaceSRC(0,0,2,2);
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;                 
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ASSERT_FLOAT_EQ(0.0, x1);
    ASSERT_FLOAT_EQ(0.4, x2);
    ASSERT_FLOAT_EQ(0.0, y1);
    ASSERT_FLOAT_EQ(0.4, y2);
    m_scene->removeSurface(surface);
}

TEST_F(ViewportTransformTest, surfaceSRCCropFromBothSides){
    int surfaceOriginalWidth = 60;
    int surfaceOriginalHeight = 60;
    Rectangle surfaceSRC(20,20,10,10);
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;                     
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ASSERT_NEAR(0.333, x1,0.01);
    ASSERT_FLOAT_EQ(0.5, x2);
    ASSERT_NEAR(0.333, y1,0.01);
    ASSERT_FLOAT_EQ(0.5, y2);
    m_scene->removeSurface(surface);
}

// SURFACE DESTINATION TRANSFORMATION

TEST_F(ViewportTransformTest, surfaceDESTScaleUp){
    Rectangle surfacePOS(0,0,100,100);
    Rectangle surfaceDEST(0,0,200,200);
    Surface* surface = m_scene->createSurface(0);
    surface->setDestinationRegion(surfaceDEST);
    ViewportTransform::applySurfaceDest(surface,&surfacePOS);
    ASSERT_EQ(0u,surfacePOS.x);
    ASSERT_EQ(200u,surfacePOS.width);
    ASSERT_EQ(0u,surfacePOS.y);
    ASSERT_EQ(200u,surfacePOS.height);
    m_scene->removeSurface(surface);
}

TEST_F(ViewportTransformTest, surfaceDESTScaleDown){
    Rectangle surfacePOS(0,0,100,100);
    Rectangle surfaceDEST(0,0,50,50);
    Surface* surface = m_scene->createSurface(0);
    surface->setDestinationRegion(surfaceDEST);
    ViewportTransform::applySurfaceDest(surface,&surfacePOS);
    ASSERT_EQ(0u,surfacePOS.x);
    ASSERT_EQ(50u,surfacePOS.width);
    ASSERT_EQ(0u,surfacePOS.y);
    ASSERT_EQ(50u,surfacePOS.height);
    m_scene->removeSurface(surface);
    
}

TEST_F(ViewportTransformTest, surfaceDESTScaleUpWithOffset){
    Rectangle surfacePOS(0,0,100,100);
    Rectangle surfaceDEST(20,20,200,200);
    Surface* surface = m_scene->createSurface(0);
    surface->setDestinationRegion(surfaceDEST);
    ViewportTransform::applySurfaceDest(surface,&surfacePOS);
    ASSERT_EQ(20u,surfacePOS.x);
    ASSERT_EQ(200u,surfacePOS.width);
    ASSERT_EQ(20u,surfacePOS.y);
    ASSERT_EQ(200u,surfacePOS.height);
    m_scene->removeSurface(surface);
    
}

TEST_F(ViewportTransformTest, surfaceDESTScaleDownWithOffset){
    Rectangle surfacePOS(0,0,100,100);
    Rectangle surfaceDEST(450,450,50,50);
    Surface* surface = m_scene->createSurface(0);
    surface->setDestinationRegion(surfaceDEST);
    ViewportTransform::applySurfaceDest(surface,&surfacePOS);
    ASSERT_EQ(450u,surfacePOS.x);
    ASSERT_EQ(50u,surfacePOS.width);
    ASSERT_EQ(450u,surfacePOS.y);
    ASSERT_EQ(50u,surfacePOS.height);
    m_scene->removeSurface(surface);
    
}

// LAYER SOURCE TRANSFORMATION
TEST_F(ViewportTransformTest, doLayerSRCSurfaceCompletelyWithin){
    Rectangle surfacePOS(20,20,10,10);
    Rectangle layerSRC(10,10,30,30);
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    // no effect on surfacecontent because its within layersource
    ASSERT_EQ(0.0,x1);
    ASSERT_EQ(1.0,x2);
    ASSERT_EQ(0.0,y1);
    ASSERT_EQ(1.0,y2);
    ASSERT_EQ(10u, surfacePOS.x);
    ASSERT_EQ(10u, surfacePOS.width);
    ASSERT_EQ(10u, surfacePOS.y);
    ASSERT_EQ(10u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, doLayerSRCSurfaceCroppedFromLeft){
    Rectangle surfacePOS(20,20,20,20);
    Rectangle layerSRC(30,30,20,20);
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ASSERT_EQ(0.5,x1);
    ASSERT_EQ(1.0,x2);
    ASSERT_EQ(0.5,y1);
    ASSERT_EQ(1.0,y2);
    ASSERT_EQ(0u, surfacePOS.x);
    ASSERT_EQ(10u, surfacePOS.width);
    ASSERT_EQ(0u, surfacePOS.y);
    ASSERT_EQ(10u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, doLayerSRCSurfaceCroppedFromRight){
    Rectangle surfacePOS(20,20,20,20);
    Rectangle layerSRC(0,0,35,35);
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ASSERT_EQ(0,x1);
    ASSERT_EQ(0.75,x2);
    ASSERT_EQ(0,y1);
    ASSERT_EQ(0.75,y2);
    ASSERT_EQ(20u, surfacePOS.x);
    ASSERT_EQ(15u, surfacePOS.width);
    ASSERT_EQ(20u, surfacePOS.y);
    ASSERT_EQ(15u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, doLayerSRCSurfaceCroppedFromBothSides){
    Rectangle surfacePOS(20,20,20,20);
    Rectangle layerSRC(25,25,10,10);
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ASSERT_EQ(0.25,x1);
    ASSERT_EQ(0.75,x2);
    ASSERT_EQ(0.25,y1);
    ASSERT_EQ(0.75,y2);
    ASSERT_EQ(0u, surfacePOS.x);
    ASSERT_EQ(10u, surfacePOS.width);
    ASSERT_EQ(0u, surfacePOS.y);
    ASSERT_EQ(10u, surfacePOS.height);
    delete layer;
}

	// LAYER DESTINATION TRANSFORMATION

TEST_F(ViewportTransformTest, doLayerDESTScaleUp){
    Rectangle surfacePOS(10,10,100,100); //      from 20 to 75
    Rectangle layerORIG(0,0,200,200);
    Rectangle layerDest(50,50,600,600);
    Layer *layer = new Layer();
    layer->OriginalSourceWidth = layerORIG.width;        
    layer->OriginalSourceHeight = layerORIG.height;        
    layer->setSourceRegion(layerORIG);
    layer->setDestinationRegion(layerDest);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerDest(&surfacePOS,layer);
    ASSERT_EQ(0.0,x1);
    ASSERT_EQ(1.0,x2);
    ASSERT_EQ(0.0,y1);
    ASSERT_EQ(1.0,y2);
    ASSERT_EQ(30u+50u, surfacePOS.x);
    ASSERT_EQ(300u, surfacePOS.width);
    ASSERT_EQ(30u+50u, surfacePOS.y);
    ASSERT_EQ(300u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, doLayerDESTScaleDown){
    Rectangle surfacePOS(10,10,100,100); //      from 20 to 75
    Rectangle layerORIG(0,0,200,200);
    Rectangle layerDest(50,50,20,20);
    Layer *layer = new Layer();
    layer->OriginalSourceWidth = layerORIG.width;        
    layer->OriginalSourceHeight = layerORIG.height;          
    layer->setSourceRegion(layerORIG);
    layer->setDestinationRegion(layerDest);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerDest(&surfacePOS,layer);
    ASSERT_EQ(0.0,x1);
    ASSERT_EQ(1.0,x2);
    ASSERT_EQ(0.0,y1);
    ASSERT_EQ(1.0,y2);
    ASSERT_EQ(1u+50u, surfacePOS.x);
    ASSERT_EQ(10u, surfacePOS.width);
    ASSERT_EQ(1u+50u, surfacePOS.y);
    ASSERT_EQ(10u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, doSurfaceSrcTransformationTest){
    int surfaceOriginalWidth =100;
    int surfaceOriginalHeight = 100;
    Rectangle surfaceSRC(25,25,50,50);
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;          
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);		
    ASSERT_NEAR(0.25, x1, 0.01);
    ASSERT_NEAR(0.75, x2, 0.01);
    ASSERT_NEAR(0.25, y1, 0.01);
    ASSERT_NEAR(0.75, y2, 0.01);
    m_scene->removeSurface(surface);
    
}

TEST_F(ViewportTransformTest, doSurfaceSrcTransformationTest2){
    int surfaceOriginalWidth =100;
    int surfaceOriginalHeight = 100;
    Rectangle surfaceSRC(50,50,25,25);
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;          
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ASSERT_NEAR(0.5, x1, 0.01);
    ASSERT_NEAR(0.75, x2, 0.01);
    ASSERT_NEAR(0.5, y1, 0.01);
    ASSERT_NEAR(0.75, y2, 0.01);
    m_scene->removeSurface(surface);
    
}

TEST_F(ViewportTransformTest, doSurfaceSrcTransformationTest3){
    int surfaceOriginalWidth =100;
    int surfaceOriginalHeight = 100;
    Rectangle surfaceSRC(0,0,100,0);
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;          
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ASSERT_NEAR(0.0, x1, 0.01);
    ASSERT_NEAR(1.0, x2, 0.01);
    m_scene->removeSurface(surface);
    
}

TEST_F(ViewportTransformTest, doSurfaceSrcTransformationTest4){
    int surfaceOriginalWidth = 60;
    int surfaceOriginalHeight = 60;
    Rectangle surfaceSRC(20,20,20,20);
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;          
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ASSERT_NEAR(0.33, x1, 0.01);
    ASSERT_NEAR(0.66, x2, 0.01);
    ASSERT_NEAR(0.33, y1, 0.01);
    ASSERT_NEAR(0.66, y2, 0.01);
    m_scene->removeSurface(surface);
    
}

TEST_F(ViewportTransformTest, doSurfaceSrcTransformationTest5){
    int surfaceOriginalWidth = 60;
    int surfaceOriginalHeight = 60;
    Rectangle surfaceSRC(20,20,20,20);
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;          
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);		ASSERT_NEAR(0.33, x1, 0.01);
    ASSERT_NEAR(0.66, x2, 0.01);
    ASSERT_NEAR(0.33, y1, 0.01);
    ASSERT_NEAR(0.66, y2, 0.01);
    m_scene->removeSurface(surface);
    
}

TEST_F(ViewportTransformTest, doSurfaceDESTTransformationTest1){
    Rectangle surfacePOS(0,0,100,100);
    Rectangle surfaceDEST(0,0,200,200);
    Surface* surface = m_scene->createSurface(0);
    surface->setDestinationRegion(surfaceDEST);
    ViewportTransform::applySurfaceDest(surface,&surfacePOS);
    ASSERT_EQ(0u,surfacePOS.x);
    ASSERT_EQ(200u,surfacePOS.width);
    ASSERT_EQ(0u,surfacePOS.y);
    ASSERT_EQ(200u,surfacePOS.height);
    m_scene->removeSurface(surface);
    
}

TEST_F(ViewportTransformTest, doSurfaceDESTTransformationTest2){
    Rectangle surfacePOS(0,0,100,100);
    Rectangle surfaceDEST(25,25,50,50);
    Surface* surface = m_scene->createSurface(0);
    surface->setDestinationRegion(surfaceDEST);
    ViewportTransform::applySurfaceDest(surface,&surfacePOS);
    ASSERT_EQ(25u,surfacePOS.x);
    ASSERT_EQ(50u,surfacePOS.width);
    ASSERT_EQ(25u,surfacePOS.y);
    ASSERT_EQ(50u,surfacePOS.height);
    m_scene->removeSurface(surface);
    
}

TEST_F(ViewportTransformTest, doLayerSRCTransformationTest1){
    Rectangle surfacePOS(20,20,10,10);
    Rectangle layerSRC(25,25,75,75);
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ASSERT_NEAR(0.5,x1,0.01);
    ASSERT_NEAR(1.0,x2,0.01);
    ASSERT_NEAR(0.5,y1,0.01);
    ASSERT_NEAR(1.0,y2,0.01);
    ASSERT_EQ(0u, surfacePOS.x);
    ASSERT_EQ(5u, surfacePOS.width);
    ASSERT_EQ(0u, surfacePOS.y);
    ASSERT_EQ(5u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, doLayerSRCTransformationTest2){
    Rectangle surfacePOS(30,30,30,30); // from 30 to 60
    Rectangle layerSRC(25,25,75,75);	// from 25 to 75
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ASSERT_NEAR(0.0,x1,0.01);
    ASSERT_NEAR(1.0,x2,0.01);
    ASSERT_NEAR(0.0,y1,0.01);
    ASSERT_NEAR(1.0,y2,0.01);
    ASSERT_EQ(5u, surfacePOS.x);
    ASSERT_EQ(30u, surfacePOS.width);
    ASSERT_EQ(5u, surfacePOS.y);
    ASSERT_EQ(30u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, doLayerSRCTransformationTest3){
    Rectangle surfacePOS(70,70,10,10); //      from 70 to 80
    Rectangle layerSRC(25,25,50,50); // from 25 to 75
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);		ASSERT_NEAR(0.0,x1,0.01);
    ASSERT_NEAR(0.5,x2,0.01);
    ASSERT_NEAR(0.0,y1,0.01);
    ASSERT_NEAR(0.5,y2,0.01);
    ASSERT_EQ(45u, surfacePOS.x);
    ASSERT_EQ(5u, surfacePOS.width);
    ASSERT_EQ(45u, surfacePOS.y);
    ASSERT_EQ(5u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, doLayerSRCTransformationTest4){
    Rectangle surfacePOS(0,0,100,100); //      from 20 to 75
    Rectangle layerSRC(25,25,50,50); // from 25 to 75
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ASSERT_NEAR(0.25,x1,0.01);
    ASSERT_NEAR(0.75,x2,0.01);
    ASSERT_NEAR(0.25,y1,0.01);
    ASSERT_NEAR(0.75,y2,0.01);
    ASSERT_EQ(0u, surfacePOS.x);
    ASSERT_EQ(50u, surfacePOS.width);
    ASSERT_EQ(0u, surfacePOS.y);
    ASSERT_EQ(50u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, doLayerSRCTransformationTest5){
    Rectangle surfacePOS(30,30,10,10); //      from 20 to 75
    Rectangle layerSRC(20,20,20,20); // from 25 to 75
    Rectangle layerDest(0,0,40,40);
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    layer->OriginalSourceWidth = layerSRC.width;
    layer->OriginalSourceHeight = layerSRC.height;
    layer->setDestinationRegion(layerDest);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ViewportTransform::applyLayerDest(&surfacePOS,layer);
    ASSERT_NEAR(0.0,x1,0.01);
    ASSERT_NEAR(1.0,x2,0.01);
    ASSERT_NEAR(0.0,y1,0.01);
    ASSERT_NEAR(1.0,y2,0.01);
    ASSERT_EQ(20u, surfacePOS.x);
    ASSERT_EQ(20u, surfacePOS.width);
    ASSERT_EQ(20u, surfacePOS.y);
    ASSERT_EQ(20u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, doLayerDESTTransformationTest1){
    Rectangle surfacePOS(0,0,100,100); //      from 20 to 75
    Rectangle layerORIG(0,0,100,100);
    Rectangle layerDest(0,0,100,100);
    Layer *layer = new Layer();
    layer->OriginalSourceWidth = layerORIG.width;
    layer->OriginalSourceHeight = layerORIG.height;
    layer->setSourceRegion(layerORIG);
    layer->setDestinationRegion(layerDest);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerDest(&surfacePOS,layer);
    ASSERT_NEAR(0.0,x1,0.01);
    ASSERT_NEAR(1.0,x2,0.01);
    ASSERT_NEAR(0.0,y1,0.01);
    ASSERT_NEAR(1.0,y2,0.01);
    ASSERT_EQ(0u, surfacePOS.x);
    ASSERT_EQ(100u, surfacePOS.width);
    ASSERT_EQ(0u, surfacePOS.y);
    ASSERT_EQ(100u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, doLayerDESTTransformationTest2){
    Rectangle surfacePOS(0,0,100,100); //      from 20 to 75
    Rectangle layerORIG(0,0,100,100);
    Rectangle layerDest(0,0,200,200);
    Layer *layer = new Layer();
    layer->OriginalSourceWidth = layerORIG.width;
    layer->OriginalSourceHeight = layerORIG.height;
    layer->setSourceRegion(layerORIG);
    layer->setDestinationRegion(layerDest);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerDest(&surfacePOS,layer);
    ASSERT_NEAR(0.0,x1,0.01);
    ASSERT_NEAR(1.0,x2,0.01);
    ASSERT_NEAR(0.0,y1,0.01);
    ASSERT_NEAR(1.0,y2,0.01);
    ASSERT_EQ(0u, surfacePOS.x);
    ASSERT_EQ(200u, surfacePOS.width);
    ASSERT_EQ(0u, surfacePOS.y);
    ASSERT_EQ(200u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, doLayerDESTTransformationTest3){
    Rectangle surfacePOS(0,0,100,100); //      from 20 to 75
    Rectangle layerORIG(0,0,100,100);
    Rectangle layerDest(50,50,200,200);
    Layer *layer = new Layer();
    layer->OriginalSourceWidth = layerORIG.width;
    layer->OriginalSourceHeight = layerORIG.height;
    layer->setSourceRegion(layerORIG);
    layer->setDestinationRegion(layerDest);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerDest(&surfacePOS,layer);

    ASSERT_NEAR(0.0,x1,0.01);
    ASSERT_NEAR(1.0,x2,0.01);
    ASSERT_EQ(50u, surfacePOS.x);
    ASSERT_EQ(200u, surfacePOS.width);
    ASSERT_NEAR(0.0,y1,0.01);
    ASSERT_NEAR(1.0,y2,0.01);
    ASSERT_EQ(50u, surfacePOS.y);
    ASSERT_EQ(200u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, doLayerDESTTransformationTest4){
    Rectangle surfacePOS(50,50,50,50); //      from 20 to 75
    Rectangle layerORIG(0,0,100,100);
    Rectangle layerDest(50,50,200,200);
    Layer *layer = new Layer();
    layer->OriginalSourceWidth = layerORIG.width;
    layer->OriginalSourceHeight = layerORIG.height;
    layer->setSourceRegion(layerORIG);    
    layer->setDestinationRegion(layerDest);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    ViewportTransform::applyLayerDest(&surfacePOS,layer);
    ASSERT_NEAR(0.0,x1,0.01);
    ASSERT_NEAR(1.0,x2,0.01);
    ASSERT_EQ(150u, surfacePOS.x);
    ASSERT_EQ(100u, surfacePOS.width);
    ASSERT_NEAR(0.0,y1,0.01);
    ASSERT_NEAR(1.0,y2,0.01);
    ASSERT_EQ(150u, surfacePOS.y);
    ASSERT_EQ(100u, surfacePOS.height);
    delete layer;
}

TEST_F(ViewportTransformTest, completeExample1){
    int surfaceOriginalWidth =60;
    int surfaceOriginalHeight = 60;
    Rectangle surfacePOS(0,0,surfaceOriginalWidth,surfaceOriginalHeight);
    Rectangle surfaceSRC(20,20,20,20); 
    Rectangle surfaceDEST(20,20,60,60);
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->setDestinationRegion(surfaceDEST);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;         
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;

    Rectangle layerORIG(0,0,100,100);
    Rectangle layerDest(0,0,200,200);
    Rectangle layerSRC(50,50,50,50);
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    layer->setDestinationRegion(layerDest);
    layer->OriginalSourceWidth = layerORIG.width;
    layer->OriginalSourceHeight = layerORIG.height;

    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ASSERT_NEAR(0.33,x1,0.01);
    ASSERT_NEAR(0.66,x2,0.01);
    ASSERT_NEAR(0.33,y1,0.01);
    ASSERT_NEAR(0.66,y2,0.01);

    ViewportTransform::applySurfaceDest(surface,&surfacePOS);
    ASSERT_EQ(60u,surfacePOS.width);
    ASSERT_EQ(20u,surfacePOS.x);
    ASSERT_NEAR(0.33,x1,0.01);
    ASSERT_NEAR(0.66,x2,0.01);
    ASSERT_EQ(60u,surfacePOS.height);
    ASSERT_EQ(20u,surfacePOS.y);
    ASSERT_NEAR(0.33,y1,0.01);
    ASSERT_NEAR(0.66,y2,0.01);    
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ASSERT_EQ(30u,surfacePOS.width);
    ASSERT_EQ(0u,surfacePOS.x);
    ASSERT_NEAR(0.5,x1,0.01);
    ASSERT_NEAR(0.66,x2,0.01);
    ASSERT_EQ(30u,surfacePOS.height);
    ASSERT_EQ(0u,surfacePOS.y);
    ASSERT_NEAR(0.5,y1,0.01);
    ASSERT_NEAR(0.66,y2,0.01);

    ViewportTransform::applyLayerDest(&surfacePOS,layer);
    ASSERT_EQ(120u,surfacePOS.width);
    ASSERT_EQ(0u,surfacePOS.x);
    ASSERT_NEAR(0.5,x1,0.01);
    ASSERT_NEAR(0.66,x2,0.01);
    ASSERT_EQ(120u,surfacePOS.height);
    ASSERT_EQ(0u,surfacePOS.y);
    ASSERT_NEAR(0.5,y1,0.01);
    ASSERT_NEAR(0.66,y2,0.01);
    m_scene->removeSurface(surface);
    
    delete layer;

}

TEST_F(ViewportTransformTest, completeExample2){
    int surfaceOriginalWidth =100;
    int surfaceOriginalHeight = 100;
    Rectangle surfacePOS(0,0,surfaceOriginalWidth,surfaceOriginalHeight);
    Rectangle surfaceSRC(0,0,100,100);
    Rectangle surfaceDEST(100,100,100,100);
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->setDestinationRegion(surfaceDEST);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;        
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;

    Rectangle layerDest(0,0,200,200);
    Rectangle layerSRC(100,100,100,100);
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    layer->setDestinationRegion(layerDest);
    layer->OriginalSourceWidth = layerSRC.width;
    layer->OriginalSourceHeight = layerSRC.height;    
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ViewportTransform::applySurfaceDest(surface,&surfacePOS);
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ViewportTransform::applyLayerDest(&surfacePOS,layer);
    ASSERT_EQ(200u,surfacePOS.width);
    ASSERT_EQ(0u,surfacePOS.x);
    ASSERT_NEAR(0.0,x1,0.01);
    ASSERT_NEAR(1.0,x2,0.01);
    ASSERT_EQ(200u,surfacePOS.height);
    ASSERT_EQ(0u,surfacePOS.y);
    ASSERT_NEAR(0.0,y1,0.01);
    ASSERT_NEAR(1.0,y2,0.01);
    m_scene->removeSurface(surface);
    
    delete layer;
}

TEST_F(ViewportTransformTest, completeExample3){
    int surfaceOriginalWidth =100;
    int surfaceOriginalHeight = 100;
    Rectangle surfacePOS(0,0,surfaceOriginalWidth,0);
    Rectangle surfaceSRC(50,50,50,50);
    Rectangle surfaceDEST(100,100,100,100);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->setDestinationRegion(surfaceDEST);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;
    Rectangle layerDest(0,0,200,200);
    Rectangle layerSRC(100,100,100,100);
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    layer->setDestinationRegion(layerDest);
    layer->OriginalSourceWidth = layerSRC.width;
    layer->OriginalSourceHeight = layerSRC.height;    
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ViewportTransform::applySurfaceDest(surface,&surfacePOS);
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ViewportTransform::applyLayerDest(&surfacePOS,layer);
    ASSERT_EQ(200u,surfacePOS.width);
    ASSERT_EQ(0u,surfacePOS.x);
    ASSERT_NEAR(0.5,x1,0.01);
    ASSERT_NEAR(1.0,x2,0.01);
    ASSERT_EQ(200u,surfacePOS.height);
    ASSERT_EQ(0u,surfacePOS.y);
    ASSERT_NEAR(0.5,y1,0.01);
    ASSERT_NEAR(1.0,y2,0.01);
    delete layer;
    m_scene->removeSurface(surface);
    
}

TEST_F(ViewportTransformTest, completeExample4){
    int surfaceOriginalWidth =100;
    int surfaceOriginalHeight = 100;
    Rectangle surfacePOS(0,0,surfaceOriginalWidth,surfaceOriginalHeight);
    Rectangle surfaceSRC(50,50,50,50);
    Rectangle surfaceDEST(100,100,100,100);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->setDestinationRegion(surfaceDEST);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;
    Rectangle layerDest(50,50,200,200);
    Rectangle layerSRC(100,100,100,100);

    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    layer->setDestinationRegion(layerDest);
    layer->OriginalSourceWidth = layerSRC.width;
    layer->OriginalSourceHeight = layerSRC.height;    
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ViewportTransform::applySurfaceDest(surface,&surfacePOS);
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ViewportTransform::applyLayerDest(&surfacePOS,layer);
    ASSERT_EQ(200u,surfacePOS.width);
    ASSERT_EQ(50u,surfacePOS.x);
    ASSERT_NEAR(0.5,x1,0.01);
    ASSERT_NEAR(1.0,x2,0.01);
    ASSERT_EQ(200u,surfacePOS.height);
    ASSERT_EQ(50u,surfacePOS.y);
    ASSERT_NEAR(0.5,y1,0.01);
    ASSERT_NEAR(1.0,y2,0.01);
    m_scene->removeSurface(surface);
    
    delete layer;
}

TEST_F(ViewportTransformTest, completeExample5){
    int surfaceOriginalWidth =320;
    int surfaceOriginalHeight = 320;
    Rectangle surfacePOS(0,0,surfaceOriginalWidth,surfaceOriginalHeight);
    Rectangle surfaceSRC(0,0,320,320);
    Rectangle surfaceDEST(0,0,320,320);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->setDestinationRegion(surfaceDEST);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;
    Rectangle layerDest(0,0,1280,1280);
    Rectangle layerSRC(100,100,1280,1280);
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    layer->setDestinationRegion(layerDest);
    layer->OriginalSourceWidth = layerSRC.width;
    layer->OriginalSourceHeight = layerSRC.height;     
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ViewportTransform::applySurfaceDest(surface,&surfacePOS);
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ViewportTransform::applyLayerDest(&surfacePOS,layer);
    ASSERT_EQ(0u,surfacePOS.x);
    ASSERT_EQ(220u,surfacePOS.width);
    ASSERT_NEAR(0.3125,x1,0.01);
    ASSERT_NEAR(1.0,x2,0.01);
    ASSERT_EQ(0u,surfacePOS.y);
    ASSERT_EQ(220u,surfacePOS.height);
    ASSERT_NEAR(0.3125,y1,0.01);
    ASSERT_NEAR(1.0,y2,0.01);
    m_scene->removeSurface(surface);
    
    delete layer;
}

TEST_F(ViewportTransformTest, completeExample6){
    int surfaceOriginalWidth =320;
    int surfaceOriginalHeight =320;
    Rectangle surfacePOS(0,0,surfaceOriginalWidth,surfaceOriginalHeight);
    Rectangle surfaceSRC(0,0,320,320);
    Rectangle surfaceDEST(320,320,320,320);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->setDestinationRegion(surfaceDEST);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;
    Rectangle layerDest(0,0,1280,1280);
    Rectangle layerSRC(100,100,1280,1280);
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    layer->OriginalSourceWidth = layerSRC.width;
    layer->OriginalSourceHeight = layerSRC.height;     
    layer->setDestinationRegion(layerDest);
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ViewportTransform::applySurfaceDest(surface,&surfacePOS);
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ViewportTransform::applyLayerDest(&surfacePOS,layer);
    ASSERT_EQ(220u,surfacePOS.x);
    ASSERT_EQ(320u,surfacePOS.width);
    ASSERT_NEAR(0.0,x1,0.01);
    ASSERT_NEAR(1.0,x2,0.01);
    ASSERT_EQ(220u,surfacePOS.y);
    ASSERT_EQ(320u,surfacePOS.height);
    ASSERT_NEAR(0.0,y1,0.01);
    ASSERT_NEAR(1.0,y2,0.01);
    m_scene->removeSurface(surface);
    
    delete layer;
}
TEST_F(ViewportTransformTest, completeExample7){
    int surfaceOriginalWidth =320;
    int surfaceOriginalHeight =240;
    Rectangle surfacePOS(0,0,surfaceOriginalWidth,surfaceOriginalHeight);
    Rectangle surfaceSRC(0,0,320,240);
    Rectangle surfaceDEST(0,0,320,240);
    float x1 = 0.0;
    float x2 = 1.0;
    float y1 = 0.0;
    float y2 = 1.0;
    Surface* surface = m_scene->createSurface(0);
    surface->setSourceRegion(surfaceSRC);
    surface->setDestinationRegion(surfaceDEST);
    surface->OriginalSourceWidth = surfaceOriginalWidth;
    surface->OriginalSourceHeight = surfaceOriginalHeight;
    Rectangle layerDest(100,0,640,480);
    Rectangle layerSRC(100,0,80,240);
    Rectangle layerORIG(0,0,1280,480);
    Layer *layer = new Layer();
    layer->setSourceRegion(layerSRC);
    layer->OriginalSourceWidth = layerORIG.width;
    layer->OriginalSourceHeight = layerORIG.height;     
    layer->setDestinationRegion(layerDest);
    ViewportTransform::applySurfaceSource(surface,&x1,&x2,&y1,&y2);
    ViewportTransform::applySurfaceDest(surface,&surfacePOS);
    ViewportTransform::applyLayerSource(&surfacePOS,layer,&x1,&x2,&y1,&y2);
    ViewportTransform::applyLayerDest(&surfacePOS,layer);
    ASSERT_EQ(100u,surfacePOS.x);
    ASSERT_EQ(640u,surfacePOS.width);
    ASSERT_NEAR(0.3125,x1,0.01);
    ASSERT_NEAR(0.5625,x2,0.01);
    ASSERT_EQ(0u,surfacePOS.y);
    ASSERT_EQ(480u,surfacePOS.height);
    ASSERT_NEAR(0.0,y1,0.01);
    ASSERT_NEAR(1.0,y2,0.01);
    m_scene->removeSurface(surface);    
    delete layer;
}
