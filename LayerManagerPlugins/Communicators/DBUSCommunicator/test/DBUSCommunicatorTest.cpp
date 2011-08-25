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

#ifndef DBUSDBUSCommunicatorTest_H_
#define DBUSDBUSCommunicatorTest_H_

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ICommunicator.h"
#include "DBUSCommunicator.h"
#include "mock_CommandExecutor.h"
#include "mock_Layerlist.h"
//#include "FieldMatcher.h"
#include <vector>

using ::testing::Field;
using ::testing::Property;
using ::testing::Matcher;
using ::testing::Pointee;
using ::testing::AllOf;
using ::testing::SafeMatcherCast;
using ::testing::MatcherCast;
using ::testing::DefaultValue;
using ::testing::Eq;
using ::testing::An;
using ::testing::ElementsAreArray;
using ::testing::ElementsAre;
using ::testing::NotNull;

// The names of the dbus service for compositing
#define DBUS_COMPOSITE_SERVICE_PATH  "/de/bmw/CompositingService"
#define DBUS_COMPOSITE_SERVICE       "de.bmw.CompositingService"

const std::string DBUSCOMMAND = "dbus-send --type=method_call --print-reply --dest=de.bmw.CompositingService /de/bmw/CompositingService de.bmw.CompositingService.";

class DBUSCommunicatorTest : public ::testing::Test {
public:

    DBUSCommunicatorTest()
    {
        communicatorUnderTest = new DBUSCommunicator(&mockCommandExecutor);
    }

    virtual ~DBUSCommunicatorTest()
    {
        if (communicatorUnderTest)
        {
            delete communicatorUnderTest;
        }
    }

    void SetUp()
    {
        this->communicatorUnderTest->start();
    }

    void TearDown()
    {
        // remove all pending commands
        //          Layermanager* lm = Layermanager::m_instance;
        //          lm->execute(new CommitCommand());
        //
        this->communicatorUnderTest->stop();
        //          removeAll();
    }
    void removeAll()
    {
//          // remove all layers after each test
//                Layermanager* lm = Layermanager::m_instance;
//                int length;
//                int * array;
//                lm->layerlist.getLayerIDs(&length, &array);
//                for (int i=0;i<length;i++){
//                    Layer *l = lm->layerlist.getLayer(array[i]);
//                    // remove all surface from all layers..
//                    // l->surfaces.clear()
//                    lm->layerlist.removeLayer(l);
//                }
//
//              const std::map<int,Surface*> surfacelist = lm->layerlist.getAllSurfaces();
//              for (std::map<int,Surface*>::const_iterator it = surfacelist.begin();it!=surfacelist.end();it++){
//                  Surface *s = lm->layerlist.getSurface((*it).first);
//                  // remove all surface from all layers..
//                  // l->surfaces.clear()
//                  lm->layerlist.removeSurface(s);
//              }
//
//            lm->layerlist.getSurfaceGroupIDs(&length, &array);
//            for (int i=0;i<length;i++){
//              SurfaceGroup *sg = lm->layerlist.getSurfaceGroup(array[i]);
//              // remove all surface from all layers..
//              // l->surfaces.clear()
//              lm->layerlist.removeSurfaceGroup(sg);
//            }
//
//            lm->layerlist.getLayerGroupIDs(&length, &array);
//            for (int i=0;i<length;i++){
//                LayerGroup *lg = lm->layerlist.getLayerGroup(array[i]);
//              // remove all surface from all layers..
//              // l->surfaces.clear()
//              lm->layerlist.removeLayerGroup(lg);
//            }
//
//            lm->layerlist.getCurrentRenderOrder().clear();

    }

    ICommunicator* communicatorUnderTest;
    MockCommandExecutor mockCommandExecutor;
    MockLayerList layerlist;
};

MATCHER_P(DebugCommandEq, onoff, "DebugCommand has onoff set to %(onoff)s")
{
    return ((DebugCommand*)arg)->m_onoff == onoff;
}

