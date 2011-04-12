/***************************************************************************
 *
 * Copyright 2010,2011 BMW Car IT GmbH
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

/* METHODS THAT ARE CURRENTLY NOT TESTED:
 *
 *	 ilm_displaySetRenderOrder
 *	 ilm_surfaceInvalidateRectangle
 *	 ilm_surfaceSetChromaKey
 *	 ilm_layerSetChromaKey
 *	 ilm_layerSetRenderOrder
 *	 ilm_getNumberOfHardwareLayers
 *	 ilm_layerGetType(t_ilm_layer layerId,ilmLayerType* layerType);
	 ilm_layerGetCapabilities(t_ilm_layer layerId, t_ilm_layercapabilities *capabilities);
	 ilm_layerTypeGetCapabilities(ilmLayerType layerType, t_ilm_layercapabilities *capabilities);
 *
 * */

#include <gtest/gtest.h>
#include <stdio.h>

extern "C" {
	#include "ilm_client.h"
}

class IlmCommandTest : public ::testing::Test {
public:
	IlmCommandTest(){
	}

	void SetUp() {
		ilm_init();
	 }
	void TearDown() {
		removeAll();
		ilm_destroy();
	}

	void removeAll(){
		t_ilm_layer* layers;
		t_ilm_int numLayer;
		ilm_getLayerIDs(&numLayer, &layers);
		for (t_ilm_int i=0; i<numLayer; i++ ){
			ilm_layerRemove(layers[i]);
		};

		t_ilm_surface* surfaces;
		t_ilm_int numSurfaces;
		ilm_getSurfaceIDs(&numSurfaces, &surfaces);
		for (t_ilm_int i=0; i<numSurfaces; i++ ){
			ilm_surfaceRemove(surfaces[i]);
		};

		t_ilm_layergroup* layergroups;
		t_ilm_int numLayergroup;
		ilm_getLayerGroupIDs(&numLayergroup, &layergroups);
		for (t_ilm_int i=0; i<numLayergroup; i++ ){
			ilm_layergroupRemove(layergroups[i]);
		};

		t_ilm_surfacegroup* surfacegroups;
		t_ilm_int numSurfacegroup;
		ilm_getSurfaceGroupIDs(&numSurfacegroup, &surfacegroups);
		for (t_ilm_int i=0; i<numSurfacegroup; i++ ){
			ilm_surfacegroupRemove(surfacegroups[i]);
		};
		ilm_commitChanges();
	  }

};

TEST_F(IlmCommandTest, SetGetSurfaceDimension) {
	uint surface = 36;

	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface);

	t_ilm_uint dim[2] = {15,25};
	ilm_surfaceSetDimension(surface,dim);
	ilm_commitChanges();

	t_ilm_uint dimreturned[2];
	ilm_surfaceGetDimension(surface,dimreturned);
	ASSERT_EQ(dim[0],dimreturned[0]);
	ASSERT_EQ(dim[1],dimreturned[1]);
}

TEST_F(IlmCommandTest, SetGetLayerDimension) {
	uint layer = 4316;

	ilm_layerCreate(&layer);

	t_ilm_uint dim[2] = {115,125};
	ilm_layerSetDimension(layer,dim);
	ilm_commitChanges();

	t_ilm_uint dimreturned[2];
	ilm_layerGetDimension(layer,dimreturned);
	ASSERT_EQ(dim[0],dimreturned[0]);
	ASSERT_EQ(dim[1],dimreturned[1]);
}

TEST_F(IlmCommandTest, SetGetSurfacePosition) {
	uint surface = 36;

	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface);

	t_ilm_uint pos[2] = {15,25};
	ilm_surfaceSetPosition(surface,pos);
	ilm_commitChanges();

	t_ilm_uint posreturned[2];
	ilm_surfaceGetPosition(surface,posreturned);
	ASSERT_EQ(pos[0],posreturned[0]);
	ASSERT_EQ(pos[1],posreturned[1]);
}

TEST_F(IlmCommandTest, SetGetLayerPosition) {
	uint layer = 4316;

	ilm_layerCreate(&layer);

	t_ilm_uint pos[2] = {115,125};
	ilm_layerSetPosition(layer,pos);
	ilm_commitChanges();

	t_ilm_uint posreturned[2];
	ilm_layerGetPosition(layer,posreturned);
	ASSERT_EQ(pos[0],posreturned[0]);
	ASSERT_EQ(pos[1],posreturned[1]);
}

