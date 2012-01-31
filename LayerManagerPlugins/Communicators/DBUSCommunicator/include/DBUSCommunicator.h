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
#ifndef _DBUSCOMMUNICATOR_H_
#define _DBUSCOMMUNICATOR_H_

/**
 * \defgroup DbusCommunicator DBUS Communicator API
 */

#include "ICommunicator.h"
#include "Log.h"

#include "ICommandExecutor.h"
#include "IApplicationReference.h"
#include "ApplicationReferenceMap.h"
#include "CommitCommand.h"
#include "LayerCreateCommand.h"
#include "LayergroupCreateCommand.h"
#include "SurfaceCreateCommand.h"
#include "SurfacegroupCreateCommand.h"
#include "SurfaceGetDimensionCommand.h"
#include "LayerGetDimensionCommand.h"
#include "SurfaceGetOpacityCommand.h"
#include "LayerGetOpacityCommand.h"
#include "SurfacegroupGetOpacityCommand.h"
#include "LayergroupGetOpacityCommand.h"
#include "SurfaceGetPixelformatCommand.h"
#include "LayerGetVisibilityCommand.h"
//#include "LayergroupGetVisibilityCommand.h"
#include "SurfaceGetVisibilityCommand.h"
//#include "SurfacegroupGetVisibilityCommand.h"
#include "LayerAddSurfaceCommand.h"
#include "LayerRemoveSurfaceCommand.h"
#include "LayerRemoveCommand.h"
#include "SurfaceRemoveCommand.h"
#include "LayergroupRemoveCommand.h"
#include "SurfacegroupRemoveCommand.h"
#include "SurfaceGetOrientationCommand.h"
#include "LayerGetOrientationCommand.h"
#include "LayergroupAddLayerCommand.h"
#include "LayergroupRemoveLayerCommand.h"
#include "LayerSetDestinationRectangleCommand.h"
#include "SurfaceSetDestinationRectangleCommand.h"
#include "LayerSetOpacityCommand.h"
#include "LayergroupSetOpacityCommand.h"
#include "SurfaceSetOpacityCommand.h"
#include "SurfacegroupSetOpacityCommand.h"
#include "LayerSetSourceRectangleCommand.h"
#include "SurfaceSetSourceRectangleCommand.h"
#include "LayerSetOrientationCommand.h"
#include "SurfaceSetOrientationCommand.h"
#include "SurfacegroupAddSurfaceCommand.h"
#include "SurfacegroupRemoveSurfaceCommand.h"
#include "LayerSetVisibilityCommand.h"
#include "SurfaceSetVisibilityCommand.h"
#include "LayergroupSetVisibilityCommand.h"
#include "SurfacegroupSetVisibilityCommand.h"
#include "DebugCommand.h"
#include "ExitCommand.h"
#include "ScreenSetRenderOrderCommand.h"
#include "LayerSetRenderOrderCommand.h"
#include "LayerSetDimensionCommand.h"
#include "SurfaceSetDimensionCommand.h"
#include "LayerSetPositionCommand.h"
#include "SurfaceSetPositionCommand.h"
#include "LayerGetPositionCommand.h"
#include "SurfaceGetPositionCommand.h"
#include "ShaderCreateCommand.h"
#include "ShaderDestroyCommand.h"
#include "SurfaceSetShaderCommand.h"
#include "ShaderSetUniformsCommand.h"
#include "ScreenDumpCommand.h"
#include "LayerDumpCommand.h"
#include "SurfaceDumpCommand.h"
#include "SurfaceSetNativeContentCommand.h"

#include <dbus/dbus.h>
#include "DBUSTypes.h"