TEST_F(DBUSCommunicatorTest, TurnDebugOnAndOff)
{
    EXPECT_CALL(this->mockCommandExecutor, execute(DebugCommandEq(false) ) ).Times(1);
  system((DBUSCOMMAND + std::string("Debug boolean:false")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute(DebugCommandEq(true) ) ).Times(1);
  system((DBUSCOMMAND + std::string("Debug boolean:true")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute(DebugCommandEq(false) ) ).Times(1);
  system((DBUSCOMMAND + std::string("Debug boolean:false")).c_str());
}

TEST_F(DBUSCommunicatorTest, StartStop) {
}

TEST_F(DBUSCommunicatorTest, ListAllLayerIDS) {
  DefaultValue<Scene*>::Set((Scene*)&this->layerlist);
  EXPECT_CALL(this->layerlist, getLayerIDs(NotNull(),NotNull() )).Times(1) ;
  system((DBUSCOMMAND + std::string("ListAllLayerIDS")).c_str());
}

TEST_F(DBUSCommunicatorTest, listAlllayerGroupIDS) {
    std::list<int> defaultlist;
    // Sets the default return value for type Bar.
    DefaultValue<Scene*>::Set((Scene*)&this->layerlist);
    DefaultValue< std::list<int> >::Set(defaultlist);
    EXPECT_CALL(this->layerlist, getLayerGroupIDs(NotNull(),NotNull()) ).Times(1) ;
    system((DBUSCOMMAND + std::string("ListAllLayerGroupIDS")).c_str());
}

TEST_F(DBUSCommunicatorTest, listAllSurfaceGroupIDS) {
    std::list<int> defaultlist;
    // Sets the default return value for type Bar.
    DefaultValue<Scene*>::Set((Scene*)&this->layerlist);
    DefaultValue< std::list<int> >::Set(defaultlist);
    EXPECT_CALL(this->layerlist, getSurfaceGroupIDs(NotNull(),NotNull() )).Times(1) ;
    system((DBUSCOMMAND + std::string("ListAllSurfaceGroupIDS")).c_str());
}

TEST_F(DBUSCommunicatorTest, listSurfacesOfSurfacegroup) {

    std::list<int> defaultlist;
    DefaultValue<Scene*>::Set((Scene*)&this->layerlist);    
    // Sets the default return value for type Bar.
    DefaultValue< std::list<int> >::Set(defaultlist);
    DefaultValue< SurfaceGroup* >::Set(new SurfaceGroup());
    EXPECT_CALL(this->layerlist, getSurfaceGroup(Eq(84567u) )).Times(1) ;
    system((DBUSCOMMAND + std::string("ListSurfacesOfSurfacegroup uint32:84567")).c_str());

}

TEST_F(DBUSCommunicatorTest, listlayersOflayergroup) {

    std::list<int> defaultlist;
    // Sets the default return value for type Bar.
    DefaultValue<Scene*>::Set((Scene*)&this->layerlist);    
    DefaultValue< std::list<int> >::Set(defaultlist);
    DefaultValue< LayerGroup* >::Set(new LayerGroup());

    EXPECT_CALL(this->layerlist, getLayerGroup(Eq(345u) )).Times(1) ;
    system((DBUSCOMMAND + std::string("ListLayersOfLayergroup uint32:345")).c_str());

}

TEST_F(DBUSCommunicatorTest, listSurfaceoflayer) {

    Scene scene;
    DefaultValue< Layer* >::Set(scene.createLayer(234));
    DefaultValue<Scene*>::Set((Scene*)&layerlist);    
    EXPECT_CALL(this->layerlist, getLayer(Eq(234u) )).Times(1) ;
    system((DBUSCOMMAND + std::string("ListSurfaceofLayer uint32:234")).c_str());
}

TEST_F(DBUSCommunicatorTest, getPropertiesOfSurface) {

    Scene scene;
    unsigned int newID=0;
    DefaultValue<Scene*>::Set((Scene*)&layerlist);            
    DefaultValue< Surface* >::Set(scene.createSurface(newID));
    EXPECT_CALL(this->layerlist, getSurface(Eq(876u) )).Times(1) ;
    system((DBUSCOMMAND + std::string("GetPropertiesOfSurface uint32:876")).c_str());
}

TEST_F(DBUSCommunicatorTest, getPropertiesOflayer) {

    Scene scene;
    DefaultValue< Layer* >::Set(scene.createLayer(0));
    DefaultValue<Scene*>::Set((Scene*)&layerlist);    
    EXPECT_CALL(this->layerlist, getLayer(Eq(876u) )).Times(1) ;
    system((DBUSCOMMAND + std::string("GetPropertiesOfLayer uint32:876")).c_str());
}

MATCHER_P5(CreateCommandEq, nativeHandle,OriginalWidth,OriginalHeight,createType,pixelformat, "%(*)s") {
    return ((CreateCommand*)arg)->m_nativeHandle == nativeHandle
            && ((CreateCommand*)arg)->m_originalHeight == OriginalHeight
            && ((CreateCommand*)arg)->m_originalWidth == OriginalWidth
            && ((CreateCommand*)arg)->m_createType == createType
            && ((CreateCommand*)arg)->m_pixelformat == pixelformat;

  }
TEST_F(DBUSCommunicatorTest, CreateSurface) {

    EXPECT_CALL(this->mockCommandExecutor, execute( CreateCommandEq(44u,33u,22u,TypeSurface,PIXELFORMAT_RGBA8888))).Times(1);
    system((DBUSCOMMAND + std::string("CreateSurface uint32:44 uint32:33 uint32:22 uint32:2 ")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( CreateCommandEq(404u,303u,0u,TypeSurface,PIXELFORMAT_RGB888))).Times(1);
        system((DBUSCOMMAND + std::string("CreateSurface uint32:404 uint32:303 uint32:0 uint32:1 ")).c_str());

}

TEST_F(DBUSCommunicatorTest, Createlayer) {

    uint resolution[2] = {0,0};
    DefaultValue<uint*>::Set(resolution);

    EXPECT_CALL(this->mockCommandExecutor, execute( CreateCommandEq(0u,0u,0u,TypeLayer,PIXELFORMAT_R8))).Times(1);
    system((DBUSCOMMAND + std::string("CreateLayer")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( CreateCommandEq(0u,0u,0u,TypeLayer,PIXELFORMAT_R8))).Times(1);
    system((DBUSCOMMAND + std::string("CreateLayer")).c_str());


}

MATCHER_P2(RemoveCommandEq, idToRemove,typeToRemove, "%(*)s") {
    return ((RemoveCommand*)arg)->m_idToRemove == idToRemove
            && ((RemoveCommand*)arg)->m_typeToRemove == typeToRemove;

  }
TEST_F(DBUSCommunicatorTest, RemoveSurface) {

 EXPECT_CALL(this->mockCommandExecutor, execute( RemoveCommandEq(8u,TypeSurface))).Times(1);
  system((DBUSCOMMAND + std::string("RemoveSurface uint32:8")).c_str());

  EXPECT_CALL(this->mockCommandExecutor, execute( RemoveCommandEq(5u,TypeSurface))).Times(1);
  system((DBUSCOMMAND + std::string("RemoveSurface uint32:5")).c_str());


}

TEST_F(DBUSCommunicatorTest, Removelayer) {
     EXPECT_CALL(this->mockCommandExecutor, execute( RemoveCommandEq(8u,TypeLayer))).Times(1);
  system((DBUSCOMMAND + std::string("RemoveLayer uint32:8")).c_str());

  EXPECT_CALL(this->mockCommandExecutor, execute( RemoveCommandEq(5u,TypeLayer))).Times(1);
  system((DBUSCOMMAND + std::string("RemoveLayer uint32:5")).c_str());


}

MATCHER_P3(SetOpacityCommandEq, id,type,Opacity, "%(*)s") {
    return ((SetOpacityCommand*)arg)->m_id == id
            && ((SetOpacityCommand*)arg)->m_type == type
            && ((SetOpacityCommand*)arg)->m_opacity == Opacity;

  }

TEST_F(DBUSCommunicatorTest, SetSurfaceOpacity) {
 EXPECT_CALL(this->mockCommandExecutor, execute(SetOpacityCommandEq(36u,TypeSurface,0.88) )).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfaceOpacity uint32:36 double:0.88")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute(SetOpacityCommandEq(44u,TypeSurface,0.001) )).Times(1);
  system((DBUSCOMMAND + std::string("SetSurfaceOpacity uint32:44 double:0.001")).c_str());

}

//TEST_F(DBUSCommunicatorTest, Exit) {
// EXPECT_CALL(this->mockCommandExecutor, execute(Field(&Command::commandType,Eq(Command::Exit)) ) ).Times(1);
//  system((DBUSCOMMAND + std::string("Exit")).c_str());
//
//
//  //EXPECT_EXIT(system((DBUSCOMMAND + std::string("Exit")).c_str());, ::testing::ExitedWithCode(0), "");
//
//}
/*
TEST_F(DBUSCommunicatorTest, Commit) {
 EXPECT_CALL(this->mockCommandExecutor, execute(Field(&ICommand::commandType,Eq(ICommand::CommitChanges)) ) ).Times(1);
  system((DBUSCOMMAND + std::string("CommitChanges")).c_str());
}
*/

MATCHER_P6(SetSourceRectangleCommandEq, id,typeToSet,x,y,width,height, "%(*)s") {
    return        ((SetSourceRectangleCommand*)arg)->m_id == id
            && ((SetSourceRectangleCommand*)arg)->m_typeToSet == typeToSet
            && ((SetSourceRectangleCommand*)arg)->m_x == x
            && ((SetSourceRectangleCommand*)arg)->m_y == y
            && ((SetSourceRectangleCommand*)arg)->m_width == width
            && ((SetSourceRectangleCommand*)arg)->m_height == height;

  }

TEST_F(DBUSCommunicatorTest, SetSurfaceSourceRegion) {

 EXPECT_CALL(this->mockCommandExecutor, execute( SetSourceRectangleCommandEq(36u,TypeSurface,1u,2u,3u,4u))).Times(1);
  system((DBUSCOMMAND + std::string("SetSurfaceSourceRegion uint32:36 uint32:1 uint32:2 uint32:3 uint32:4")).c_str());

  EXPECT_CALL(this->mockCommandExecutor, execute( SetSourceRectangleCommandEq(44u,TypeSurface,15u,25u,35u,45u))).Times(1);
  system((DBUSCOMMAND + std::string("SetSurfaceSourceRegion uint32:44 uint32:15 uint32:25 uint32:35 uint32:45 ")).c_str());

}

TEST_F(DBUSCommunicatorTest, SetlayerSourceRegion) {

  EXPECT_CALL(this->mockCommandExecutor, execute( SetSourceRectangleCommandEq(36u,TypeLayer,1u,2u,3u,4u))).Times(1);
  system((DBUSCOMMAND + std::string("SetLayerSourceRegion uint32:36 uint32:1 uint32:2 uint32:3 uint32:4")).c_str());

  EXPECT_CALL(this->mockCommandExecutor, execute( SetSourceRectangleCommandEq(44u,TypeLayer,15u,25u,35u,45u))).Times(1);
  system((DBUSCOMMAND + std::string("SetLayerSourceRegion uint32:44 uint32:15 uint32:25 uint32:35 uint32:45 ")).c_str());


}

MATCHER_P6(SetDestinationRectangleCommandEq, id,typeToSet,x,y,width,height, "%(*)s") {
    return        ((SetDestinationRectangleCommand*)arg)->m_id == id
            && ((SetDestinationRectangleCommand*)arg)->m_typeToSet == typeToSet
            && ((SetDestinationRectangleCommand*)arg)->m_x == x
            && ((SetDestinationRectangleCommand*)arg)->m_y == y
            && ((SetDestinationRectangleCommand*)arg)->m_width == width
            && ((SetDestinationRectangleCommand*)arg)->m_height == height;

  }
TEST_F(DBUSCommunicatorTest, SetlayerDestinationRegion) {

    EXPECT_CALL(this->mockCommandExecutor, execute( SetDestinationRectangleCommandEq(36u,TypeLayer,1u,2u,3u,4u))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayerDestinationRegion uint32:36 uint32:1 uint32:2 uint32:3 uint32:4")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetDestinationRectangleCommandEq(44u,TypeLayer,15u,25u,35u,45u))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayerDestinationRegion uint32:44 uint32:15 uint32:25 uint32:35 uint32:45 ")).c_str());

}

TEST_F(DBUSCommunicatorTest, SetSurfaceDestinationRegion) {

    EXPECT_CALL(this->mockCommandExecutor, execute( SetDestinationRectangleCommandEq(36u,TypeSurface,1u,2u,3u,4u))).Times(1);
  system((DBUSCOMMAND + std::string("SetSurfaceDestinationRegion uint32:36 uint32:1 uint32:2 uint32:3 uint32:4")).c_str());

  EXPECT_CALL(this->mockCommandExecutor, execute( SetDestinationRectangleCommandEq(44u,TypeSurface,15u,25u,35u,45u))).Times(1);
  system((DBUSCOMMAND + std::string("SetSurfaceDestinationRegion uint32:44 uint32:15 uint32:25 uint32:35 uint32:45 ")).c_str());


}

MATCHER_P4(SetPositionCommandEq, id,typeToSet,x,y, "%(*)s") {
    return        ((SetPositionCommand*)arg)->m_id == id
            && ((SetPositionCommand*)arg)->m_typeToSet == typeToSet
            && ((SetPositionCommand*)arg)->m_x == x
            && ((SetPositionCommand*)arg)->m_y == y;

  }

TEST_F(DBUSCommunicatorTest, SetlayerPosition) {
    EXPECT_CALL(this->mockCommandExecutor, execute( SetPositionCommandEq(36u,TypeLayer,1u,2u))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayerPosition uint32:36 uint32:1 uint32:2")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetPositionCommandEq(44u,TypeLayer,15u,25u))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayerPosition uint32:44 uint32:15 uint32:25 ")).c_str());
}