TEST_F(IlmCommandTest, SetGetSurfaceOrientation) {
	uint surface = 36;
	ilmOrientation returned;
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface);

	ilm_surfaceSetOrientation(surface,ILM_NINETY);
	ilm_commitChanges();
	ilm_surfaceGetOrientation(surface,&returned);
	ASSERT_EQ(ILM_NINETY,returned);

	ilm_surfaceSetOrientation(surface,ILM_ONEHUNDREDEIGHTY);
	ilm_commitChanges();
	ilm_surfaceGetOrientation(surface,&returned);
	ASSERT_EQ(ILM_ONEHUNDREDEIGHTY,returned);

	ilm_surfaceSetOrientation(surface,ILM_TWOHUNDREDSEVENTY);
	ilm_commitChanges();
	ilm_surfaceGetOrientation(surface,&returned);
	ASSERT_EQ(ILM_TWOHUNDREDSEVENTY,returned);

	ilm_surfaceSetOrientation(surface,ILM_ZERO);
	ilm_commitChanges();
	ilm_surfaceGetOrientation(surface,&returned);
	ASSERT_EQ(ILM_ZERO,returned);
}

TEST_F(IlmCommandTest, SetGetLayerOrientation) {
	uint layer = 4316;
	ilm_layerCreate(&layer);
	ilm_commitChanges();
	ilmOrientation returned;

	ilm_layerSetOrientation(layer,ILM_NINETY);
	ilm_commitChanges();
	ilm_layerGetOrientation(layer,&returned);
	ASSERT_EQ(ILM_NINETY,returned);

	ilm_layerSetOrientation(layer,ILM_ONEHUNDREDEIGHTY);
	ilm_commitChanges();
	ilm_layerGetOrientation(layer,&returned);
	ASSERT_EQ(ILM_ONEHUNDREDEIGHTY,returned);

	ilm_layerSetOrientation(layer,ILM_TWOHUNDREDSEVENTY);
	ilm_commitChanges();
	ilm_layerGetOrientation(layer,&returned);
	ASSERT_EQ(ILM_TWOHUNDREDSEVENTY,returned);

	ilm_layerSetOrientation(layer,ILM_ZERO);
	ilm_commitChanges();
	ilm_layerGetOrientation(layer,&returned);
	ASSERT_EQ(ILM_ZERO,returned);
}

TEST_F(IlmCommandTest, SetGetSurfaceOpacity) {
	uint surface1 = 36;
	uint surface2 = 44;
	t_ilm_float opacity;

	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface1);
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface2);

	ilm_surfaceSetOpacity(surface1,0.88);
	ilm_commitChanges();
	ilm_surfaceGetOpacity(surface1,&opacity);
	ASSERT_DOUBLE_EQ(0.88, opacity);

	ilm_surfaceSetOpacity(surface2,0.001);
	ilm_commitChanges();
	ilm_surfaceGetOpacity(surface2,&opacity);
	ASSERT_DOUBLE_EQ(0.001, opacity);
}

TEST_F(IlmCommandTest, SetGetLayerOpacity) {
	uint layer1 = 36;
	uint layer2 = 44;
	t_ilm_float opacity;

	ilm_layerCreate(&layer1);
	ilm_layerCreate(&layer2);

	ilm_layerSetOpacity(layer1,0.88);
	ilm_commitChanges();
	ilm_layerGetOpacity(layer1,&opacity);
	ASSERT_DOUBLE_EQ(0.88, opacity);

	ilm_layerSetOpacity(layer2,0.001);
	ilm_commitChanges();
	ilm_layerGetOpacity(layer2,&opacity);
	ASSERT_DOUBLE_EQ(0.001, opacity);
}

