/***************************************************************************
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

#include "GenericCommunicator.h"
#include "ilm_types.h"
#include "Log.h"

#include "ICommandExecutor.h"
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
#include "SurfaceSetKeyboardFocusCommand.h"
#include "SurfaceGetKeyboardFocusCommand.h"
#include "SurfaceUpdateInputEventAcceptance.h"

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>

#define DEFAULT_SCREEN 0

const char* RESSOURCE_ALREADY_INUSE = "Ressource already in use";
const char* INVALID_ARGUMENT = "Invalid argument";
const char* RESSOURCE_NOT_FOUND = "Ressource not found";


GenericCommunicator::GenericCommunicator(ICommandExecutor* executor)
: ICommunicator(executor)
, m_running(ILM_FALSE)
{

    MethodTable manager_methods[] =
    {
        { "ServiceConnect",                   &GenericCommunicator::ServiceConnect },
        { "ServiceDisconnect",                &GenericCommunicator::ServiceDisconnect },
        { "Debug",                            &GenericCommunicator::Debug },
        { "ScreenShot",                       &GenericCommunicator::ScreenShot },
        { "ScreenShotOfLayer",                &GenericCommunicator::ScreenShotOfLayer },
        { "ScreenShotOfSurface",              &GenericCommunicator::ScreenShotOfSurface },
        { "GetScreenResolution",              &GenericCommunicator::GetScreenResolution },
        { "GetNumberOfHardwareLayers",        &GenericCommunicator::GetNumberOfHardwareLayers },
        { "GetScreenIDs",                     &GenericCommunicator::GetScreenIDs },
        { "ListAllLayerIDS",                  &GenericCommunicator::ListAllLayerIDS },
        { "ListAllLayerIDsOnScreen",          &GenericCommunicator::ListAllLayerIDsOnScreen },
        { "ListAllSurfaceIDS",                &GenericCommunicator::ListAllSurfaceIDS },
        { "ListAllLayerGroupIDS",             &GenericCommunicator::ListAllLayerGroupIDS },
        { "ListAllSurfaceGroupIDS",           &GenericCommunicator::ListAllSurfaceGroupIDS },
        { "ListSurfacesOfSurfacegroup",       &GenericCommunicator::ListSurfacesOfSurfacegroup },
        { "ListLayersOfLayergroup",           &GenericCommunicator::ListLayersOfLayergroup },
        { "ListSurfaceofLayer",               &GenericCommunicator::ListSurfaceofLayer },
        { "GetPropertiesOfSurface",           &GenericCommunicator::GetPropertiesOfSurface },
        { "GetPropertiesOfLayer",             &GenericCommunicator::GetPropertiesOfLayer },
        { "CreateSurface",                    &GenericCommunicator::CreateSurface },
        { "CreateSurfaceFromId",              &GenericCommunicator::CreateSurfaceFromId },
        { "InitializeSurface",                &GenericCommunicator::InitializeSurface },
        { "InitializeSurfaceFromId",          &GenericCommunicator::InitializeSurfaceFromId },
        { "SetSurfaceNativeContent",          &GenericCommunicator::SetSurfaceNativeContent },
        { "RemoveSurfaceNativeContent",       &GenericCommunicator::RemoveSurfaceNativeContent },
        { "RemoveSurface",                    &GenericCommunicator::RemoveSurface },
        { "CreateLayer",                      &GenericCommunicator::CreateLayer },
        { "CreateLayerFromId",                &GenericCommunicator::CreateLayerFromId },
        { "CreateLayerWithDimension",         &GenericCommunicator::CreateLayerWithDimension },
        { "CreateLayerFromIdWithDimension",   &GenericCommunicator::CreateLayerFromIdWithDimension },
        { "RemoveLayer",                      &GenericCommunicator::RemoveLayer },
        { "AddSurfaceToSurfaceGroup",         &GenericCommunicator::AddSurfaceToSurfaceGroup },
        { "RemoveSurfaceFromSurfaceGroup",    &GenericCommunicator::RemoveSurfaceFromSurfaceGroup },
        { "AddLayerToLayerGroup",             &GenericCommunicator::AddLayerToLayerGroup },
        { "RemoveLayerFromLayerGroup",        &GenericCommunicator::RemoveLayerFromLayerGroup },
        { "AddSurfaceToLayer",                &GenericCommunicator::AddSurfaceToLayer },
        { "RemoveSurfaceFromLayer",           &GenericCommunicator::RemoveSurfaceFromLayer },
        { "CreateSurfaceGroup",               &GenericCommunicator::CreateSurfaceGroup },
        { "CreateSurfaceGroupFromId",         &GenericCommunicator::CreateSurfaceGroupFromId },
        { "RemoveSurfaceGroup",               &GenericCommunicator::RemoveSurfaceGroup },
        { "CreateLayerGroup",                 &GenericCommunicator::CreateLayerGroup },
        { "CreateLayerGroupFromId",           &GenericCommunicator::CreateLayerGroupFromId },
        { "RemoveLayerGroup",                 &GenericCommunicator::RemoveLayerGroup },
        { "SetSurfaceSourceRegion",           &GenericCommunicator::SetSurfaceSourceRegion },
        { "SetLayerSourceRegion",             &GenericCommunicator::SetLayerSourceRegion },
        { "SetSurfaceDestinationRegion",      &GenericCommunicator::SetSurfaceDestinationRegion },
        { "SetSurfacePosition",               &GenericCommunicator::SetSurfacePosition },
        { "GetSurfacePosition",               &GenericCommunicator::GetSurfacePosition },
        { "SetSurfaceDimension",              &GenericCommunicator::SetSurfaceDimension },
        { "SetLayerDestinationRegion",        &GenericCommunicator::SetLayerDestinationRegion },
        { "SetLayerPosition",                 &GenericCommunicator::SetLayerPosition },
        { "GetLayerPosition",                 &GenericCommunicator::GetLayerPosition },
        { "SetLayerDimension",                &GenericCommunicator::SetLayerDimension },
        { "GetLayerDimension",                &GenericCommunicator::GetLayerDimension },
        { "GetSurfaceDimension",              &GenericCommunicator::GetSurfaceDimension },
        { "SetSurfaceOpacity",                &GenericCommunicator::SetSurfaceOpacity },
        { "SetLayerOpacity",                  &GenericCommunicator::SetLayerOpacity },
        { "SetSurfacegroupOpacity",           &GenericCommunicator::SetSurfacegroupOpacity },
        { "SetLayergroupOpacity",             &GenericCommunicator::SetLayergroupOpacity },
        { "GetSurfaceOpacity",                &GenericCommunicator::GetSurfaceOpacity },
        { "GetLayerOpacity",                  &GenericCommunicator::GetLayerOpacity },
        { "SetSurfaceOrientation",            &GenericCommunicator::SetSurfaceOrientation },
        { "GetSurfaceOrientation",            &GenericCommunicator::GetSurfaceOrientation },
        { "SetLayerOrientation",              &GenericCommunicator::SetLayerOrientation },
        { "GetLayerOrientation",              &GenericCommunicator::GetLayerOrientation },
        { "GetSurfacePixelformat",            &GenericCommunicator::GetSurfacePixelformat },
        { "SetSurfaceVisibility",             &GenericCommunicator::SetSurfaceVisibility },
        { "SetLayerVisibility",               &GenericCommunicator::SetLayerVisibility },
        { "GetSurfaceVisibility",             &GenericCommunicator::GetSurfaceVisibility },
        { "GetLayerVisibility",               &GenericCommunicator::GetLayerVisibility },
        { "SetSurfacegroupVisibility",        &GenericCommunicator::SetSurfacegroupVisibility },
        { "SetLayergroupVisibility",          &GenericCommunicator::SetLayergroupVisibility },
        { "SetRenderOrderOfLayers",           &GenericCommunicator::SetRenderOrderOfLayers },
        { "SetSurfaceRenderOrderWithinLayer", &GenericCommunicator::SetSurfaceRenderOrderWithinLayer },
        { "GetLayerType",                     &GenericCommunicator::GetLayerType },
        { "GetLayertypeCapabilities",         &GenericCommunicator::GetLayertypeCapabilities },
        { "GetLayerCapabilities",             &GenericCommunicator::GetLayerCapabilities },
        { "Exit",                             &GenericCommunicator::Exit },
        { "CommitChanges",                    &GenericCommunicator::CommitChanges },
        { "CreateShader",                     &GenericCommunicator::CreateShader },
        { "DestroyShader",                    &GenericCommunicator::DestroyShader },
        { "SetShader",                        &GenericCommunicator::SetShader },
        { "SetUniforms",                      &GenericCommunicator::SetUniforms },
        { "SetKeyboardFocusOn",               &GenericCommunicator::SetKeyboardFocusOn },
        { "GetKeyboardFocusSurfaceId",        &GenericCommunicator::GetKeyboardFocusSurfaceId },
        { "UpdateInputEventAcceptanceOn",     &GenericCommunicator::UpdateInputEventAcceptanceOn }
    };

    int entryCount = sizeof(manager_methods) / sizeof(MethodTable);

    for (int index = 0; index < entryCount; ++index)
    {
        MethodTable* method = &manager_methods[index];
        if (method->function)
        {
            m_callBackTable[method->name] = *method;
            LOG_DEBUG("GenericCommunicator", "registered callback for " << method->name);
        }
    }
}

bool GenericCommunicator::start()
{
    LOG_DEBUG("GenericCommunicator", "Starting up IpcModules.");

    if (!loadIpcModule(&m_ipcModule))
    {
        LOG_ERROR("GenericCommunicator", "Loading IpcModule failed.");
        return ILM_FALSE;
    }
    LOG_DEBUG("GenericCommunicator", "Loading IpcModule success.");

    if (!m_ipcModule.init(ILM_FALSE))
    {
        LOG_ERROR("GenericCommunicator", "Initializing IpcModule failed.");
        return ILM_FALSE;
    }
    LOG_DEBUG("GenericCommunicator", "Initializing IpcModule success.");

    m_running = ILM_TRUE;

    return ILM_TRUE;
}

void GenericCommunicator::stop()
{
    LOG_INFO("GenericCommunicator","stopping");

    if (m_running)
    {
        m_ipcModule.destroy();
    }
}

void GenericCommunicator::process(int timeout_ms)
{
    enum IpcMessageType messageType = m_ipcModule.receiveMessage(timeout_ms);

    t_ilm_const_string name = m_ipcModule.getMessageName();
    t_ilm_const_string sender = m_ipcModule.getSenderName();

    switch(messageType)
    {
    case IpcMessageTypeCommand:
        if (m_callBackTable.end() != m_callBackTable.find(name))
        {
            LOG_DEBUG("GenericCommunicator", "Received command " << name << " from " << sender);
            CallBackMethod method = m_callBackTable[name].function;
            (this->*method)();
        }
        else
        {
            LOG_WARNING("GenericCommunicator", "Received unknown command " << name << " from " << sender);
        }
        break;

    case IpcMessageTypeDisconnect:
        LOG_DEBUG("GenericCommunicator", "Received message " << name
                  << " (type=disconnect" << ", sender=" << sender << ")");
        ServiceDisconnect();
        break;

    case IpcMessageTypeNotification:
        LOG_DEBUG("GenericCommunicator", "Received message " << name
                  << " (type=notification" << ", sender=" << sender << ")");
        break;

    case IpcMessageTypeError:
        LOG_DEBUG("GenericCommunicator", "Received error message " << name << " from " << sender);
        break;

    case IpcMessageTypeNone:
        break;

    default:
        LOG_DEBUG("GenericCommunicator", "Received unknown data from " << sender);
        break;
    }
}

void GenericCommunicator::setdebug(bool onoff)
{
    (void)onoff; // TODO: remove, only prevents warning
}

void GenericCommunicator::ServiceConnect()
{
    LOG_DEBUG("GenericCommunicator", "ServiceConnect called");
    u_int32_t processId = 0;
    m_ipcModule.getUint(&processId);
    char* owner = strdup(m_ipcModule.getSenderName());
    m_executor->addApplicationReference(new IApplicationReference(owner,processId));
    m_ipcModule.createMessage((char*)__FUNCTION__);
    m_ipcModule.sendMessage();
}

void GenericCommunicator::ServiceDisconnect()
{
    LOG_DEBUG("GenericCommunicator", "ServiceDisconnect called");
    char* owner = strdup(m_ipcModule.getSenderName());
    long int ownerHash = IApplicationReference::generateApplicationHash(owner);
    ApplicationReferenceMap* refmap = m_executor->getApplicationReferenceMap();

    ApplicationReferenceMapIterator iter = refmap->find(ownerHash);
    ApplicationReferenceMapIterator iterEnd = refmap->end();

    for (; iter != iterEnd; ++iter)
    {
        IApplicationReference* registeredApp = (*iter).second;
        m_executor->removeApplicationReference(registeredApp);
    }

    m_ipcModule.createMessage((char*)__FUNCTION__);
    m_ipcModule.sendMessage();
}

void GenericCommunicator::Debug()
{
    t_ilm_bool param = ILM_FALSE;
    m_ipcModule.getBool(&param);

    t_ilm_bool status = m_executor->execute(new DebugCommand(param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(INVALID_ARGUMENT);
    }
}

void GenericCommunicator::GetScreenResolution()
{
    uint screenid = 0;
    m_ipcModule.getUint(&screenid);
    uint* resolution = m_executor->getScreenResolution(screenid);
    m_ipcModule.createMessage((char*)__FUNCTION__);
    m_ipcModule.appendUint(resolution[0]);
    m_ipcModule.appendUint(resolution[1]);
    m_ipcModule.sendMessage();
}

void GenericCommunicator::GetNumberOfHardwareLayers()
{
    uint screenid = 0;
    m_ipcModule.getUint(&screenid);
    uint numberOfHardwareLayers = m_executor->getNumberOfHardwareLayers(screenid);
    m_ipcModule.createMessage((char*)__FUNCTION__);
    m_ipcModule.appendUint(numberOfHardwareLayers);
    m_ipcModule.sendMessage();
}

void GenericCommunicator::GetScreenIDs()
{
    uint length = 0;
    uint* IDs = m_executor->getScreenIDs(&length);
    m_ipcModule.createMessage((char*)__FUNCTION__);
    m_ipcModule.appendUintArray(IDs, length);
    m_ipcModule.sendMessage();
}

void GenericCommunicator::ScreenShot()
{
    uint screenid = 0;
    m_ipcModule.getUint(&screenid);
    char filename[1024];
    m_ipcModule.getString(filename);

    t_ilm_bool status = m_executor->execute(new ScreenDumpCommand(filename, screenid));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(INVALID_ARGUMENT);
    }
}

void GenericCommunicator::ScreenShotOfLayer()
{
    char filename[1024];
    m_ipcModule.getString(filename);
    uint layerid = 0;
    m_ipcModule.getUint(&layerid);

    t_ilm_bool status = m_executor->execute(new LayerDumpCommand(filename, layerid));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::ScreenShotOfSurface()
{
    char filename[1024];
    m_ipcModule.getString(filename);
    uint id = 0;
    m_ipcModule.getUint(&id);
    t_ilm_bool status = m_executor->execute(new SurfaceDumpCommand(filename, id));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::ListAllLayerIDS()
{
    uint* array = NULL;
    uint length = 0;
    m_executor->getScene()->lockScene();
    m_executor->getScene()->getLayerIDs(&length, &array);
    m_ipcModule.createMessage((char*)__FUNCTION__);
    m_ipcModule.appendUintArray(array, length);
    m_ipcModule.sendMessage();
    m_executor->getScene()->unlockScene();
}

void GenericCommunicator::ListAllLayerIDsOnScreen()
{
    uint screenID = 0;
    m_ipcModule.getUint(&screenID);

    uint* array = NULL;
    uint length = 0;
    m_executor->getScene()->lockScene();
    t_ilm_bool status = m_executor->getScene()->getLayerIDsOfScreen(screenID, &length, &array);
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUintArray(array, length);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(INVALID_ARGUMENT);
    }
    m_executor->getScene()->unlockScene();
}

void GenericCommunicator::ListAllSurfaceIDS()
{
    uint* array = NULL;
    uint length = 0;
    m_executor->getScene()->lockScene();
    m_executor->getScene()->getSurfaceIDs(&length, &array);
    m_ipcModule.createMessage((char*)__FUNCTION__);
    m_ipcModule.appendUintArray(array, length);
    m_ipcModule.sendMessage();
    m_executor->getScene()->unlockScene();
}

void GenericCommunicator::ListAllLayerGroupIDS()
{
    uint* array = NULL;
    uint length = 0;
    m_executor->getScene()->lockScene();
    m_executor->getScene()->getLayerGroupIDs(&length, &array);
    m_ipcModule.createMessage((char*)__FUNCTION__);
    m_ipcModule.appendUintArray(array, length);
    m_ipcModule.sendMessage();
    m_executor->getScene()->unlockScene();
}

void GenericCommunicator::ListAllSurfaceGroupIDS()
{
    uint* array = NULL;
    uint length = 0;
    m_executor->getScene()->lockScene();
    m_executor->getScene()->getSurfaceGroupIDs(&length, &array);
    m_ipcModule.createMessage((char*)__FUNCTION__);
    m_ipcModule.appendUintArray(array, length);
    m_ipcModule.sendMessage();
    m_executor->getScene()->unlockScene();
}

void GenericCommunicator::ListSurfacesOfSurfacegroup()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    m_executor->getScene()->lockScene();
    SurfaceGroup* sg = m_executor->getScene()->getSurfaceGroup(id);
    if (NULL != sg)
    {
        std::list<Surface*> surfaces = sg->getList();
        uint length = surfaces.size();
        uint* array = new uint[length];
        uint arrayPos = 0;
        m_ipcModule.createMessage((char*)__FUNCTION__);

        for (std::list<Surface*>::const_iterator it = surfaces.begin(); it != surfaces.end(); ++it)
        {
            Surface* s = *it;
            array[arrayPos] = s->getID();
            ++arrayPos;
        }

        m_ipcModule.appendUintArray(array, length);

        m_ipcModule.sendMessage();
        m_executor->getScene()->unlockScene();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::ListLayersOfLayergroup()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    m_executor->getScene()->lockScene();
    LayerGroup* sg = m_executor->getScene()->getLayerGroup(id);
    if (NULL != sg)
    {
        std::list<Layer*> layers = sg->getList();

        uint length = layers.size();
        uint* array = new uint[length];
        uint arrayPos = 0;

        m_ipcModule.createMessage((char*)__FUNCTION__);
        for (std::list<Layer*>::const_iterator it = layers.begin(); it != layers.end(); ++it)
        {
            Layer* l = *it;
            array[arrayPos] = l->getID();
            ++arrayPos;
        }

        m_ipcModule.appendUintArray(array, length);

        m_ipcModule.sendMessage();
        m_executor->getScene()->unlockScene();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::ListSurfaceofLayer()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    m_executor->getScene()->lockScene();
    Layer* layer = m_executor->getScene()->getLayer(id);
    if (layer != NULL)
    {
        std::list<Surface*> surfaces = layer->getAllSurfaces();

        uint length = surfaces.size();
        uint* array = new uint[length];
        uint arrayPos = 0;

        m_ipcModule.createMessage((char*)__FUNCTION__);

        for (std::list<Surface*>::const_iterator it = surfaces.begin(); it != surfaces.end(); ++it)
        {
            Surface* s = *it;
            array[arrayPos] = s->getID();
            ++arrayPos;
        }

        m_ipcModule.appendUintArray(array, length);

        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
    m_executor->getScene()->unlockScene();
}

void GenericCommunicator::GetPropertiesOfSurface()
{
    uint id = 0;
    m_ipcModule.getUint(&id);

    Surface* surface = m_executor->getScene()->getSurface(id);
    if (surface != NULL)
    {
        Rectangle dest = surface->getDestinationRegion();
        Rectangle src = surface->getSourceRegion();
        OrientationType orientation = surface->getOrientation();

        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendDouble(surface->getOpacity());
        m_ipcModule.appendUint(src.x);
        m_ipcModule.appendUint(src.y);
        m_ipcModule.appendUint(src.width);
        m_ipcModule.appendUint(src.height);
        m_ipcModule.appendUint(surface->OriginalSourceWidth);
        m_ipcModule.appendUint(surface->OriginalSourceHeight);
        m_ipcModule.appendUint(dest.x);
        m_ipcModule.appendUint(dest.y);
        m_ipcModule.appendUint(dest.width);
        m_ipcModule.appendUint(dest.height);
        m_ipcModule.appendUint(orientation);
        m_ipcModule.appendBool(surface->getVisibility());
        m_ipcModule.appendUint(surface->frameCounter);
        m_ipcModule.appendUint(surface->drawCounter);
        m_ipcModule.appendUint(surface->updateCounter);
        m_ipcModule.appendUint(surface->getPixelFormat());
        m_ipcModule.appendUint(surface->getNativeContent());
        m_ipcModule.appendUint(surface->getInputEventAcceptanceOnDevices());
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetPropertiesOfLayer()
{
    uint id = 0;
    m_ipcModule.getUint(&id);

    Layer* layer = m_executor->getScene()->getLayer(id);
    if (layer != NULL)
    {
        Rectangle dest = layer->getDestinationRegion();
        Rectangle src = layer->getSourceRegion();
        OrientationType orientation = layer->getOrientation();

        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendDouble(layer->getOpacity());
        m_ipcModule.appendUint(src.x);
        m_ipcModule.appendUint(src.y);
        m_ipcModule.appendUint(src.width);
        m_ipcModule.appendUint(src.height);
        m_ipcModule.appendUint(layer->OriginalSourceWidth);
        m_ipcModule.appendUint(layer->OriginalSourceHeight);
        m_ipcModule.appendUint(dest.x);
        m_ipcModule.appendUint(dest.y);
        m_ipcModule.appendUint(dest.width);
        m_ipcModule.appendUint(dest.height);
        m_ipcModule.appendUint(orientation);
        m_ipcModule.appendBool(layer->getVisibility());
        m_ipcModule.appendUint(layer->getLayerType());
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::CreateSurface()
{
    uint handle = 0;
    m_ipcModule.getUint(&handle);
    uint width = 0;
    m_ipcModule.getUint(&width);
    uint height = 0;
    m_ipcModule.getUint(&height);
    uint pixelformat = 0;
    m_ipcModule.getUint(&pixelformat);
    PixelFormat pf = (PixelFormat) pixelformat;

    //LOG_DEBUG("GenericCommunicator::CreateSurface","pixelformat: " << pixelformat);
    uint id = GraphicalObject::INVALID_ID;
    /* First of all create the surface */
    t_ilm_bool status = m_executor->execute(new SurfaceCreateCommand(&id));
    /* after that apply the native content */
    status &= m_executor->execute(new SurfaceSetNativeContentCommand(id, handle, pf, width, height));    
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(id);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::CreateSurfaceFromId()
{
    uint handle = 0;
    m_ipcModule.getUint(&handle);
    uint width = 0;
    m_ipcModule.getUint(&width);
    uint height = 0;
    m_ipcModule.getUint(&height);
    uint pixelformat = 0;
    m_ipcModule.getUint(&pixelformat);
    PixelFormat pf = (PixelFormat) pixelformat;

    //LOG_DEBUG("GenericCommunicator::CreateSurface","pixelformat: " << pixelformat);
    uint id = 0;
    m_ipcModule.getUint(&id);

    // First of all create the surface
    t_ilm_bool status = m_executor->execute(new SurfaceCreateCommand(&id));
    // after that apply the native content
    status &= m_executor->execute(new SurfaceSetNativeContentCommand(id, handle, pf, width, height));    

    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(id);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::InitializeSurface()
{
    uint id = GraphicalObject::INVALID_ID;

    t_ilm_bool status = m_executor->execute(new SurfaceCreateCommand(&id));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(id);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::InitializeSurfaceFromId()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    t_ilm_bool status = m_executor->execute(new SurfaceCreateCommand(&id));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(id);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::SetSurfaceNativeContent()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint handle = 0;
    m_ipcModule.getUint(&handle);
    uint width = 0;
    m_ipcModule.getUint(&width);
    uint height = 0;
    m_ipcModule.getUint(&height);
    uint pixelformat = 0;
    m_ipcModule.getUint(&pixelformat);
    PixelFormat pf = (PixelFormat) pixelformat;

    t_ilm_bool status = m_executor->execute(new SurfaceSetNativeContentCommand(id, handle, pf, width, height));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::RemoveSurfaceNativeContent()
{
    uint id = 0;
    m_ipcModule.getUint(&id);

    t_ilm_bool status = m_executor->execute(new SurfaceRemoveNativeContentCommand(id));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::RemoveSurface()
{
    uint param = 0;
    m_ipcModule.getUint(&param);
    t_ilm_bool status = m_executor->execute(new SurfaceRemoveCommand(param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::CreateLayer()
{
    uint id = GraphicalObject::INVALID_ID;
    // use resolution of default screen as default width and height of layers
    uint* resolution = m_executor->getScreenResolution(DEFAULT_SCREEN);
    t_ilm_bool status = m_executor->execute(new LayerCreateCommand(resolution[0], resolution[1], &id));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(id);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::CreateLayerFromId()
{
    uint id = GraphicalObject::INVALID_ID;

    m_ipcModule.getUint(&id);
    // use resolution of default screen as default width and height of layers
    uint* resolution = m_executor->getScreenResolution(DEFAULT_SCREEN);
    t_ilm_bool status = m_executor->execute(new LayerCreateCommand(resolution[0], resolution[1], &id));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(id);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}


void GenericCommunicator::CreateLayerWithDimension()
{
    uint width = 0;
    m_ipcModule.getUint(&width);
    uint height = 0;
    m_ipcModule.getUint(&height);

    uint id = GraphicalObject::INVALID_ID;
    t_ilm_bool status = m_executor->execute(new LayerCreateCommand(width, height, &id));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(id);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::CreateLayerFromIdWithDimension()
{
    uint id = GraphicalObject::INVALID_ID;

    m_ipcModule.getUint(&id);
    uint width = 0;
    m_ipcModule.getUint(&width);
    uint height = 0;
    m_ipcModule.getUint(&height);
    t_ilm_bool status = m_executor->execute(new LayerCreateCommand(width, height, &id));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(id);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::RemoveLayer()
{
    uint param = 0;
    m_ipcModule.getUint(&param);
    t_ilm_bool status = m_executor->execute(new LayerRemoveCommand(param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::AddSurfaceToSurfaceGroup()
{
    uint surfaceid = 0;
    m_ipcModule.getUint(&surfaceid);
    uint surfacegroupid = 0;
    m_ipcModule.getUint(&surfacegroupid);

    t_ilm_bool status = m_executor->execute(new SurfacegroupAddSurfaceCommand(surfacegroupid, surfaceid));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::RemoveSurfaceFromSurfaceGroup()
{
    uint surfaceid = 0;
    m_ipcModule.getUint(&surfaceid);
    uint surfacegroupid = 0;
    m_ipcModule.getUint(&surfacegroupid);

    t_ilm_bool status = m_executor->execute(new SurfacegroupRemoveSurfaceCommand(surfacegroupid, surfaceid));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::AddLayerToLayerGroup()
{
    uint layerid = 0;
    m_ipcModule.getUint(&layerid);
    uint layergroupid = 0;
    m_ipcModule.getUint(&layergroupid);

    t_ilm_bool status = m_executor->execute(new LayergroupAddLayerCommand(layergroupid, layerid));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::RemoveLayerFromLayerGroup()
{
    uint layerid = 0;
    m_ipcModule.getUint(&layerid);
    uint layergroupid = 0;
    m_ipcModule.getUint(&layergroupid);

    t_ilm_bool status = m_executor->execute(new LayergroupRemoveLayerCommand(layergroupid, layerid));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::AddSurfaceToLayer()
{
    uint surfaceid = 0;
    m_ipcModule.getUint(&surfaceid);
    uint layer = 0;
    m_ipcModule.getUint(&layer);

    t_ilm_bool status = m_executor->execute(new LayerAddSurfaceCommand(layer, surfaceid));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::RemoveSurfaceFromLayer()
{
    uint surfaceid = 0;
    m_ipcModule.getUint(&surfaceid);
    uint layerid = 0;
    m_ipcModule.getUint(&layerid);

    t_ilm_bool status = m_executor->execute(new LayerRemoveSurfaceCommand(layerid, surfaceid));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::CreateSurfaceGroup()
{
    uint newID = GraphicalObject::INVALID_ID;

    t_ilm_bool status = m_executor->execute(new SurfacegroupCreateCommand(&newID));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(newID);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::CreateSurfaceGroupFromId()
{
    uint newID = GraphicalObject::INVALID_ID;

    m_ipcModule.getUint(&newID);

    t_ilm_bool status = m_executor->execute(new SurfacegroupCreateCommand(&newID));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(newID);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::RemoveSurfaceGroup()
{
    uint param = 0;
    m_ipcModule.getUint(&param);

    t_ilm_bool status = m_executor->execute(new SurfacegroupRemoveCommand(param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::CreateLayerGroup()
{
    uint newID = GraphicalObject::INVALID_ID;

    t_ilm_bool status = m_executor->execute(new LayergroupCreateCommand(&newID));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(newID);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::CreateLayerGroupFromId()
{
    uint newID = GraphicalObject::INVALID_ID;

    m_ipcModule.getUint(&newID);

    t_ilm_bool status = m_executor->execute(new LayergroupCreateCommand(&newID));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(newID);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_ALREADY_INUSE);
    }
}

void GenericCommunicator::RemoveLayerGroup()
{
    uint param = 0;
    m_ipcModule.getUint(&param);

    t_ilm_bool status = m_executor->execute(new LayergroupRemoveCommand(param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetSurfaceSourceRegion()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint x = 0;
    m_ipcModule.getUint(&x);
    uint y = 0;
    m_ipcModule.getUint(&y);
    uint width = 0;
    m_ipcModule.getUint(&width);
    uint height = 0;
    m_ipcModule.getUint(&height);

    t_ilm_bool status = m_executor->execute(new SurfaceSetSourceRectangleCommand(id, x, y, width, height));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetLayerSourceRegion()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint x = 0;
    m_ipcModule.getUint(&x);
    uint y = 0;
    m_ipcModule.getUint(&y);
    uint width = 0;
    m_ipcModule.getUint(&width);
    uint height = 0;
    m_ipcModule.getUint(&height);

    //LOG_DEBUG("DBUSC","new SetSourceRectangleCommand with arguments: " <<id <<" " << x <<" "<< y <<" "<< width <<" "<< height );
    t_ilm_bool status = m_executor->execute(new LayerSetSourceRectangleCommand(id, x, y, width, height));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetSurfaceDestinationRegion()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint x = 0;
    m_ipcModule.getUint(&x);
    uint y = 0;
    m_ipcModule.getUint(&y);
    uint width = 0;
    m_ipcModule.getUint(&width);
    uint height = 0;
    m_ipcModule.getUint(&height);

    t_ilm_bool status = m_executor->execute(new SurfaceSetDestinationRectangleCommand(id, x, y, width, height));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetSurfacePosition()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint x = 0;
    m_ipcModule.getUint(&x);
    uint y = 0;
    m_ipcModule.getUint(&y);

    t_ilm_bool status = m_executor->execute(new SurfaceSetPositionCommand(id, x, y));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetSurfacePosition()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint x = 0;
    uint y = 0;

    t_ilm_bool status = m_executor->execute(new SurfaceGetPositionCommand(id, &x, &y));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(x);
        m_ipcModule.appendUint(y);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetSurfaceDimension()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint width = 0;
    m_ipcModule.getUint(&width);
    uint height = 0;
    m_ipcModule.getUint(&height);

    t_ilm_bool status = m_executor->execute(new SurfaceSetDimensionCommand(id, width, height));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetLayerDestinationRegion()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint x = 0;
    m_ipcModule.getUint(&x);
    uint y = 0;
    m_ipcModule.getUint(&y);
    uint width = 0;
    m_ipcModule.getUint(&width);
    uint height = 0;
    m_ipcModule.getUint(&height);

    t_ilm_bool status = m_executor->execute(new LayerSetDestinationRectangleCommand(id, x, y, width, height));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetLayerPosition()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint x = 0;
    m_ipcModule.getUint(&x);
    uint y = 0;
    m_ipcModule.getUint(&y);

    t_ilm_bool status = m_executor->execute(new LayerSetPositionCommand(id, x, y));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetLayerPosition()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint x = 0;
    uint y = 0;

    t_ilm_bool status = m_executor->execute(new LayerGetPositionCommand(id, &x, &y));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(x);
        m_ipcModule.appendUint(y);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetLayerDimension()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint width = 0;
    m_ipcModule.getUint(&width);
    uint height = 0;
    m_ipcModule.getUint(&height);

    t_ilm_bool status = m_executor->execute(new LayerSetDimensionCommand(id, width, height));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetLayerDimension()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint width = 0;
    uint height = 0;

    t_ilm_bool status = m_executor->execute(new LayerGetDimensionCommand(id, &width, &height));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(width);
        m_ipcModule.appendUint(height);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetSurfaceDimension()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint width = 0;
    uint height = 0;

    t_ilm_bool status = m_executor->execute(new SurfaceGetDimensionCommand(id, &width, &height));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(width);
        m_ipcModule.appendUint(height);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetSurfaceOpacity()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    double param = 0.0;
    m_ipcModule.getDouble(&param);

    t_ilm_bool status = m_executor->execute(new SurfaceSetOpacityCommand(id, param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetLayerOpacity()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    double param = 0.0;
    m_ipcModule.getDouble(&param);

    t_ilm_bool status = m_executor->execute(new LayerSetOpacityCommand(id, param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetSurfacegroupOpacity()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    double param = 0.0;
    m_ipcModule.getDouble(&param);

    t_ilm_bool status = m_executor->execute(new SurfacegroupSetOpacityCommand(id, param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetLayergroupOpacity()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    double param = 0.0;
    m_ipcModule.getDouble(&param);

    t_ilm_bool status = m_executor->execute(new LayergroupSetOpacityCommand(id, param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetSurfaceOpacity()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    double param = 0.0;

    t_ilm_bool status = m_executor->execute(new SurfaceGetOpacityCommand(id, &param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendDouble(param);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetLayerOpacity()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    double param = 0.0;

    t_ilm_bool status = m_executor->execute(new LayerGetOpacityCommand(id, &param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendDouble(param);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetSurfaceOrientation()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint param = 0;
    m_ipcModule.getUint(&param);
    OrientationType o = (OrientationType) param;

    t_ilm_bool status = m_executor->execute(new SurfaceSetOrientationCommand(id, o));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetSurfaceOrientation()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    OrientationType o;

    t_ilm_bool status = m_executor->execute(new SurfaceGetOrientationCommand(id, &o));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(o);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetLayerOrientation()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    uint param = 0;
    m_ipcModule.getUint(&param);
    OrientationType o = (OrientationType) param;

    t_ilm_bool status = m_executor->execute(new LayerSetOrientationCommand(id, o));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetLayerOrientation()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    OrientationType o;

    t_ilm_bool status = m_executor->execute(new LayerGetOrientationCommand(id, &o));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(o);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetSurfacePixelformat()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    PixelFormat param;

    t_ilm_bool status = m_executor->execute(new SurfaceGetPixelformatCommand(id, &param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(param);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetSurfaceVisibility()
{
    uint surfaceid = 0;
    m_ipcModule.getUint(&surfaceid);
    t_ilm_bool newVis = ILM_FALSE;
    m_ipcModule.getBool(&newVis);

    t_ilm_bool status = m_executor->execute(new SurfaceSetVisibilityCommand(surfaceid, newVis));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetLayerVisibility()
{
    uint layerid = 0;
    m_ipcModule.getUint(&layerid);
    t_ilm_bool myparam = ILM_FALSE;
    m_ipcModule.getBool(&myparam);

    t_ilm_bool status = m_executor->execute(new LayerSetVisibilityCommand(layerid, myparam));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetSurfaceVisibility()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    bool param;

    t_ilm_bool status = m_executor->execute(new SurfaceGetVisibilityCommand(id, &param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        //LOG_DEBUG("GenericCommunicator", "returning surfacevisibility: " << param);
        m_ipcModule.appendBool(param);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetLayerVisibility()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    bool param;

    t_ilm_bool status = m_executor->execute(new LayerGetVisibilityCommand(id, &param));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendBool(param);
        //LOG_DEBUG("GenericCommunicator", "returning layervisibility: " << param);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetSurfacegroupVisibility()
{
    uint groupid = 0;
    m_ipcModule.getUint(&groupid);
    t_ilm_bool myparam = ILM_FALSE;
    m_ipcModule.getBool(&myparam);

    t_ilm_bool status = m_executor->execute(new SurfacegroupSetVisibilityCommand(groupid, myparam));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetLayergroupVisibility()
{
    uint groupid = 0;
    m_ipcModule.getUint(&groupid);
    t_ilm_bool myparam = ILM_FALSE;
    m_ipcModule.getBool(&myparam);

    t_ilm_bool status = m_executor->execute(new LayergroupSetVisibilityCommand(groupid, myparam));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}


void GenericCommunicator::SetRenderOrderOfLayers()
{
    uint* array = NULL;
    int length = 0;

    m_ipcModule.getUintArray(&array, &length);

    //LOG_DEBUG("GenericCommunicator","Renderorder: Got " << length << " ids.");

    t_ilm_bool status = m_executor->execute(new ScreenSetRenderOrderCommand(array, length));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetSurfaceRenderOrderWithinLayer()
{
    uint* array = NULL;
    int length = 0;

    uint layerid = 0;
    m_ipcModule.getUint(&layerid);

    m_ipcModule.getUintArray(&array, &length);

    t_ilm_bool status = m_executor->execute(new LayerSetRenderOrderCommand(layerid, array, length));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetLayerType()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    Layer* l = m_executor->getScene()->getLayer(id);
    if (l != NULL)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(l->getLayerType());
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::GetLayertypeCapabilities()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    LayerType type = (LayerType) id;
    uint capabilities = m_executor->getLayerTypeCapabilities(type);
    //LOG_DEBUG("GenericCommunicator", "GetLayertypeCapabilities: returning capabilities:" << capabilities);
    m_ipcModule.createMessage((char*)__FUNCTION__);
    m_ipcModule.appendUint(capabilities);
    m_ipcModule.sendMessage();
}

void GenericCommunicator::GetLayerCapabilities()
{
    uint id = 0;
    m_ipcModule.getUint(&id);
    Layer* l = m_executor->getScene()->getLayer(id);
    if (l != NULL)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(l->getCapabilities());
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::FadeIn()
{
    m_ipcModule.createMessage((char*)__FUNCTION__);
    m_ipcModule.sendMessage();
}

void GenericCommunicator::SynchronizedFade()
{
    m_ipcModule.createMessage((char*)__FUNCTION__);
    m_ipcModule.sendMessage();
}

void GenericCommunicator::FadeOut()
{
    m_ipcModule.createMessage((char*)__FUNCTION__);
    m_ipcModule.sendMessage();
}

void GenericCommunicator::Exit()
{
    t_ilm_bool status = m_executor->execute(new ExitCommand());
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(INVALID_ARGUMENT);
    }
}

void GenericCommunicator::CommitChanges()
{
    t_ilm_bool status = m_executor->execute(new CommitCommand());
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(INVALID_ARGUMENT);
    }
}

void GenericCommunicator::CreateShader()
{
    char vertname[1024];
    char fragname[1024];
    m_ipcModule.getString(vertname);
    m_ipcModule.getString(fragname);
    uint id = 0;

    t_ilm_bool status = m_executor->execute(new ShaderCreateCommand(vertname, fragname, &id));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(id);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::DestroyShader()
{
    uint shaderid = 0;
    m_ipcModule.getUint(&shaderid);

    t_ilm_bool status = m_executor->execute(new ShaderDestroyCommand(shaderid));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetShader()
{
    uint surfaceId = 0;
    uint shaderid = 0;
    m_ipcModule.getUint(&surfaceId);
    m_ipcModule.getUint(&shaderid);

    t_ilm_bool status = m_executor->execute(new SurfaceSetShaderCommand(surfaceId, shaderid));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetUniforms()
{
    uint id = 0;
    m_ipcModule.getUint(&id);

    std::vector<string> uniforms;

    // TODO
    //m_ipcModule.getStringArray(&uniforms);

    t_ilm_bool status = m_executor->execute(new ShaderSetUniformsCommand(id, uniforms));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

void GenericCommunicator::SetKeyboardFocusOn()
{
    uint surfaceId = 0;

    m_ipcModule.getUint(&surfaceId);

    t_ilm_bool status = m_executor->execute(new SurfaceSetKeyboardFocusCommand(surfaceId));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}


void GenericCommunicator::GetKeyboardFocusSurfaceId()
{
    uint surfaceId;
    
    t_ilm_bool status = m_executor->execute(new SurfaceGetKeyboardFocusCommand(&surfaceId));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.appendUint(surfaceId);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
    
    
}


void GenericCommunicator::UpdateInputEventAcceptanceOn()
{
    uint surfaceId = 0;
        uint udevices = 0;
        InputDevice devices;
        t_ilm_bool accept;

    m_ipcModule.getUint(&surfaceId);
    m_ipcModule.getUint(&udevices);
    m_ipcModule.getBool(&accept);

    devices = (InputDevice) udevices;
    t_ilm_bool status = m_executor->execute(new SurfaceUpdateInputEventAcceptance(surfaceId, devices, accept));
    if (status)
    {
        m_ipcModule.createMessage((char*)__FUNCTION__);
        m_ipcModule.sendMessage();
    }
    else
    {
        m_ipcModule.sendError(RESSOURCE_NOT_FOUND);
    }
}

extern "C" ICommunicator* createGenericCommunicator(ICommandExecutor* executor)
{
    return new GenericCommunicator(executor);
}

extern "C" void destroyGenericCommunicator(GenericCommunicator* p)
{
    delete p;
}