TEST_F(DBUSCommunicatorTest, SetSurfacePosition) {
    EXPECT_CALL(this->mockCommandExecutor, execute( SetPositionCommandEq(36u,TypeSurface,3u,4u))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfacePosition uint32:36 uint32:3 uint32:4")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetPositionCommandEq(44u,TypeSurface,35u,45u))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfacePosition uint32:44 uint32:35 uint32:45 ")).c_str());
}

MATCHER_P4(SetDimensionCommandEq, id,typeToSet, width,height, "%(*)s") {
    return        ((SetDimensionCommand*)arg)->m_id == id
            && ((SetDimensionCommand*)arg)->m_typeToSet == typeToSet
            && ((SetDimensionCommand*)arg)->m_width == width
            && ((SetDimensionCommand*)arg)->m_height == height;

  }
TEST_F(DBUSCommunicatorTest, SetlayerDimension) {
 EXPECT_CALL(this->mockCommandExecutor, execute( SetDimensionCommandEq(8554u,TypeLayer,400u,444u))).Times(1);
  system((DBUSCOMMAND + std::string("SetLayerDimension uint32:8554 uint32:400 uint32:444")).c_str());

  EXPECT_CALL(this->mockCommandExecutor, execute( SetDimensionCommandEq(34589u,TypeLayer,400u,444u))).Times(1);
  system((DBUSCOMMAND + std::string("SetLayerDimension uint32:34589 uint32:400 uint32:444")).c_str());
}