TEST_F(IlmCommandTest, SetGetLayerGroupOpacity) {
	uint layer1 = 36;
	uint layer2 = 44;
	uint group = 99;
	t_ilm_float opacity;

	ilm_layerCreate(&layer1);
	ilm_layerCreate(&layer2);
	ilm_layergroupCreate(&group);
	ilm_layergroupAddLayer(group, layer1);
	ilm_layergroupAddLayer(group, layer2);
	ilm_commitChanges();

	ilm_layergroupSetOpacity(group,0.88);
	ilm_commitChanges();
	ilm_layerGetOpacity(layer1,&opacity);
	ASSERT_DOUBLE_EQ(0.88, opacity);
	ilm_layerGetOpacity(layer2,&opacity);
	ASSERT_DOUBLE_EQ(0.88, opacity);

	ilm_layergroupSetOpacity(group,0.001);
	ilm_commitChanges();
	ilm_layerGetOpacity(layer1,&opacity);
	ASSERT_DOUBLE_EQ(0.001, opacity);
	ilm_layerGetOpacity(layer2,&opacity);
	ASSERT_DOUBLE_EQ(0.001, opacity);
}

TEST_F(IlmCommandTest, SetGetSurfaceGroupOpacity) {
	uint surface1 = 36;
	uint surface2 = 44;
	uint group = 99;
	t_ilm_float opacity;

	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface1);
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface2);
	ilm_surfacegroupCreate(&group);

	ilm_surfacegroupAddSurface(group,surface1);
	ilm_surfacegroupAddSurface(group,surface2);

	ilm_surfacegroupSetOpacity(group,0.88);
	ilm_commitChanges();
	ilm_surfaceGetOpacity(surface1,&opacity);
	ASSERT_DOUBLE_EQ(0.88, opacity);
	ilm_surfaceGetOpacity(surface2,&opacity);
	ASSERT_DOUBLE_EQ(0.88, opacity);

	ilm_surfacegroupSetOpacity(group,0.001);
	ilm_commitChanges();
	ilm_surfaceGetOpacity(surface1,&opacity);
	ASSERT_DOUBLE_EQ(0.001, opacity);
	ilm_surfaceGetOpacity(surface2,&opacity);
	ASSERT_DOUBLE_EQ(0.001, opacity);
}

TEST_F(IlmCommandTest, SetGetSurfaceVisibility) {
	uint surface1 = 36;
	t_ilm_bool visibility;

	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface1);

	ilm_surfaceSetVisibility(surface1,ILM_TRUE);
	ilm_commitChanges();
	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_EQ(ILM_TRUE, visibility);

	ilm_surfaceSetVisibility(surface1,ILM_FALSE);
	ilm_commitChanges();
	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_EQ(ILM_FALSE, visibility);

	ilm_surfaceSetVisibility(surface1,ILM_TRUE);
	ilm_commitChanges();
	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_EQ(ILM_TRUE, visibility);
}

TEST_F(IlmCommandTest, SetGetLayerVisibility) {
	uint layer1 = 36;
	t_ilm_bool visibility;

	ilm_layerCreate(&layer1);

	ilm_layerSetVisibility(layer1,ILM_TRUE);
	ilm_commitChanges();
	ilm_layerGetVisibility(layer1,&visibility);
	ASSERT_EQ(ILM_TRUE, visibility);

	ilm_layerSetVisibility(layer1,ILM_FALSE);
	ilm_commitChanges();
	ilm_layerGetVisibility(layer1,&visibility);
	ASSERT_EQ(ILM_FALSE, visibility);

	ilm_layerSetVisibility(layer1,ILM_TRUE);
	ilm_commitChanges();
	ilm_layerGetVisibility(layer1,&visibility);
	ASSERT_EQ(ILM_TRUE, visibility);
}

TEST_F(IlmCommandTest, SetGetLayerGroupVisibility) {
	uint layer1 = 36;
	uint layer2 = 44;
	uint group = 99;
	t_ilm_bool visibility;

	ilm_layerCreate(&layer1);
	ilm_layerCreate(&layer2);
	printf("layers: %i %i", layer1, layer2);
	ilm_layergroupCreate(&group);
	ilm_layergroupAddLayer(group,layer1);
	ilm_layergroupAddLayer(group,layer2);
	ilm_commitChanges();

	ilm_layergroupSetVisibility(group,ILM_TRUE);
	ilm_commitChanges();
	ilm_layerGetVisibility(layer1,&visibility);
	ASSERT_EQ(ILM_TRUE, visibility);
	ilm_layerGetVisibility(layer2,&visibility);
	ASSERT_EQ(ILM_TRUE, visibility);

	ilm_layergroupSetVisibility(group,ILM_FALSE);
	ilm_commitChanges();
	ilm_layerGetVisibility(layer1,&visibility);
	ASSERT_EQ(ILM_FALSE, visibility);
	ilm_layerGetVisibility(layer2,&visibility);
	ASSERT_EQ(ILM_FALSE, visibility);

	ilm_layergroupSetVisibility(group,ILM_TRUE);
	ilm_commitChanges();
	ilm_layerGetVisibility(layer1,&visibility);
	ASSERT_EQ(ILM_TRUE, visibility);
	ilm_layerGetVisibility(layer2,&visibility);
	ASSERT_EQ(ILM_TRUE, visibility);
}

