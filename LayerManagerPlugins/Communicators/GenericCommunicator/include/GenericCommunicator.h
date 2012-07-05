/***************************************************************************
 *
 * Copyright 2012 BMW Car IT GmbH
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
 * \defgroup GenericCommunicator Generic Communicator API
 */

#include "ICommunicator.h"
#include "Log.h"
#include "IpcModuleLoader.h"

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
#include "SurfaceGetPixelformatCommand.h"
#include "LayerGetVisibilityCommand.h"
#include "SurfaceGetVisibilityCommand.h"
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
#include "SurfaceRemoveNativeContentCommand.h"


class GenericCommunicator;
class DBusConnection;
class DBusMessage;

typedef void (GenericCommunicator::*CallBackMethod)(void);

struct MethodTable
{
    const char *name;
    CallBackMethod function;
};

typedef std::map<std::string,MethodTable> CallBackTable;

class GenericCommunicator: public ICommunicator
{
public:
    GenericCommunicator(ICommandExecutor* executor);
    virtual ~GenericCommunicator() {}
    virtual bool start();
    virtual void stop();
    virtual void process(int timeout_ms);
    virtual void setdebug(bool onoff);
    void ServiceConnect();
    void ServiceDisconnect();
    void Debug();
    void ScreenShot();
    void ScreenShotOfLayer();
    void ScreenShotOfSurface();
    void GetScreenResolution();
    void GetNumberOfHardwareLayers();
    void GetScreenIDs();
    void ListAllLayerIDS();
    void ListAllLayerIDsOnScreen();
    void ListAllSurfaceIDS();
    void ListAllLayerGroupIDS();
    void ListAllSurfaceGroupIDS();
    void ListSurfacesOfSurfacegroup();
    void ListLayersOfLayergroup();
    void ListSurfaceofLayer();
    void GetPropertiesOfSurface();
    void GetPropertiesOfLayer();
    void CreateSurface();
    void InitializeSurface();
    void InitializeSurfaceFromId();
    void CreateSurfaceFromId();
    void SetSurfaceNativeContent();
    void RemoveSurfaceNativeContent();
    void RemoveSurface();
    void CreateLayer();
    void CreateLayerFromId();
    void CreateLayerWithDimension();
    void CreateLayerFromIdWithDimension();
    void RemoveLayer();
    void AddSurfaceToSurfaceGroup();
    void RemoveSurfaceFromSurfaceGroup();
    void AddLayerToLayerGroup();
    void RemoveLayerFromLayerGroup();
    void AddSurfaceToLayer();
    void RemoveSurfaceFromLayer();
    void CreateSurfaceGroup();
    void CreateSurfaceGroupFromId();
    void RemoveSurfaceGroup();
    void CreateLayerGroup();
    void CreateLayerGroupFromId();
    void RemoveLayerGroup();
    void SetSurfaceSourceRegion();
    void SetLayerSourceRegion();
    void SetSurfaceDestinationRegion();
    void SetSurfacePosition();
    void GetSurfacePosition();
    void SetSurfaceDimension();
    void SetLayerDestinationRegion();
    void SetLayerPosition();
    void GetLayerPosition();
    void SetLayerDimension();
    void GetLayerDimension();
    void GetSurfaceDimension();
    void SetSurfaceOpacity();
    void SetLayerOpacity();
    void SetSurfacegroupOpacity();
    void SetLayergroupOpacity();
    void GetSurfaceOpacity();
    void GetLayerOpacity();
    void GetSurfacegroupOpacity();
    void GetLayergroupOpacity();
    void SetSurfaceOrientation();
    void GetSurfaceOrientation();
    void SetLayerOrientation();
    void GetLayerOrientation();
    void GetSurfacePixelformat();
    void SetSurfaceVisibility();
    void SetLayerVisibility();
    void GetSurfaceVisibility();
    void GetLayerVisibility();
    void SetSurfacegroupVisibility();
    void SetLayergroupVisibility();
    void SetRenderOrderOfLayers();
    void SetSurfaceRenderOrderWithinLayer();
    void GetLayerType();
    void GetLayertypeCapabilities();
    void GetLayerCapabilities();
    void FadeIn();
    void SynchronizedFade();
    void FadeOut();
    void Exit();
    void CommitChanges();
    void CreateShader();
    void DestroyShader();
    void SetShader();
    void SetUniforms();

private:
    void RemoveApplicationReference(char* owner);

private:
    IpcModule m_ipcModule;
    CallBackTable m_callBackTable;
    bool m_running;
};

#endif /* _DBUSCOMMUNICATOR_H_ */