TEST_F(DBUSCommunicatorTest, SetSurfaceDimension) {
    EXPECT_CALL(this->mockCommandExecutor, execute( SetDimensionCommandEq(36u,TypeSurface,3u,4u))).Times(1);
  system((DBUSCOMMAND + std::string("SetSurfaceDimension uint32:36 uint32:3 uint32:4")).c_str());

  EXPECT_CALL(this->mockCommandExecutor, execute( SetDimensionCommandEq(44u,TypeSurface,35u,45u))).Times(1);
  system((DBUSCOMMAND + std::string("SetSurfaceDimension uint32:44 uint32:35 uint32:45 ")).c_str());

}

TEST_F(DBUSCommunicatorTest, SetlayerOpacity) {

    EXPECT_CALL(this->mockCommandExecutor, execute( SetOpacityCommandEq(36u,TypeLayer,0.88))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayerOpacity uint32:36 double:0.88")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetOpacityCommandEq(44u,TypeLayer,0.001))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayerOpacity uint32:44 double:0.001")).c_str());
}

MATCHER_P2(GetOpacityCommandEq, id,type,  "%(*)s") {
    return        ((GetOpacityCommand*)arg)->m_id == id
            && ((GetOpacityCommand*)arg)->m_type == type;

  }
TEST_F(DBUSCommunicatorTest, GetlayerOpacity) {

    EXPECT_CALL(this->mockCommandExecutor, execute( GetOpacityCommandEq(36u,TypeLayer))).Times(1);
    system((DBUSCOMMAND + std::string("GetLayerOpacity uint32:36")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( GetOpacityCommandEq(44u,TypeLayer))).Times(1);
    system((DBUSCOMMAND + std::string("GetLayerOpacity uint32:44 ")).c_str());
}

TEST_F(DBUSCommunicatorTest, SetSurfacegroupOpacity) {

    EXPECT_CALL(this->mockCommandExecutor, execute( SetOpacityCommandEq(36u,TypeSurfaceGroup,0.88))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfacegroupOpacity uint32:36 double:0.88")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetOpacityCommandEq(44u,TypeSurfaceGroup,0.001))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfacegroupOpacity uint32:44 double:0.001")).c_str());
}