TEST_F(IlmCommandTest, SetGetSurfaceGroupVisibility) {
	uint surface1 = 36;
	uint surface2 = 44;
	uint group = 99;
	t_ilm_bool visibility;

	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface1);
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface2);
	ilm_surfacegroupCreate(&group);
	ilm_surfacegroupAddSurface(group,surface1);
	ilm_surfacegroupAddSurface(group,surface2);

	ilm_surfacegroupSetVisibility(group,ILM_TRUE);
	ilm_commitChanges();
	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_EQ(ILM_TRUE, visibility);
	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_EQ(ILM_TRUE, visibility);

	ilm_surfacegroupSetVisibility(group,ILM_FALSE);
	ilm_commitChanges();
	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_EQ(ILM_FALSE, visibility);
	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_EQ(ILM_FALSE, visibility);

	ilm_surfacegroupSetVisibility(group,ILM_TRUE);
	ilm_commitChanges();
	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_EQ(ILM_TRUE, visibility);
	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_EQ(ILM_TRUE, visibility);

}
TEST_F(IlmCommandTest, ilm_getScreenIDs) {
	t_ilm_uint numberOfScreens;
	t_ilm_uint* screenIDs;
	ilm_getScreenIDs(&numberOfScreens,&screenIDs);
	ASSERT_GT(numberOfScreens,0);
}

TEST_F(IlmCommandTest, ilm_getScreenResolution) {
	t_ilm_uint numberOfScreens;
	t_ilm_uint* screenIDs;
	ilm_getScreenIDs(&numberOfScreens,&screenIDs);
	ASSERT_TRUE(numberOfScreens>0);

	uint firstScreen = screenIDs[0];
	t_ilm_uint width,height;
	ilm_getScreenResolution(firstScreen, &width, &height);
	ASSERT_GT(width,0);
	ASSERT_GT(height,0);
}

TEST_F(IlmCommandTest, ilm_getLayerIDs) {
	uint layer1 = 3246;
	uint layer2 = 46586;

	ilm_layerCreate(&layer1);
	ilm_layerCreate(&layer2);
	ilm_commitChanges();

	t_ilm_int length;
	t_ilm_uint* IDs;
	ilm_getLayerIDs(&length,&IDs);

	ASSERT_EQ(layer1,IDs[0]);
	ASSERT_EQ(layer2,IDs[1]);
}

TEST_F(IlmCommandTest, ilm_getLayerIDsOfScreen) {
	uint layer1 = 3246;
	uint layer2 = 46586;

	ilm_layerCreate(&layer1);
	ilm_layerCreate(&layer2);
	ilm_commitChanges();

	t_ilm_int length;
	t_ilm_uint* IDs;
	ilm_getLayerIDsOnScreen(0,&length,&IDs);

	ASSERT_EQ(layer1,IDs[0]);
	ASSERT_EQ(layer2,IDs[1]);
}

TEST_F(IlmCommandTest, ilm_getSurfaceIDs) {
	uint surface1 = 3246;
	uint surface2 = 46586;

	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface1);
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface2);
	ilm_commitChanges();

	t_ilm_int length;
	t_ilm_uint* IDs;
	ilm_getSurfaceIDs(&length,&IDs);

	ASSERT_EQ(surface1,IDs[0]);
	ASSERT_EQ(surface2,IDs[1]);
}

TEST_F(IlmCommandTest, ilm_getSurfaceGroupIDs) {
	uint surfacegroup1 = 3246;
	uint surfacegroup2 = 46586;

	ilm_surfacegroupCreate(&surfacegroup1);
	ilm_surfacegroupCreate(&surfacegroup2);
	ilm_commitChanges();

	t_ilm_int length;
	t_ilm_uint* IDs;
	ilm_getSurfaceGroupIDs(&length,&IDs);

	ASSERT_EQ(surfacegroup1,IDs[0]);
	ASSERT_EQ(surfacegroup2,IDs[1]);
}