class DBUSCommunicator: public ICommunicator
{
public:
    DBUSCommunicator(ICommandExecutor* executor);
    virtual ~DBUSCommunicator() {}
    virtual bool start();
    virtual void stop();
    virtual void process(int timeout_ms);
    virtual void setdebug(bool onoff);
    void ServiceConnect(DBusConnection* conn, DBusMessage* msg);
    void ServiceDisconnect(DBusConnection* conn, DBusMessage* msg);    
    void Debug(DBusConnection* conn, DBusMessage* msg);
    void ScreenShot(DBusConnection* conn, DBusMessage* msg);
    void ScreenShotOfLayer(DBusConnection* conn, DBusMessage* msg);
    void ScreenShotOfSurface(DBusConnection* conn, DBusMessage* msg);
    void GetScreenResolution(DBusConnection* conn, DBusMessage* msg);
    void GetNumberOfHardwareLayers(DBusConnection* conn, DBusMessage* msg);
    void GetScreenIDs(DBusConnection* conn, DBusMessage* msg);
    void ListAllLayerIDS(DBusConnection* conn, DBusMessage* msg);
    void ListAllLayerIDsOnScreen(DBusConnection* conn, DBusMessage* msg);
    void ListAllSurfaceIDS(DBusConnection* conn, DBusMessage* msg);
    void ListAllLayerGroupIDS(DBusConnection* conn, DBusMessage* msg);
    void ListAllSurfaceGroupIDS(DBusConnection* conn, DBusMessage* msg);
    void ListSurfacesOfSurfacegroup(DBusConnection* conn, DBusMessage* msg);
    void ListLayersOfLayergroup(DBusConnection* conn, DBusMessage* msg);
    void ListSurfaceofLayer(DBusConnection* conn, DBusMessage* msg);
    void GetPropertiesOfSurface(DBusConnection* conn, DBusMessage* msg);
    void GetPropertiesOfLayer(DBusConnection* conn, DBusMessage* msg);
    void CreateSurface(DBusConnection* conn, DBusMessage* msg);
    void InitializeSurface(DBusConnection* conn, DBusMessage* msg);
    void InitializeSurfaceFromId(DBusConnection* conn, DBusMessage* msg);
    void CreateSurfaceFromId(DBusConnection* conn, DBusMessage* msg);
    void SetSurfaceNativeContent(DBusConnection* conn, DBusMessage* msg);
    void RemoveSurface(DBusConnection* conn, DBusMessage* msg);
    void CreateLayer(DBusConnection* conn, DBusMessage* msg);
    void CreateLayerFromId(DBusConnection* conn, DBusMessage* msg);
    void CreateLayerWithDimension(DBusConnection* conn, DBusMessage* msg);
    void CreateLayerFromIdWithDimension(DBusConnection* conn, DBusMessage* msg);
    void RemoveLayer(DBusConnection* conn, DBusMessage* msg);
    void AddSurfaceToSurfaceGroup(DBusConnection* conn, DBusMessage* msg);
    void RemoveSurfaceFromSurfaceGroup(DBusConnection* conn, DBusMessage* msg);
    void AddLayerToLayerGroup(DBusConnection* conn, DBusMessage* msg);
    void RemoveLayerFromLayerGroup(DBusConnection* conn, DBusMessage* msg);
    void AddSurfaceToLayer(DBusConnection* conn, DBusMessage* msg);
    void RemoveSurfaceFromLayer(DBusConnection* conn, DBusMessage* msg);
    void CreateSurfaceGroup(DBusConnection* conn, DBusMessage* msg);
    void CreateSurfaceGroupFromId(DBusConnection* conn, DBusMessage* msg);
    void RemoveSurfaceGroup(DBusConnection* conn, DBusMessage* msg);
    void CreateLayerGroup(DBusConnection* conn, DBusMessage* msg);
    void CreateLayerGroupFromId(DBusConnection* conn, DBusMessage* msg);
    void RemoveLayerGroup(DBusConnection* conn, DBusMessage* msg);
    void SetSurfaceSourceRegion(DBusConnection* conn, DBusMessage* msg);
    void SetLayerSourceRegion(DBusConnection* conn, DBusMessage* msg);
    void SetSurfaceDestinationRegion(DBusConnection* conn, DBusMessage* msg);
    void SetSurfacePosition(DBusConnection* conn, DBusMessage* msg);
    void GetSurfacePosition(DBusConnection* conn, DBusMessage* msg);
    void SetSurfaceDimension(DBusConnection* conn, DBusMessage* msg);
    void SetLayerDestinationRegion(DBusConnection* conn, DBusMessage* msg);
    void SetLayerPosition(DBusConnection* conn, DBusMessage* msg);
    void GetLayerPosition(DBusConnection* conn, DBusMessage* msg);
    void SetLayerDimension(DBusConnection* conn, DBusMessage* msg);
    void GetLayerDimension(DBusConnection* conn, DBusMessage* msg);
    void GetSurfaceDimension(DBusConnection* conn, DBusMessage* msg);
    void SetSurfaceOpacity(DBusConnection* conn, DBusMessage* msg);
    void SetLayerOpacity(DBusConnection* conn, DBusMessage* msg);
    void SetSurfacegroupOpacity(DBusConnection* conn, DBusMessage* msg);
    void SetLayergroupOpacity(DBusConnection* conn, DBusMessage* msg);
    void GetSurfaceOpacity(DBusConnection* conn, DBusMessage* msg);
    void GetLayerOpacity(DBusConnection* conn, DBusMessage* msg);
    void GetSurfacegroupOpacity(DBusConnection* conn, DBusMessage* msg);
    void GetLayergroupOpacity(DBusConnection* conn, DBusMessage* msg);
    void SetSurfaceOrientation(DBusConnection* conn, DBusMessage* msg);
    void GetSurfaceOrientation(DBusConnection* conn, DBusMessage* msg);
    void SetLayerOrientation(DBusConnection* conn, DBusMessage* msg);
    void GetLayerOrientation(DBusConnection* conn, DBusMessage* msg);
    void GetSurfacePixelformat(DBusConnection* conn, DBusMessage* msg);
    void SetSurfaceVisibility(DBusConnection* conn, DBusMessage* msg);
    void SetLayerVisibility(DBusConnection* conn, DBusMessage* msg);
    void GetSurfaceVisibility(DBusConnection* conn, DBusMessage* msg);
    void GetLayerVisibility(DBusConnection* conn, DBusMessage* msg);
    void SetSurfacegroupVisibility(DBusConnection* conn, DBusMessage* msg);
    void SetLayergroupVisibility(DBusConnection* conn, DBusMessage* msg);
    void SetRenderOrderOfLayers(DBusConnection* conn, DBusMessage* msg);
    void SetSurfaceRenderOrderWithinLayer(DBusConnection* conn, DBusMessage* msg);
    void GetLayerType(DBusConnection* conn, DBusMessage* msg);
    void GetLayertypeCapabilities(DBusConnection* conn, DBusMessage* msg);
    void GetLayerCapabilities(DBusConnection* conn, DBusMessage* msg);
    void FadeIn(DBusConnection* conn, DBusMessage* msg);
    void SynchronizedFade(DBusConnection* conn, DBusMessage* msg);
    void FadeOut(DBusConnection* conn, DBusMessage* msg);
    void Exit(DBusConnection* conn, DBusMessage* msg);
    void CommitChanges(DBusConnection* conn, DBusMessage* msg);
    void CreateShader(DBusConnection* conn, DBusMessage* msg);
    void DestroyShader(DBusConnection* conn, DBusMessage* msg);
    void SetShader(DBusConnection* conn, DBusMessage* msg);
    void SetUniforms(DBusConnection* conn, DBusMessage* msg);
    DBusHandlerResult delegateMessage(DBusConnection* conn, DBusMessage* msg);


private:
    void AddClientWatch(DBusConnection *conn, char* sender);
    void RemoveApplicationReference(char* owner);
    static DBusHandlerResult processMessageFunc(DBusConnection* conn,DBusMessage* msg, void *user_data);
    static void unregisterMessageFunc(DBusConnection* conn, void *user_data);
    bool m_running;
};

#endif /* _DBUSCOMMUNICATOR_H_ */