TEST_F(DBUSCommunicatorTest, SetlayergroupOpacity) {

    EXPECT_CALL(this->mockCommandExecutor, execute( SetOpacityCommandEq(36u,TypeLayerGroup,0.88))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayergroupOpacity uint32:36 double:0.88")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetOpacityCommandEq(44u,TypeLayerGroup,0.001))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayergroupOpacity uint32:44 double:0.001")).c_str());
}

MATCHER_P3(SetOrientationCommandEq, id,type,Orientation,  "%(*)s") {
    return        ((SetOrientationCommand*)arg)->m_id == id
            && ((SetOrientationCommand*)arg)->m_type == type
            && ((SetOrientationCommand*)arg)->m_orientation == Orientation;

  }

TEST_F(DBUSCommunicatorTest, SetSurfaceOrientation) {

    EXPECT_CALL(this->mockCommandExecutor, execute( SetOrientationCommandEq(36u,TypeSurface,0))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfaceOrientation uint32:36 uint32:0")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetOrientationCommandEq(44u,TypeSurface,1))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfaceOrientation uint32:44 uint32:1")).c_str());
}

TEST_F(DBUSCommunicatorTest, SetlayerOrientation) {

    EXPECT_CALL(this->mockCommandExecutor, execute( SetOrientationCommandEq(36u,TypeLayer,0))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayerOrientation uint32:36 uint32:0")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetOrientationCommandEq(44u,TypeLayer,1))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayerOrientation uint32:44 uint32:1")).c_str());
}


MATCHER_P3(SetVisibilityCommandEq, id,otype,visibility, "%(*)s") {
    return ((SetVisibilityCommand*)arg)->m_idtoSet == id
            && ((SetVisibilityCommand*)arg)->m_otype == otype
            && ((SetVisibilityCommand*)arg)->m_visibility == visibility;

  }
TEST_F(DBUSCommunicatorTest, SetSurfaceVisibility) {

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(36u,TypeSurface,false))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfaceVisibility uint32:36 boolean:false")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(44u,TypeSurface,true))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfaceVisibility uint32:44 boolean:true")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(36u,TypeSurface,false))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfaceVisibility uint32:36 boolean:false")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(44u,TypeSurface,true))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfaceVisibility uint32:44 boolean:true")).c_str());

}

TEST_F(DBUSCommunicatorTest, SetlayerVisibility) {

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(36u,TypeLayer,false))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayerVisibility uint32:36 boolean:false")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(44u,TypeLayer,true))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayerVisibility uint32:44 boolean:true")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(36u,TypeLayer,false))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayerVisibility uint32:36 boolean:false")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(44u,TypeLayer,true))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayerVisibility uint32:44 boolean:true")).c_str());
}

TEST_F(DBUSCommunicatorTest, SetSurfacegroupVisibility) {

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(36u,TypeSurfaceGroup,false))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfacegroupVisibility uint32:36 boolean:false")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(44u,TypeSurfaceGroup,true))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfacegroupVisibility uint32:44 boolean:true")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(36u,TypeSurfaceGroup,false))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfacegroupVisibility uint32:36 boolean:false")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(44u,TypeSurfaceGroup,true))).Times(1);
    system((DBUSCOMMAND + std::string("SetSurfacegroupVisibility uint32:44 boolean:true")).c_str());

}

TEST_F(DBUSCommunicatorTest, SetlayergroupVisibility) {

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(36u,TypeLayerGroup,false))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayergroupVisibility uint32:36 boolean:false")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(44u,TypeLayerGroup,true))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayergroupVisibility uint32:44 boolean:true")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(36u,TypeLayerGroup,false))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayergroupVisibility uint32:36 boolean:false")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetVisibilityCommandEq(44u,TypeLayerGroup,true))).Times(1);
    system((DBUSCOMMAND + std::string("SetLayergroupVisibility uint32:44 boolean:true")).c_str());
}

MATCHER_P2(SurfacegroupAddSurfaceCommandEq, surfaceid, surfacegroupid, "%(*)s") {
    return ((SurfacegroupAddSurfaceCommand*)arg)->m_surfacegroupid == surfacegroupid
        && ((SurfacegroupAddSurfaceCommand*)arg)->m_surfaceid == surfaceid;

  }
TEST_F(DBUSCommunicatorTest, AddSurfaceToSurfaceGroup) {

 EXPECT_CALL(this->mockCommandExecutor, execute( SurfacegroupAddSurfaceCommandEq(36u,77u))).Times(1);
    system((DBUSCOMMAND + std::string("AddSurfaceToSurfaceGroup uint32:36 uint32:77")).c_str());
}