TEST_F(IlmCommandTest, ilm_getLayerGroupIDs) {
	uint layergroup1 = 3246;
	uint layergroup2 = 46586;

	ilm_layergroupCreate(&layergroup1);
	ilm_layergroupCreate(&layergroup2);
	ilm_commitChanges();

	t_ilm_int length;
	t_ilm_uint* IDs;
	ilm_getLayerGroupIDs(&length,&IDs);

	ASSERT_EQ(layergroup1,IDs[0]);
	ASSERT_EQ(layergroup2,IDs[1]);
}

TEST_F(IlmCommandTest, ilm_surfaceCreate_Remove) {
	uint surface1 = 3246;
	uint surface2 = 46586;
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface1);
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface2);
	ilm_commitChanges();

	t_ilm_int length;
	t_ilm_uint* IDs;
	ilm_getSurfaceIDs(&length,&IDs);

	ASSERT_EQ(length,2);
	ASSERT_EQ(surface1,IDs[0]);
	ASSERT_EQ(surface2,IDs[1]);

	ilm_surfaceRemove(surface1);
	ilm_commitChanges();
	ilm_getSurfaceIDs(&length,&IDs);
	ASSERT_EQ(length,1);
	ASSERT_EQ(surface2,IDs[0]);

	ilm_surfaceRemove(surface2);
	ilm_commitChanges();
	ilm_getSurfaceIDs(&length,&IDs);
	ASSERT_EQ(length,0);
}

TEST_F(IlmCommandTest, ilm_layerCreate_Remove) {
	uint layer1 = 3246;
	uint layer2 = 46586;
	ilm_layerCreate(&layer1);
	ilm_layerCreate(&layer2);
	ilm_commitChanges();

	t_ilm_int length;
	t_ilm_uint* IDs;
	ilm_getLayerIDs(&length,&IDs);

	ASSERT_EQ(length,2);
	ASSERT_EQ(layer1,IDs[0]);
	ASSERT_EQ(layer2,IDs[1]);

	ilm_layerRemove(layer1);
	ilm_commitChanges();
	ilm_getLayerIDs(&length,&IDs);
	ASSERT_EQ(length,1);
	ASSERT_EQ(layer2,IDs[0]);

	ilm_layerRemove(layer2);
	ilm_commitChanges();
	ilm_getLayerIDs(&length,&IDs);
	ASSERT_EQ(length,0);
}

TEST_F(IlmCommandTest, ilm_layerAddSurface_ilm_layerRemoveSurface_ilm_getSurfaceIDsOnLayer) {
	uint layer = 3246;
	ilm_layerCreate(&layer);
	uint surface1 = 3246;
	uint surface2 = 46586;
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface1);
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface2);
	ilm_commitChanges();

	t_ilm_int length;
	t_ilm_uint* IDs;
	ilm_getSurfaceIDsOnLayer(layer,&length,&IDs);
	ASSERT_EQ(length,0);

	ilm_layerAddSurface(layer,surface1);
	ilm_commitChanges();
	ilm_getSurfaceIDsOnLayer(layer,&length,&IDs);
	ASSERT_EQ(length,1);
	ASSERT_EQ(surface1,IDs[0]);

	ilm_layerAddSurface(layer,surface2);
	ilm_commitChanges();
	ilm_getSurfaceIDsOnLayer(layer,&length,&IDs);
	ASSERT_EQ(length,2);
	ASSERT_EQ(surface1,IDs[0]);
	ASSERT_EQ(surface2,IDs[1]);

	ilm_layerRemoveSurface(layer,surface1);
	ilm_commitChanges();
	ilm_getSurfaceIDsOnLayer(layer,&length,&IDs);
	ASSERT_EQ(length,1);
	ASSERT_EQ(surface2,IDs[0]);

	ilm_layerRemoveSurface(layer,surface2);
	ilm_commitChanges();
	ilm_getSurfaceIDsOnLayer(layer,&length,&IDs);
	ASSERT_EQ(length,0);
}

