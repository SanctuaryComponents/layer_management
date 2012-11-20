/***************************************************************************
 *
 * Copyright 2012 BMW Car IT GmbH
 * Copyright (C) 2012 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
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
#ifndef __GENERICCOMMUNICATOR_H__
#define __GENERICCOMMUNICATOR_H__

/**
 * \defgroup GenericCommunicator Generic Communicator API
 */

#include "ICommunicator.h"
#include "Log.h"
#include "IpcModuleLoader.h"
#include "ObjectType.h"
#include <map>
#include <list>
#include <string>

class GenericCommunicator;
class GraphicalObject;

//=============================================================================
// internal types
//=============================================================================
typedef void (GenericCommunicator::*CallBackMethod)(t_ilm_message);

struct MethodTable
{
    const char *name;
    CallBackMethod function;
};
typedef std::map<std::string,MethodTable> CallBackTable;


//=============================================================================
// interface
//=============================================================================
class GenericCommunicator: public ICommunicator
{
public:
    GenericCommunicator(ICommandExecutor* executor);
    virtual ~GenericCommunicator() {}
    virtual bool start();
    virtual void stop();
    virtual void process(int timeout_ms);
    virtual void setdebug(bool onoff);

private:
    void ServiceConnect(t_ilm_message message);
    void ServiceDisconnect(t_ilm_message message);
    void Debug(t_ilm_message message);
    void ScreenShot(t_ilm_message message);
    void ScreenShotOfLayer(t_ilm_message message);
    void ScreenShotOfSurface(t_ilm_message message);
    void GetScreenResolution(t_ilm_message message);
    void GetNumberOfHardwareLayers(t_ilm_message message);
    void GetScreenIDs(t_ilm_message message);
    void ListAllLayerIDS(t_ilm_message message);
    void ListAllLayerIDsOnScreen(t_ilm_message message);
    void ListAllSurfaceIDS(t_ilm_message message);
    void ListAllLayerGroupIDS(t_ilm_message message);
    void ListAllSurfaceGroupIDS(t_ilm_message message);
    void ListSurfacesOfSurfacegroup(t_ilm_message message);
    void ListLayersOfLayergroup(t_ilm_message message);
    void ListSurfaceofLayer(t_ilm_message message);
    void GetPropertiesOfSurface(t_ilm_message message);
    void GetPropertiesOfLayer(t_ilm_message message);
    void CreateSurface(t_ilm_message message);
    void InitializeSurface(t_ilm_message message);
    void InitializeSurfaceFromId(t_ilm_message message);
    void CreateSurfaceFromId(t_ilm_message message);
    void SetSurfaceNativeContent(t_ilm_message message);
    void RemoveSurfaceNativeContent(t_ilm_message message);
    void RemoveSurface(t_ilm_message message);
    void CreateLayer(t_ilm_message message);
    void CreateLayerFromId(t_ilm_message message);
    void CreateLayerWithDimension(t_ilm_message message);
    void CreateLayerFromIdWithDimension(t_ilm_message message);
    void RemoveLayer(t_ilm_message message);
    void AddSurfaceToSurfaceGroup(t_ilm_message message);
    void RemoveSurfaceFromSurfaceGroup(t_ilm_message message);
    void AddLayerToLayerGroup(t_ilm_message message);
    void RemoveLayerFromLayerGroup(t_ilm_message message);
    void AddSurfaceToLayer(t_ilm_message message);
    void RemoveSurfaceFromLayer(t_ilm_message message);
    void CreateSurfaceGroup(t_ilm_message message);
    void CreateSurfaceGroupFromId(t_ilm_message message);
    void RemoveSurfaceGroup(t_ilm_message message);
    void CreateLayerGroup(t_ilm_message message);
    void CreateLayerGroupFromId(t_ilm_message message);
    void RemoveLayerGroup(t_ilm_message message);
    void SetSurfaceSourceRegion(t_ilm_message message);
    void SetLayerSourceRegion(t_ilm_message message);
    void SetSurfaceDestinationRegion(t_ilm_message message);
    void SetSurfacePosition(t_ilm_message message);
    void GetSurfacePosition(t_ilm_message message);
    void SetSurfaceDimension(t_ilm_message message);
    void SetLayerDestinationRegion(t_ilm_message message);
    void SetLayerPosition(t_ilm_message message);
    void GetLayerPosition(t_ilm_message message);
    void SetLayerDimension(t_ilm_message message);
    void GetLayerDimension(t_ilm_message message);
    void GetSurfaceDimension(t_ilm_message message);
    void SetSurfaceOpacity(t_ilm_message message);
    void SetLayerOpacity(t_ilm_message message);
    void SetSurfacegroupOpacity(t_ilm_message message);
    void SetLayergroupOpacity(t_ilm_message message);
    void GetSurfaceOpacity(t_ilm_message message);
    void GetLayerOpacity(t_ilm_message message);
    void GetSurfacegroupOpacity(t_ilm_message message);
    void GetLayergroupOpacity(t_ilm_message message);
    void SetSurfaceOrientation(t_ilm_message message);
    void GetSurfaceOrientation(t_ilm_message message);
    void SetLayerOrientation(t_ilm_message message);
    void GetLayerOrientation(t_ilm_message message);
    void GetSurfacePixelformat(t_ilm_message message);
    void SetSurfaceVisibility(t_ilm_message message);
    void SetLayerVisibility(t_ilm_message message);
    void GetSurfaceVisibility(t_ilm_message message);
    void GetLayerVisibility(t_ilm_message message);
    void SetSurfacegroupVisibility(t_ilm_message message);
    void SetLayergroupVisibility(t_ilm_message message);
    void SetRenderOrderOfLayers(t_ilm_message message);
    void SetSurfaceRenderOrderWithinLayer(t_ilm_message message);
    void GetLayerType(t_ilm_message message);
    void GetLayertypeCapabilities(t_ilm_message message);
    void GetLayerCapabilities(t_ilm_message message);
    void FadeIn(t_ilm_message message);
    void SynchronizedFade(t_ilm_message message);
    void FadeOut(t_ilm_message message);
    void Exit(t_ilm_message message);
    void CommitChanges(t_ilm_message message);
    void CreateShader(t_ilm_message message);
    void DestroyShader(t_ilm_message message);
    void SetShader(t_ilm_message message);
    void SetUniforms(t_ilm_message message);
    void SetKeyboardFocusOn(t_ilm_message message);
    void GetKeyboardFocusSurfaceId(t_ilm_message message);
    void UpdateInputEventAcceptanceOn(t_ilm_message message);
    void SetSurfaceChromaKey(t_ilm_message message);
    void SetLayerChromaKey(t_ilm_message message);
    void LayerAddNotification(t_ilm_message message);
    void SurfaceAddNotification(t_ilm_message message);
    void LayerRemoveNotification(t_ilm_message message);
    void SurfaceRemoveNotification(t_ilm_message message);

    virtual HealthCondition getHealth();

private:
    void RemoveApplicationReference(char* owner);
    void processNotificationQueue();
    void sendNotification(GraphicalObject* object, t_ilm_notification_mask mask);

private:
    IpcModule m_ipcModule;
    CallBackTable m_callBackTable;
    bool m_running;
    unsigned long int mThreadId;
};

#endif // __GENERICCOMMUNICATOR_H__