MATCHER_P2(SurfacegroupRemoveSurfaceCommandEq, surfacegroupid, surfaceid, "%(*)s") {
    return ((SurfacegroupRemoveSurfaceCommand*)arg)->m_surfacegroupid == surfacegroupid
        && ((SurfacegroupRemoveSurfaceCommand*)arg)->m_surfaceid == surfaceid;
  }
TEST_F(DBUSCommunicatorTest, RemoveSurfaceFromSurfaceGroup) {

    EXPECT_CALL(this->mockCommandExecutor, execute( SurfacegroupAddSurfaceCommandEq(36u,77u))).Times(1);
    system((DBUSCOMMAND + std::string("RemoveSurfaceFromSurfaceGroup uint32:36 uint32:77")).c_str());

}

MATCHER_P2(LayergroupAddLayerCommandEq, layerid,layergroupid, "%(*)s") {
    return ((LayergroupAddLayerCommand*)arg)->m_layergroupid == layergroupid
        && ((LayergroupAddLayerCommand*)arg)->m_layerid == layerid;

  }
TEST_F(DBUSCommunicatorTest, AddlayerTolayerGroup) {

 EXPECT_CALL(this->mockCommandExecutor, execute( LayergroupAddLayerCommandEq(36u,77u))).Times(1);
    system((DBUSCOMMAND + std::string("AddLayerToLayerGroup uint32:36 uint32:77")).c_str());

}

MATCHER_P2(LayergroupRemoveLayerCommandEq, layerid,layergroupid, "%(*)s") {
    return ((LayergroupRemoveLayerCommand*)arg)->m_layergroupid == layergroupid
        && ((LayergroupRemoveLayerCommand*)arg)->m_layerid == layerid;

  }
TEST_F(DBUSCommunicatorTest, RemovelayerFromlayerGroup) {

    EXPECT_CALL(this->mockCommandExecutor, execute( LayergroupRemoveLayerCommandEq(36u,77u))).Times(1);
    system((DBUSCOMMAND + std::string("RemoveLayerFromLayerGroup uint32:36 uint32:77")).c_str());
}

MATCHER_P2(LayerAddSurfaceCommandEq, surfaceid,layerid, "%(*)s") {
    return ((LayerAddSurfaceCommand*)arg)->m_layerid == layerid
        && ((LayerAddSurfaceCommand*)arg)->m_surfaceid == surfaceid;

  }
TEST_F(DBUSCommunicatorTest, AddSurfaceTolayer) {

    EXPECT_CALL(this->mockCommandExecutor, execute( LayerAddSurfaceCommandEq(36u,77u))).Times(1);
    system((DBUSCOMMAND + std::string("AddSurfaceToLayer uint32:36 uint32:77")).c_str());
}

MATCHER_P2(LayerRemoveSurfaceCommandEq, surfaceid,layerid, "%(*)s") {
    return ((LayerRemoveSurfaceCommand*)arg)->m_layerid == layerid
        && ((LayerRemoveSurfaceCommand*)arg)->m_surfaceid == surfaceid;

  }
TEST_F(DBUSCommunicatorTest, RemoveSurfaceFromlayer) {
    EXPECT_CALL(this->mockCommandExecutor, execute( LayerRemoveSurfaceCommandEq(36u,77u))).Times(1);
    system((DBUSCOMMAND + std::string("RemoveSurfaceFromLayer uint32:36 uint32:77")).c_str());
}