TEST_F(IlmCommandTest, ilm_getPropertiesOfSurface_ilm_surfaceSetSourceRectangle_ilm_surfaceSetDestinationRectangle) {
	t_ilm_uint surface;
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface);
	ilm_commitChanges();

	t_ilm_float opacity;
	t_ilm_uint sourceX,sourceY, sourceWidth,sourceHeight, destX,destY,destWidth,destHeight;
	ilmOrientation orientation;
	t_ilm_bool visibility;

	ilm_surfaceSetOpacity(surface,0.8765);
	ilm_surfaceSetSourceRectangle(surface,89,6538,638,4);
	ilm_surfaceSetDestinationRectangle(surface,54,47,947,9);
	ilm_surfaceSetOrientation(surface,ILM_NINETY);
	ilm_surfaceSetVisibility(surface,true);
	ilm_commitChanges();

	ilmSurfaceProperties surfaceProperties;
	ilm_getPropertiesOfSurface(surface, &surfaceProperties);
	ASSERT_EQ(0.8765, surfaceProperties.opacity);
	ASSERT_EQ(89, surfaceProperties.sourceX);
	ASSERT_EQ(6538, surfaceProperties.sourceY);
	ASSERT_EQ(638, surfaceProperties.sourceWidth);
	ASSERT_EQ(4, surfaceProperties.sourceHeight);
	ASSERT_EQ(54, surfaceProperties.destX);
	ASSERT_EQ(47, surfaceProperties.destY);
	ASSERT_EQ(947, surfaceProperties.destWidth);
	ASSERT_EQ(9, surfaceProperties.destHeight);
	ASSERT_EQ(ILM_NINETY, surfaceProperties.orientation);
	ASSERT_TRUE( surfaceProperties.visibility);

	ilm_surfaceSetOpacity(surface,0.436);
	ilm_surfaceSetSourceRectangle(surface,784,546,235,78);
	ilm_surfaceSetDestinationRectangle(surface,536,5372,3,4316);
	ilm_surfaceSetOrientation(surface,ILM_TWOHUNDREDSEVENTY);
	ilm_surfaceSetVisibility(surface,false);
	ilm_commitChanges();

	ilmSurfaceProperties surfaceProperties2;
	ilm_getPropertiesOfSurface(surface, &surfaceProperties2);
	ASSERT_EQ(0.436, surfaceProperties2.opacity);
	ASSERT_EQ(784, surfaceProperties2.sourceX);
	ASSERT_EQ(546, surfaceProperties2.sourceY);
	ASSERT_EQ(235, surfaceProperties2.sourceWidth);
	ASSERT_EQ(78, surfaceProperties2.sourceHeight);
	ASSERT_EQ(536, surfaceProperties2.destX);
	ASSERT_EQ(5372, surfaceProperties2.destY);
	ASSERT_EQ(3, surfaceProperties2.destWidth);
	ASSERT_EQ(4316, surfaceProperties2.destHeight);
	ASSERT_EQ(ILM_TWOHUNDREDSEVENTY, surfaceProperties2.orientation);
	ASSERT_FALSE(surfaceProperties2.visibility);
}