TEST_F(DBUSCommunicatorTest, CreateSurfaceGroup) {
    EXPECT_CALL(this->mockCommandExecutor, execute( CreateCommandEq(0u,0u,0u,TypeSurfaceGroup,PIXELFORMAT_R8))).Times(1);
    system((DBUSCOMMAND + std::string("CreateSurfaceGroup")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( CreateCommandEq(0u,0u,0u,TypeSurfaceGroup,PIXELFORMAT_R8))).Times(1);
    system((DBUSCOMMAND + std::string("CreateSurfaceGroup")).c_str());
}

TEST_F(DBUSCommunicatorTest, RemoveSurfaceGroup) {
    EXPECT_CALL(this->mockCommandExecutor, execute( RemoveCommandEq(8554u,TypeSurfaceGroup))).Times(1);
    system((DBUSCOMMAND + std::string("RemoveSurfaceGroup uint32:8554")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( RemoveCommandEq(34589u,TypeSurfaceGroup))).Times(1);
    system((DBUSCOMMAND + std::string("RemoveSurfaceGroup uint32:34589")).c_str());
}

TEST_F(DBUSCommunicatorTest, CreatelayerGroup) {
    EXPECT_CALL(this->mockCommandExecutor, execute( CreateCommandEq(0u,0u,0u,TypeLayerGroup,PIXELFORMAT_R8))).Times(1);
    system((DBUSCOMMAND + std::string("CreateLayerGroup")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( CreateCommandEq(0u,0u,0u,TypeLayerGroup,PIXELFORMAT_R8))).Times(1);
    system((DBUSCOMMAND + std::string("CreateLayerGroup")).c_str());
}

TEST_F(DBUSCommunicatorTest, RemovelayerGroup) {
    EXPECT_CALL(this->mockCommandExecutor, execute( RemoveCommandEq(8554u,TypeLayerGroup))).Times(1);
    system((DBUSCOMMAND + std::string("RemoveLayerGroup uint32:8554")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( RemoveCommandEq(34589u,TypeLayerGroup))).Times(1);
    system((DBUSCOMMAND + std::string("RemoveLayerGroup uint32:34589")).c_str());
}

MATCHER_P2(GetDimensionCommandEq, id, type, "%(*)s") {
    return ((GetDimensionCommand*)arg)->m_id == id
        && ((GetDimensionCommand*)arg)->m_type == type;
  }
TEST_F(DBUSCommunicatorTest, GetlayerDimension) {
    EXPECT_CALL(this->mockCommandExecutor, execute( GetDimensionCommandEq(8554u,TypeLayer))).Times(1);
    system((DBUSCOMMAND + std::string("GetLayerDimension uint32:8554")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( GetDimensionCommandEq(34589u,TypeLayer))).Times(1);
    system((DBUSCOMMAND + std::string("GetLayerDimension uint32:34589")).c_str());

}


TEST_F(DBUSCommunicatorTest, GetSurfaceDimension) {

    EXPECT_CALL(this->mockCommandExecutor, execute( GetDimensionCommandEq(8554u,TypeSurface))).Times(1);
    system((DBUSCOMMAND + std::string("GetSurfaceDimension uint32:8554")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( GetDimensionCommandEq(34589u,TypeSurface))).Times(1);
    system((DBUSCOMMAND + std::string("GetSurfaceDimension uint32:34589")).c_str());
}

MATCHER_P2(GetPixelformatCommandEq, id, type, "%(*)s") {
    return ((GetPixelformatCommand*)arg)->m_id == id
        && ((GetPixelformatCommand*)arg)->m_type == type;
  }
TEST_F(DBUSCommunicatorTest, GetSurfacePixelformat) {
    EXPECT_CALL(this->mockCommandExecutor, execute( GetPixelformatCommandEq(8554u,TypeSurface))).Times(1);
    system((DBUSCOMMAND + std::string("GetSurfacePixelformat uint32:8554")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( GetPixelformatCommandEq(34589u,TypeSurface))).Times(1);
    system((DBUSCOMMAND + std::string("GetSurfacePixelformat uint32:34589")).c_str());
}

TEST_F(DBUSCommunicatorTest, GetSurfaceOpacity) {
    EXPECT_CALL(this->mockCommandExecutor, execute( GetOpacityCommandEq(8554u,TypeSurface))).Times(1);
    system((DBUSCOMMAND + std::string("GetSurfaceOpacity uint32:8554")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( GetOpacityCommandEq(34589u,TypeSurface))).Times(1);
    system((DBUSCOMMAND + std::string("GetSurfaceOpacity uint32:34589")).c_str());
}

TEST_F(DBUSCommunicatorTest, GetSurfaceVisibility) {
    EXPECT_CALL(this->mockCommandExecutor, execute( GetOpacityCommandEq(8554u,TypeSurface))).Times(1);
    system((DBUSCOMMAND + std::string("GetSurfaceVisibility uint32:8554")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( GetOpacityCommandEq(34589u,TypeSurface))).Times(1);
    system((DBUSCOMMAND + std::string("GetSurfaceVisibility uint32:34589")).c_str());
}

MATCHER_P2(GetVisibilityCommandEq, id,type, "%(*)s") {
    return ((GetVisibilityCommand*)arg)->m_id == id
            && ((GetVisibilityCommand*)arg)->m_type == type;

  }
TEST_F(DBUSCommunicatorTest, GetlayerVisibility) {
    EXPECT_CALL(this->mockCommandExecutor, execute( GetVisibilityCommandEq(8554u,TypeLayer))).Times(1);
    system((DBUSCOMMAND + std::string("GetLayerVisibility uint32:8554")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( GetVisibilityCommandEq(34589u,TypeLayer))).Times(1);
    system((DBUSCOMMAND + std::string("GetLayerVisibility uint32:34589")).c_str());
}

//MATCHER_P2(RenderOrderTester,commandtype,list, "") {
//    LOG_ERROR("in tester",0);
//    commandtype_type command = (commandtype_type)arg;
//    testing::internal::ElementsAreArrayMatcher<int> matcher = ElementsAreArray(command->array,3);
//    std::vector<uint> l;
//    testing::Matcher<std::vector<uint, std::allocator<uint> > > matcher2 = matcher.operator Matcher<std::vector<uint> >();
//    bool result = matcher2.Matches(list);
//    return result;
//
//}
//
//TEST_F(DBUSCommunicatorTest, SetRenderOrderOflayers) {
//
////  DBUSClient* client = dbus_helper();
////  if (NULL== client)
////      LOG_ERROR("TEST", "client is null");
////
////  std::vector<uint> expected;
////  expected.push_back(4);
////  expected.push_back(47);
////  expected.push_back(49);
////  SetLayerOrderCommand * p;
//// EXPECT_CALL(this->mockCommandExecutor,execute(RenderOrderTester(p,expected))).Times(1);
////  uint display = 0;
////  client->SetRenderOrderOfLayers(expected,display);
//
//}
//
//TEST_F(DBUSCommunicatorTest, SetRenderOrderWithinlayer) {
////  DBUSClient* client = dbus_helper();
////
////  std::vector<uint> expected;
////  expected.push_back(4);
////  expected.push_back(47);
////  expected.push_back(49);
////  SetOrderWithinLayerCommand * p;
//// EXPECT_CALL(this->mockCommandExecutor,execute(AllOf(
////                          RenderOrderTester(p,expected),
////                          FieldCast(&SetOrderWithinLayerCommand::layerid,55))
////                          )).Times(1);
////  client->SetSurfaceRenderOrderWithinLayer(55,expected);
//
//
//}

TEST_F(DBUSCommunicatorTest, GetlayerType) {
    DefaultValue<Scene*>::Set((Scene*)&layerlist);    
    EXPECT_CALL(this->layerlist, getLayer(Eq(8554u))).Times(1);
    system((DBUSCOMMAND + std::string("GetLayerType uint32:8554")).c_str());
}

TEST_F(DBUSCommunicatorTest, GetlayertypeCapabilities) {
 EXPECT_CALL(this->mockCommandExecutor, getLayerTypeCapabilities(Eq(367))).Times(1);
  system((DBUSCOMMAND + std::string("GetLayertypeCapabilities uint32:367")).c_str());
}

TEST_F(DBUSCommunicatorTest, GetlayerCapabilities) {
    Scene scene;
    DefaultValue<Scene*>::Set((Scene*)&layerlist);    
    EXPECT_CALL(this->layerlist, getLayer(Eq(367u))).Times(1);
    system((DBUSCOMMAND + std::string("GetLayerCapabilities uint32:367")).c_str());

}

MATCHER_P2(CreateShaderCommandEq, _vertName,_fragName, "%(*)s") {
    return ((CreateShaderCommand*)arg)->getVertName() == _vertName
            && ((CreateShaderCommand*)arg)->getFragName() == _fragName;

  }

TEST_F(DBUSCommunicatorTest, CreateShader) {
    EXPECT_CALL(this->mockCommandExecutor, execute( CreateShaderCommandEq(std::string("test123.glslv"),std::string("differentshader.glslv")))).Times(1);
    system((DBUSCOMMAND + std::string("CreateShader string:test123.glslv string:differentshader.glslv")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( CreateShaderCommandEq(std::string("/usr/lib/shadertest.glslv"),std::string("foobar")))).Times(1);
    system((DBUSCOMMAND + std::string("CreateShader string:/usr/lib/shadertest.glslv string:foobar")).c_str());
}

MATCHER_P(DestroyShaderCommandEq, id, "%(*)s") {
    return ((DestroyShaderCommand*)arg)->getShaderID() == id;

  }
TEST_F(DBUSCommunicatorTest, DestroyShaderCommand) {
    EXPECT_CALL(this->mockCommandExecutor, execute( DestroyShaderCommandEq(567u))).Times(1);
    system((DBUSCOMMAND + std::string("DestroyShader uint32:567")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( DestroyShaderCommandEq(185u))).Times(1);
    system((DBUSCOMMAND + std::string("DestroyShader uint32:185")).c_str());
}

MATCHER_P2(SetShaderCommandEq, id,shaderid, "%(*)s") {
    return ((SetShaderCommand*)arg)->getID() == id
        && ((SetShaderCommand*)arg)->getShaderID() == shaderid;

  }
TEST_F(DBUSCommunicatorTest, SetShaderCommand) {

    EXPECT_CALL(this->mockCommandExecutor, execute( SetShaderCommandEq(987u, 567u))).Times(1);
    system((DBUSCOMMAND + std::string("SetShader uint32:987 uint32:567")).c_str());

    EXPECT_CALL(this->mockCommandExecutor, execute( SetShaderCommandEq(1u, 998877u))).Times(1);
    system((DBUSCOMMAND + std::string("SetShader uint32:1 uint32:998877")).c_str());
}

MATCHER_P(UniformsMatcher,expectedlist,"") {
    SetUniformsCommand* command = (SetUniformsCommand*)arg;
    std::vector<std::string> uniforms = command->getUniforms();

    EXPECT_THAT(uniforms,ElementsAreArray(*expectedlist));
    //EXPECT_THAT(uniforms,ElementsAreArray(expectedlist));
    return true;
}

MATCHER_P(SetUniformsCommandEq, _shaderid, "%(*)s") {
    return ((SetUniformsCommand*)arg)->getShaderId() == _shaderid;

  }

TEST_F(DBUSCommunicatorTest, SetUniformsCommand) {

    std::string expected1[] = { "uRotAngle 1f 1 0","uRotVector 3f 1 1 0 0" };
    EXPECT_CALL(this->mockCommandExecutor, execute( AllOf( SetUniformsCommandEq(1u),
                                                                          UniformsMatcher(&expected1)
                                               ))).Times(1);
    system((DBUSCOMMAND + std::string("SetUniforms uint32:1 array:string:\"uRotAngle 1f 1 0\",\"uRotVector 3f 1 1 0 0\"")).c_str());

    std::string expected2[] = { "teststring foobar" };
    EXPECT_CALL(this->mockCommandExecutor, execute( AllOf( SetUniformsCommandEq(17346u),
                                                           UniformsMatcher(&expected2)
                                                   ))).Times(1);
    system((DBUSCOMMAND + std::string("SetUniforms uint32:17346 array:string:\"teststring foobar\"")).c_str());
}

#endif /* DBUSCommunicatorTest_H_ */