TEST_F(IlmCommandTest, ilm_getPropertiesOfLayer_ilm_layerSetSourceRectangle_ilm_layerSetDestinationRectangle) {
	t_ilm_uint layer;
	ilm_layerCreate(&layer);
	ilm_commitChanges();

	t_ilm_float opacity;
	t_ilm_uint sourceX,sourceY, sourceWidth,sourceHeight, destX,destY,destWidth,destHeight;
	ilmOrientation orientation;
	t_ilm_bool visibility;

	ilm_layerSetOpacity(layer,0.8765);
	ilm_layerSetSourceRectangle(layer,89,6538,638,4);
	ilm_layerSetDestinationRectangle(layer,54,47,947,9);
	ilm_layerSetOrientation(layer,ILM_NINETY);
	ilm_layerSetVisibility(layer,true);
	ilm_commitChanges();

	ilmLayerProperties layerProperties1;
	ilm_getPropertiesOfLayer(layer, &layerProperties1);
	ASSERT_EQ(0.8765, layerProperties1.opacity);
	ASSERT_EQ(89, layerProperties1.sourceX);
	ASSERT_EQ(6538, layerProperties1.sourceY);
	ASSERT_EQ(638, layerProperties1.sourceWidth);
	ASSERT_EQ(4, layerProperties1.sourceHeight);
	ASSERT_EQ(54, layerProperties1.destX);
	ASSERT_EQ(47, layerProperties1.destY);
	ASSERT_EQ(947, layerProperties1.destWidth);
	ASSERT_EQ(9, layerProperties1.destHeight);
	ASSERT_EQ(ILM_NINETY, layerProperties1.orientation);
	ASSERT_TRUE( layerProperties1.visibility);

	ilm_layerSetOpacity(layer,0.436);
	ilm_layerSetSourceRectangle(layer,784,546,235,78);
	ilm_layerSetDestinationRectangle(layer,536,5372,3,4316);
	ilm_layerSetOrientation(layer,ILM_TWOHUNDREDSEVENTY);
	ilm_layerSetVisibility(layer,false);
	ilm_commitChanges();

	ilmLayerProperties layerProperties2;
	ilm_getPropertiesOfLayer(layer, &layerProperties2);
	ASSERT_EQ(0.436, layerProperties2.opacity);
	ASSERT_EQ(784, layerProperties2.sourceX);
	ASSERT_EQ(546, layerProperties2.sourceY);
	ASSERT_EQ(235, layerProperties2.sourceWidth);
	ASSERT_EQ(78, layerProperties2.sourceHeight);
	ASSERT_EQ(536, layerProperties2.destX);
	ASSERT_EQ(5372, layerProperties2.destY);
	ASSERT_EQ(3, layerProperties2.destWidth);
	ASSERT_EQ(4316, layerProperties2.destHeight);
	ASSERT_EQ(ILM_TWOHUNDREDSEVENTY, layerProperties2.orientation);
	ASSERT_FALSE(layerProperties2.visibility);
}

TEST_F(IlmCommandTest, ilm_takeScreenshot) {
	// make sure the file is not there before
	FILE* f = fopen("/tmp/test.bmp","r");
	if (f!=NULL){
		fclose(f);
		int result = remove("/tmp/test.bmp");
		ASSERT_EQ(0,result);
	}

	ilm_takeScreenshot(0, "/tmp/test.bmp");

	usleep(50000);
	f = fopen("/tmp/test.bmp","r");
	ASSERT_TRUE(f!=NULL);
	fclose(f);
}

TEST_F(IlmCommandTest, ilm_surfaceGetPixelformat) {
	t_ilm_uint surface1=0;
	t_ilm_uint surface2=0;
	t_ilm_uint surface3=0;
	t_ilm_uint surface4=0;
	t_ilm_uint surface5=0;
	t_ilm_uint surface6=0;
	t_ilm_uint surface7=0;

	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_4444,&surface1);
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_5551,&surface2);
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_6661,&surface3);
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface4);
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGB_565,&surface5);
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGB_888,&surface6);
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_R_8,&surface7);
	ilm_commitChanges();

	ilmPixelFormat p1,p2,p3,p4,p5,p6,p7;

	ilm_surfaceGetPixelformat(surface1,&p1);
	ilm_surfaceGetPixelformat(surface2,&p2);
	ilm_surfaceGetPixelformat(surface3,&p3);
	ilm_surfaceGetPixelformat(surface4,&p4);
	ilm_surfaceGetPixelformat(surface5,&p5);
	ilm_surfaceGetPixelformat(surface6,&p6);
	ilm_surfaceGetPixelformat(surface7,&p7);

	ASSERT_EQ(ILM_PIXELFORMAT_RGBA_4444,p1);
	ASSERT_EQ(ILM_PIXELFORMAT_RGBA_5551,p2);
	ASSERT_EQ(ILM_PIXELFORMAT_RGBA_6661,p3);
	ASSERT_EQ(ILM_PIXELFORMAT_RGBA_8888,p4);
	ASSERT_EQ(ILM_PIXELFORMAT_RGB_565,p5);
	ASSERT_EQ(ILM_PIXELFORMAT_RGB_888,p6);
	ASSERT_EQ(ILM_PIXELFORMAT_R_8,p7);
}

TEST_F(IlmCommandTest, ilm_surfacegroupCreate_ilm_surfacegroupRemove_ilm_surfacegroupAddSurface_ilm_surfacegroupRemoveSurface) {
	uint surface1 = 36;
	uint surface2 = 44;
	uint group = 99;
	t_ilm_bool visibility;

	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface1);
	ilm_surfaceCreate(0,0,0,ILM_PIXELFORMAT_RGBA_8888,&surface2);
	ilm_surfacegroupCreate(&group);
	ilm_surfaceSetVisibility(surface1,true);
	ilm_surfaceSetVisibility(surface2,true);
	ilm_commitChanges();

	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_TRUE(visibility);
	ilm_surfaceGetVisibility(surface2,&visibility);
	ASSERT_TRUE(visibility);

	ilm_surfacegroupSetVisibility(group,false);
	ilm_commitChanges();
	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_TRUE(visibility);
	ilm_surfaceGetVisibility(surface2,&visibility);
	ASSERT_TRUE(visibility);

	ilm_surfacegroupAddSurface(group,surface1);
	ilm_surfacegroupSetVisibility(group,false);
	ilm_commitChanges();
	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_FALSE(visibility);
	ilm_surfaceGetVisibility(surface2,&visibility);
	ASSERT_TRUE(visibility);

	ilm_surfacegroupAddSurface(group,surface2);
	ilm_surfacegroupSetVisibility(group,false);
	ilm_commitChanges();
	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_FALSE(visibility);
	ilm_surfaceGetVisibility(surface2,&visibility);
	ASSERT_FALSE(visibility);

	ilm_surfacegroupRemoveSurface(group,surface2);
	ilm_surfacegroupSetVisibility(group,true);
	ilm_commitChanges();
	ilm_surfaceGetVisibility(surface1,&visibility);
	ASSERT_TRUE(visibility);
	ilm_surfaceGetVisibility(surface2,&visibility);
	ASSERT_FALSE(visibility);

	t_ilm_int length;
	t_ilm_uint* IDs;
	ilm_getSurfaceGroupIDs(&length,&IDs);
	ASSERT_EQ(length,1);
	ASSERT_EQ(group,IDs[0]);

	ilm_surfacegroupRemove(group);
	ilm_commitChanges();
	ilm_getSurfaceGroupIDs(&length,&IDs);
	ASSERT_EQ(length,0);

}

TEST_F(IlmCommandTest, ilm_layergroupCreate_ilm_layergroupRemove_ilm_layergroupAddlayer_ilm_layergroupRemovelayer) {
	uint layer1 = 36;
	uint layer2 = 44;
	uint group = 99;
	t_ilm_bool visibility;

	ilm_layerCreate(&layer1);
	ilm_layerCreate(&layer2);
	ilm_layergroupCreate(&group);
	ilm_layerSetVisibility(layer1,true);
	ilm_layerSetVisibility(layer2,true);
	ilm_commitChanges();

	ilm_layerGetVisibility(layer1,&visibility);
	ASSERT_TRUE(visibility);
	ilm_layerGetVisibility(layer2,&visibility);
	ASSERT_TRUE(visibility);

	ilm_layergroupSetVisibility(group,false);
	ilm_commitChanges();
	ilm_layerGetVisibility(layer1,&visibility);
	ASSERT_TRUE(visibility);
	ilm_layerGetVisibility(layer2,&visibility);
	ASSERT_TRUE(visibility);

	ilm_layergroupAddLayer(group,layer1);
	ilm_layergroupSetVisibility(group,false);
	ilm_commitChanges();
	ilm_layerGetVisibility(layer1,&visibility);
	ASSERT_FALSE(visibility);
	ilm_layerGetVisibility(layer2,&visibility);
	ASSERT_TRUE(visibility);

	ilm_layergroupAddLayer(group,layer2);
	ilm_layergroupSetVisibility(group,false);
	ilm_commitChanges();
	ilm_layerGetVisibility(layer1,&visibility);
	ASSERT_FALSE(visibility);
	ilm_layerGetVisibility(layer2,&visibility);
	ASSERT_FALSE(visibility);

	ilm_layergroupRemoveLayer(group,layer2);
	ilm_layergroupSetVisibility(group,true);
	ilm_commitChanges();
	ilm_layerGetVisibility(layer1,&visibility);
	ASSERT_TRUE(visibility);
	ilm_layerGetVisibility(layer2,&visibility);
	ASSERT_FALSE(visibility);

	t_ilm_int length;
	t_ilm_uint* IDs;
	ilm_getLayerGroupIDs(&length,&IDs);
	ASSERT_EQ(length,1);
	ASSERT_EQ(group,IDs[0]);

	ilm_layergroupRemove(group);
	ilm_commitChanges();
	ilm_getLayerGroupIDs(&length,&IDs);
	ASSERT_EQ(length,0);

}
