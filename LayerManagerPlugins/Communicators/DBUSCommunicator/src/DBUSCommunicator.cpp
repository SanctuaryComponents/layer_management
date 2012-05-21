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
#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "DBUSCommunicator.h"
#include "DBUSIntrospection.h"
#include "DBUSMessageHandler.h"
#include "DBUSConfiguration.h"

#include "Log.h"

#define DEFAULT_SCREEN 0

const char* SERVICE_ERROR = "org.genivi.layermanagementservice.error";
const char* RESSOURCE_ALREADY_INUSE = "Ressource already in use";
const char* INVALID_ARGUMENT = "Invalid argument";
const char* RESSOURCE_NOT_FOUND = "Ressource not found";



static DBUSMessageHandler* g_pDbusMessage;

static MethodTable manager_methods[] =
{
    { "ServiceConnect",                   "u",     "",            &DBUSCommunicator::ServiceConnect },
    { "ServiceDisconnect",                "u",     "",            &DBUSCommunicator::ServiceDisconnect },    
    { "Debug",                            "b",     "",            &DBUSCommunicator::Debug },
    { "ScreenShot",                       "us",    "",            &DBUSCommunicator::ScreenShot },
    { "ScreenShotOfLayer",                "su",    "",            &DBUSCommunicator::ScreenShotOfLayer },
    { "ScreenShotOfSurface",              "su",    "",            &DBUSCommunicator::ScreenShotOfSurface },
    { "GetScreenResolution",              "u",     "uu",          &DBUSCommunicator::GetScreenResolution },
    { "GetNumberOfHardwareLayers",        "u",     "u",           &DBUSCommunicator::GetNumberOfHardwareLayers },
    { "GetScreenIDs",                     "",      "au",          &DBUSCommunicator::GetScreenIDs },
    { "ListAllLayerIDS",                  "",      "au",          &DBUSCommunicator::ListAllLayerIDS },
    { "ListAllLayerIDsOnScreen",          "u",     "au",          &DBUSCommunicator::ListAllLayerIDsOnScreen },
    { "ListAllSurfaceIDS",                "",      "au",          &DBUSCommunicator::ListAllSurfaceIDS },
    { "ListAllLayerGroupIDS",             "",      "au",          &DBUSCommunicator::ListAllLayerGroupIDS },
    { "ListAllSurfaceGroupIDS",           "",      "au",          &DBUSCommunicator::ListAllSurfaceGroupIDS },
    { "ListSurfacesOfSurfacegroup",       "u",     "au",          &DBUSCommunicator::ListSurfacesOfSurfacegroup },
    { "ListLayersOfLayergroup",           "u",     "au",          &DBUSCommunicator::ListLayersOfLayergroup },
    { "ListSurfaceofLayer",               "u",     "au",          &DBUSCommunicator::ListSurfaceofLayer },
    { "GetPropertiesOfSurface",           "u",     "duuuuuuuuuuybu", &DBUSCommunicator::GetPropertiesOfSurface },
    { "GetPropertiesOfLayer",             "u",     "duuuuuuuuuuyb", &DBUSCommunicator::GetPropertiesOfLayer },
    { "CreateSurface",                    "uuuu",  "u",           &DBUSCommunicator::CreateSurface },
    { "CreateSurfaceFromId",              "uuuuu", "u",           &DBUSCommunicator::CreateSurfaceFromId },    
    { "InitializeSurface",                "",      "u",           &DBUSCommunicator::InitializeSurface },
    { "InitializeSurfaceFromId",          "u",     "u",           &DBUSCommunicator::InitializeSurfaceFromId },
    { "SetSurfaceNativeContent",          "uuuuu", "",            &DBUSCommunicator::SetSurfaceNativeContent },
    { "RemoveSurface",                    "u",     "",            &DBUSCommunicator::RemoveSurface },
    { "CreateLayer",                      "",      "u",           &DBUSCommunicator::CreateLayer },
    { "CreateLayerFromId",                "u",     "u",           &DBUSCommunicator::CreateLayerFromId },
    { "CreateLayerWithDimension",         "uu",    "u",           &DBUSCommunicator::CreateLayerWithDimension },
    { "CreateLayerFromIdWithDimension",   "uuu",   "u",           &DBUSCommunicator::CreateLayerFromIdWithDimension },
    { "RemoveLayer",                      "u",     "",            &DBUSCommunicator::RemoveLayer },
    { "AddSurfaceToSurfaceGroup",         "uu",    "",            &DBUSCommunicator::AddSurfaceToSurfaceGroup },
    { "RemoveSurfaceFromSurfaceGroup",    "uu",    "",            &DBUSCommunicator::RemoveSurfaceFromSurfaceGroup },
    { "AddLayerToLayerGroup",             "uu",    "",            &DBUSCommunicator::AddLayerToLayerGroup },
    { "RemoveLayerFromLayerGroup",        "uu",    "",            &DBUSCommunicator::RemoveLayerFromLayerGroup },
    { "AddSurfaceToLayer",                "uu",    "",            &DBUSCommunicator::AddSurfaceToLayer },
    { "RemoveSurfaceFromLayer",           "uu",    "",            &DBUSCommunicator::RemoveSurfaceFromLayer },
    { "CreateSurfaceGroup",               "",      "u",           &DBUSCommunicator::CreateSurfaceGroup },
    { "CreateSurfaceGroupFromId",         "u",     "u",           &DBUSCommunicator::CreateSurfaceGroupFromId },
    { "RemoveSurfaceGroup",               "u",     "",            &DBUSCommunicator::RemoveSurfaceGroup },
    { "CreateLayerGroup",                 "",      "u",           &DBUSCommunicator::CreateLayerGroup },
    { "CreateLayerGroupFromId",           "u",     "u",           &DBUSCommunicator::CreateLayerGroupFromId },
    { "RemoveLayerGroup",                 "u",     "",            &DBUSCommunicator::RemoveLayerGroup },
    { "SetSurfaceSourceRegion",           "uuuuu", "",            &DBUSCommunicator::SetSurfaceSourceRegion },
    { "SetLayerSourceRegion",             "uuuuu", "",            &DBUSCommunicator::SetLayerSourceRegion },
    { "SetSurfaceDestinationRegion",      "uuuuu", "",            &DBUSCommunicator::SetSurfaceDestinationRegion },
    { "SetSurfacePosition",               "uuu",   "",            &DBUSCommunicator::SetSurfacePosition },
    { "GetSurfacePosition",               "u",     "uu",          &DBUSCommunicator::GetSurfacePosition },
    { "SetSurfaceDimension",              "uuu",   "",            &DBUSCommunicator::SetSurfaceDimension },
    { "SetLayerDestinationRegion",        "uuuuu", "",            &DBUSCommunicator::SetLayerDestinationRegion },
    { "SetLayerPosition",                 "uuu",   "",            &DBUSCommunicator::SetLayerPosition },
    { "GetLayerPosition",                 "u",     "uu",          &DBUSCommunicator::GetLayerPosition },
    { "SetLayerDimension",                "uuu",   "",            &DBUSCommunicator::SetLayerDimension },
    { "GetLayerDimension",                "u",     "uu",          &DBUSCommunicator::GetLayerDimension },
    { "GetSurfaceDimension",              "u",     "uu",          &DBUSCommunicator::GetSurfaceDimension },
    { "SetSurfaceOpacity",                "ud",    "",            &DBUSCommunicator::SetSurfaceOpacity },
    { "SetLayerOpacity",                  "ud",    "",            &DBUSCommunicator::SetLayerOpacity },
    { "SetSurfacegroupOpacity",           "ud",    "",            &DBUSCommunicator::SetSurfacegroupOpacity },
    { "SetLayergroupOpacity",             "ud",    "",            &DBUSCommunicator::SetLayergroupOpacity },
    { "GetSurfaceOpacity",                "u",     "d",           &DBUSCommunicator::GetSurfaceOpacity },
    { "GetLayerOpacity",                  "u",     "d",           &DBUSCommunicator::GetLayerOpacity },
    { "SetSurfaceOrientation",            "uu",    "",            &DBUSCommunicator::SetSurfaceOrientation },
    { "GetSurfaceOrientation",            "uu",    "",            &DBUSCommunicator::GetSurfaceOrientation },
    { "SetLayerOrientation",              "uu",    "",            &DBUSCommunicator::SetLayerOrientation },
    { "GetLayerOrientation",              "uu",    "",            &DBUSCommunicator::GetLayerOrientation },
    { "GetSurfacePixelformat",            "u",     "u",           &DBUSCommunicator::GetSurfacePixelformat },
    { "SetSurfaceVisibility",             "ub",    "",            &DBUSCommunicator::SetSurfaceVisibility },
    { "SetLayerVisibility",               "ub",    "",            &DBUSCommunicator::SetLayerVisibility },
    { "GetSurfaceVisibility",             "u",     "b",           &DBUSCommunicator::GetSurfaceVisibility },
    { "GetLayerVisibility",               "u",     "b",           &DBUSCommunicator::GetLayerVisibility },
    { "SetSurfacegroupVisibility",        "ub",    "",            &DBUSCommunicator::SetSurfacegroupVisibility },
    { "SetLayergroupVisibility",          "ub",    "",            &DBUSCommunicator::SetLayergroupVisibility },
    { "SetRenderOrderOfLayers",           "auu",   "",            &DBUSCommunicator::SetRenderOrderOfLayers },
    { "SetSurfaceRenderOrderWithinLayer", "uau",   "",            &DBUSCommunicator::SetSurfaceRenderOrderWithinLayer },
    { "GetLayerType",                     "u",     "u",           &DBUSCommunicator::GetLayerType },
    { "GetLayertypeCapabilities",         "u",     "u",           &DBUSCommunicator::GetLayertypeCapabilities },
    { "GetLayerCapabilities",             "u",     "u",           &DBUSCommunicator::GetLayerCapabilities },
    { "Exit",                             "",      "",            &DBUSCommunicator::Exit },
    { "CommitChanges",                    "",      "",            &DBUSCommunicator::CommitChanges },
    { "CreateShader",                     "ss",    "u",           &DBUSCommunicator::CreateShader },
    { "DestroyShader",                    "u",     "",            &DBUSCommunicator::DestroyShader },
    { "SetShader",                        "uu",    "",            &DBUSCommunicator::SetShader },
    { "SetUniforms",                      "uas",   "",            &DBUSCommunicator::SetUniforms },
    { "",                                 "",      "",            NULL }
};

#include <sstream>
using std::stringstream;

#include <string>
using std::string;

DBUSCommunicator::DBUSCommunicator(ICommandExecutor* executor)
: ICommunicator(executor)
, m_running(false)
{
}

void DBUSCommunicator::setdebug(bool onoff)
{
    (void)onoff; // TODO: remove, only prevents warning
}

bool DBUSCommunicator::start()
{
    LOG_DEBUG("DBUSCommunicator", "Starting up dbus connector");
    bool result = true;
    g_pDbusMessage = new DBUSMessageHandler();

    LOG_DEBUG("DBUSCommunicator","registering for dbus path " << DBUS_SERVICE_OBJECT_PATH);
    bool registered = g_pDbusMessage->registerPathFunction(DBUSCommunicator::processMessageFunc,
                                                           DBUSCommunicator::unregisterMessageFunc,
                                                           this);
    if (!registered)
    {
        
        LOG_ERROR("DBUSCommunicator","Register Message Callbacks failed");
        result = false;
    } 
    else 
    {
        registered =  g_pDbusMessage->registerMessageFilter(DBUSCommunicator::processMessageFunc,this);
        if (!registered)
        {
            
            LOG_ERROR("DBUSCommunicator","Register Message Filter failed");
            result = false;
        } 
    }
    LOG_INFO("DBUSCommunicator", "Started dbus connector");
    m_running = true;
    return result;
}

void DBUSCommunicator::stop()
{
    LOG_INFO("DBUSCommunicator","stopping");
    if (m_running) 
    {
        g_pDbusMessage->unregisterMessageFilter(DBUSCommunicator::processMessageFunc,this);
        // deregister dbus messaging implicitly by deleting messageHandler
    }
    if (g_pDbusMessage != NULL) 
    {
        delete g_pDbusMessage;
    }
}

void DBUSCommunicator::ServiceConnect(DBusConnection* conn, DBusMessage* msg)
{
    g_pDbusMessage->initReceive(msg);
    u_int32_t processId = g_pDbusMessage->getUInt();
    char* owner = strdup(dbus_message_get_sender(msg));   
    m_executor->addApplicationReference(new IApplicationReference(owner,processId));    
    AddClientWatch(conn,owner);
    g_pDbusMessage->initReply(msg);
    g_pDbusMessage->closeReply();
}

void DBUSCommunicator::ServiceDisconnect(DBusConnection* conn, DBusMessage* msg)   
{
   (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    char* owner = strdup(dbus_message_get_sender(msg));
    RemoveApplicationReference(owner);
    RemoveClientWatch(conn,owner);
    g_pDbusMessage->initReply(msg);
    g_pDbusMessage->closeReply();
}

void DBUSCommunicator::Debug(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    bool param = g_pDbusMessage->getBool();

    bool status = m_executor->execute(new DebugCommand(param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, INVALID_ARGUMENT);
    }
}

void DBUSCommunicator::GetScreenResolution(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint screenid = g_pDbusMessage->getUInt();
    uint* resolution = m_executor->getScreenResolution(screenid);
    g_pDbusMessage->initReply(msg);
    g_pDbusMessage->appendUInt(resolution[0]);
    g_pDbusMessage->appendUInt(resolution[1]);
    g_pDbusMessage->closeReply();
}

void DBUSCommunicator::GetNumberOfHardwareLayers(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint screenid = g_pDbusMessage->getUInt();
    uint numberOfHardwareLayers = m_executor->getNumberOfHardwareLayers(screenid);
    g_pDbusMessage->initReply(msg);
    g_pDbusMessage->appendUInt(numberOfHardwareLayers);
    g_pDbusMessage->closeReply();
}

void DBUSCommunicator::GetScreenIDs(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint length = 0;
    uint* IDs = m_executor->getScreenIDs(&length);
    g_pDbusMessage->initReply(msg);
    g_pDbusMessage->appendArrayOfUInt(length, IDs);
    g_pDbusMessage->closeReply();
}

void DBUSCommunicator::ScreenShot(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint screenid = g_pDbusMessage->getUInt();
    char* filename = g_pDbusMessage->getString();

    bool status = m_executor->execute(new ScreenDumpCommand(filename, screenid));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, INVALID_ARGUMENT);
    }
}
void DBUSCommunicator::process(int timeout_ms) 
{
    dbus_connection_read_write_dispatch (g_pDbusMessage->getConnection(), timeout_ms);
}
void DBUSCommunicator::ScreenShotOfLayer(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    char* filename = g_pDbusMessage->getString();
    uint layerid = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new LayerDumpCommand(filename, layerid));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::ScreenShotOfSurface(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    char* filename = g_pDbusMessage->getString();
    uint id = g_pDbusMessage->getUInt();
    bool status = m_executor->execute(new SurfaceDumpCommand(filename, id));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::ListAllLayerIDS(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    uint* array = NULL;
    uint length = 0;
    m_executor->getScene()->lockScene();
    m_executor->getScene()->getLayerIDs(&length, &array);
    g_pDbusMessage->initReply(msg);
    g_pDbusMessage->appendArrayOfUInt(length, array);
    g_pDbusMessage->closeReply();
    m_executor->getScene()->unlockScene();
}

void DBUSCommunicator::ListAllLayerIDsOnScreen(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint screenID = g_pDbusMessage->getUInt();

    uint* array = NULL;
    uint length = 0;
    m_executor->getScene()->lockScene();
    bool status = m_executor->getScene()->getLayerIDsOfScreen(screenID, &length, &array);
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendArrayOfUInt(length, array);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, INVALID_ARGUMENT);
    }
    m_executor->getScene()->unlockScene();

}

void DBUSCommunicator::ListAllSurfaceIDS(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    uint* array = NULL;
    uint length = 0;
    m_executor->getScene()->lockScene();
    m_executor->getScene()->getSurfaceIDs(&length, &array);
    g_pDbusMessage->initReply(msg);
    g_pDbusMessage->appendArrayOfUInt(length, array);
    g_pDbusMessage->closeReply();
    m_executor->getScene()->unlockScene();
}

void DBUSCommunicator::ListAllLayerGroupIDS(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    uint* array = NULL;
    uint length = 0;
    m_executor->getScene()->lockScene();
    m_executor->getScene()->getLayerGroupIDs(&length, &array);
    g_pDbusMessage->initReply(msg);
    g_pDbusMessage->appendArrayOfUInt(length, array);
    g_pDbusMessage->closeReply();
    m_executor->getScene()->unlockScene();
}

void DBUSCommunicator::ListAllSurfaceGroupIDS(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    uint* array = NULL;
    uint length = 0;
    m_executor->getScene()->lockScene();
    m_executor->getScene()->getSurfaceGroupIDs(&length, &array);
    g_pDbusMessage->initReply(msg);
    g_pDbusMessage->appendArrayOfUInt(length, array);
    g_pDbusMessage->closeReply();
    m_executor->getScene()->unlockScene();
}

void DBUSCommunicator::ListSurfacesOfSurfacegroup(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    m_executor->getScene()->lockScene();
    SurfaceGroup* sg = m_executor->getScene()->getSurfaceGroup(id);
    if (NULL != sg)
    {
        std::list<Surface*> surfaces = sg->getList();
        uint length = surfaces.size();
        uint* array = new uint[length];
        uint arrayPos = 0;
        g_pDbusMessage->initReply(msg);

        for (std::list<Surface*>::const_iterator it = surfaces.begin(); it != surfaces.end(); ++it)
        {
            Surface* s = *it;
            array[arrayPos] = s->getID();
            ++arrayPos;
        }

        g_pDbusMessage->appendArrayOfUInt(length, array);

        g_pDbusMessage->closeReply();
        m_executor->getScene()->unlockScene();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}
DBusHandlerResult DBUSCommunicator::processMessageFunc(DBusConnection* conn,DBusMessage* msg, void *user_data)
{
    return ((DBUSCommunicator*)user_data)->delegateMessage(conn,msg);
}

void DBUSCommunicator::unregisterMessageFunc(DBusConnection* conn, void *user_data)
{
    (void)conn;
    (void)user_data;
}

void DBUSCommunicator::ListLayersOfLayergroup(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    m_executor->getScene()->lockScene();
    LayerGroup* sg = m_executor->getScene()->getLayerGroup(id);
    if (NULL != sg)
    {
        std::list<Layer*> layers = sg->getList();

        uint length = layers.size();
        uint* array = new uint[length];
        uint arrayPos = 0;

        g_pDbusMessage->initReply(msg);
        for (std::list<Layer*>::const_iterator it = layers.begin(); it != layers.end(); ++it)
        {
            Layer* l = *it;
            array[arrayPos] = l->getID();
            ++arrayPos;
        }

        g_pDbusMessage->appendArrayOfUInt(length, array);

        g_pDbusMessage->closeReply();
        m_executor->getScene()->unlockScene();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::ListSurfaceofLayer(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    m_executor->getScene()->lockScene();
    Layer* layer = m_executor->getScene()->getLayer(id);
    if (layer != NULL)
    {
        std::list<Surface*> surfaces = layer->getAllSurfaces();

        uint length = surfaces.size();
        uint* array = new uint[length];
        uint arrayPos = 0;

        g_pDbusMessage->initReply(msg);

        for (std::list<Surface*>::const_iterator it = surfaces.begin(); it != surfaces.end(); ++it)
        {
            Surface* s = *it;
            array[arrayPos] = s->getID();
            ++arrayPos;
        }

        g_pDbusMessage->appendArrayOfUInt(length, array);

        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
    m_executor->getScene()->unlockScene();
}

void DBUSCommunicator::GetPropertiesOfSurface(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();

    Surface* surface = m_executor->getScene()->getSurface(id);
    if (surface != NULL)
    {
        Rectangle dest = surface->getDestinationRegion();
        Rectangle src = surface->getSourceRegion();
        OrientationType orientation = surface->getOrientation();

        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendDouble(surface->getOpacity());
        g_pDbusMessage->appendUInt(src.x);
        g_pDbusMessage->appendUInt(src.y);
        g_pDbusMessage->appendUInt(src.width);
        g_pDbusMessage->appendUInt(src.height);
        g_pDbusMessage->appendUInt(surface->OriginalSourceWidth);
        g_pDbusMessage->appendUInt(surface->OriginalSourceHeight);        
        g_pDbusMessage->appendUInt(dest.x);
        g_pDbusMessage->appendUInt(dest.y);
        g_pDbusMessage->appendUInt(dest.width);
        g_pDbusMessage->appendUInt(dest.height);
        g_pDbusMessage->appendUInt(orientation);
        g_pDbusMessage->appendBool(surface->getVisibility());
        g_pDbusMessage->appendUInt(surface->frameCounter);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetPropertiesOfLayer(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();

    Layer* layer = m_executor->getScene()->getLayer(id);
    if (layer != NULL)
    {
        Rectangle dest = layer->getDestinationRegion();
        Rectangle src = layer->getSourceRegion();
        OrientationType orientation = layer->getOrientation();

        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendDouble(layer->getOpacity());
        g_pDbusMessage->appendUInt(src.x);
        g_pDbusMessage->appendUInt(src.y);
        g_pDbusMessage->appendUInt(src.width);
        g_pDbusMessage->appendUInt(src.height);
        g_pDbusMessage->appendUInt(layer->OriginalSourceWidth);
        g_pDbusMessage->appendUInt(layer->OriginalSourceHeight);
        g_pDbusMessage->appendUInt(dest.x);
        g_pDbusMessage->appendUInt(dest.y);
        g_pDbusMessage->appendUInt(dest.width);
        g_pDbusMessage->appendUInt(dest.height);
        g_pDbusMessage->appendUInt(orientation);
        g_pDbusMessage->appendBool(layer->getVisibility());
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::CreateSurface(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint handle = g_pDbusMessage->getUInt();
    uint width = g_pDbusMessage->getUInt();
    uint height = g_pDbusMessage->getUInt();
    uint pixelformat = g_pDbusMessage->getUInt();
    PixelFormat pf = (PixelFormat) pixelformat;

    //LOG_DEBUG("DBUSCommunicator::CreateSurface","pixelformat: " << pixelformat);
    uint id = GraphicalObject::INVALID_ID;
    /* First of all create the surface */
    bool status = m_executor->execute(new SurfaceCreateCommand(&id));
    /* after that apply the native content */
    status &= m_executor->execute(new SurfaceSetNativeContentCommand(id, handle, pf, width, height));    
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(id);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}

void DBUSCommunicator::CreateSurfaceFromId(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint handle = g_pDbusMessage->getUInt();
    uint width = g_pDbusMessage->getUInt();
    uint height = g_pDbusMessage->getUInt();
    uint pixelformat = g_pDbusMessage->getUInt();
    PixelFormat pf = (PixelFormat) pixelformat;

    //LOG_DEBUG("DBUSCommunicator::CreateSurface","pixelformat: " << pixelformat);
    uint id = g_pDbusMessage->getUInt();

    /* First of all create the surface */
    bool status = m_executor->execute(new SurfaceCreateCommand(&id));
    /* after that apply the native content */
    status &= m_executor->execute(new SurfaceSetNativeContentCommand(id, handle, pf, width, height));    

    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(id);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}

void DBUSCommunicator::InitializeSurface(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = GraphicalObject::INVALID_ID;

    bool status = m_executor->execute(new SurfaceCreateCommand(&id));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(id);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}

void DBUSCommunicator::InitializeSurfaceFromId(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    bool status = m_executor->execute(new SurfaceCreateCommand(&id));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(id);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}

void DBUSCommunicator::SetSurfaceNativeContent(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint handle = g_pDbusMessage->getUInt();
    uint width = g_pDbusMessage->getUInt();
    uint height = g_pDbusMessage->getUInt();
    PixelFormat pf = (PixelFormat)g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new SurfaceSetNativeContentCommand(id, handle, pf, width, height));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}

void DBUSCommunicator::RemoveSurface(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint param = g_pDbusMessage->getUInt();
    bool status = m_executor->execute(new SurfaceRemoveCommand(param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}

void DBUSCommunicator::CreateLayer(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    uint id = GraphicalObject::INVALID_ID;
    // use resolution of default screen as default width and height of layers
    uint* resolution = m_executor->getScreenResolution(DEFAULT_SCREEN);
    bool status = m_executor->execute(new LayerCreateCommand(resolution[0], resolution[1], &id));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(id);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}

void DBUSCommunicator::CreateLayerFromId(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    uint id = GraphicalObject::INVALID_ID;
    g_pDbusMessage->initReceive(msg);
    id = g_pDbusMessage->getUInt();
    // use resolution of default screen as default width and height of layers
    uint* resolution = m_executor->getScreenResolution(DEFAULT_SCREEN);
    bool status = m_executor->execute(new LayerCreateCommand(resolution[0], resolution[1], &id));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(id);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}


void DBUSCommunicator::CreateLayerWithDimension(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint width = g_pDbusMessage->getUInt();
    uint height = g_pDbusMessage->getUInt();

    uint id = GraphicalObject::INVALID_ID;
    bool status = m_executor->execute(new LayerCreateCommand(width, height, &id));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(id);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::CreateLayerFromIdWithDimension(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    uint id = GraphicalObject::INVALID_ID;
    g_pDbusMessage->initReceive(msg);
    id = g_pDbusMessage->getUInt();
    uint width = g_pDbusMessage->getUInt();
    uint height = g_pDbusMessage->getUInt();
    bool status = m_executor->execute(new LayerCreateCommand(width, height, &id));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(id);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}

void DBUSCommunicator::RemoveLayer(DBusConnection* conn, DBusMessage* msg)
{
   (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint param = g_pDbusMessage->getUInt();
    bool status = m_executor->execute(new LayerRemoveCommand(param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::AddSurfaceToSurfaceGroup(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint surfaceid = g_pDbusMessage->getUInt();
    uint surfacegroupid = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new SurfacegroupAddSurfaceCommand(surfacegroupid, surfaceid));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::RemoveSurfaceFromSurfaceGroup(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint surfaceid = g_pDbusMessage->getUInt();
    uint surfacegroupid = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new SurfacegroupRemoveSurfaceCommand(surfacegroupid, surfaceid));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::AddLayerToLayerGroup(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint layerid = g_pDbusMessage->getUInt();
    uint layergroupid = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new LayergroupAddLayerCommand(layergroupid, layerid));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::RemoveLayerFromLayerGroup(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint layerid = g_pDbusMessage->getUInt();
    uint layergroupid = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new LayergroupRemoveLayerCommand(layergroupid, layerid));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::AddSurfaceToLayer(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint surfaceid = g_pDbusMessage->getUInt();
    uint layer = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new LayerAddSurfaceCommand(layer, surfaceid));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}

void DBUSCommunicator::RemoveSurfaceFromLayer(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint surfaceid = g_pDbusMessage->getUInt();
    uint layerid = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new LayerRemoveSurfaceCommand(layerid, surfaceid));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::CreateSurfaceGroup(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    uint newID = GraphicalObject::INVALID_ID;

    bool status = m_executor->execute(new SurfacegroupCreateCommand(&newID));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(newID);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}

void DBUSCommunicator::CreateSurfaceGroupFromId(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    uint newID = GraphicalObject::INVALID_ID;
    g_pDbusMessage->initReceive(msg);
    newID = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new SurfacegroupCreateCommand(&newID));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(newID);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}

void DBUSCommunicator::RemoveSurfaceGroup(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint param = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new SurfacegroupRemoveCommand(param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::CreateLayerGroup(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    uint newID = GraphicalObject::INVALID_ID;

    bool status = m_executor->execute(new LayergroupCreateCommand(&newID));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(newID);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}

void DBUSCommunicator::CreateLayerGroupFromId(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    uint newID = GraphicalObject::INVALID_ID;
    g_pDbusMessage->initReceive(msg);
    newID = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new LayergroupCreateCommand(&newID));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(newID);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_ALREADY_INUSE);
    }
}

void DBUSCommunicator::RemoveLayerGroup(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint param = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new LayergroupRemoveCommand(param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetSurfaceSourceRegion(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint x = g_pDbusMessage->getUInt();
    uint y = g_pDbusMessage->getUInt();
    uint width = g_pDbusMessage->getUInt();
    uint height = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new SurfaceSetSourceRectangleCommand(id, x, y, width, height));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetLayerSourceRegion(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint x = g_pDbusMessage->getUInt();
    uint y = g_pDbusMessage->getUInt();
    uint width = g_pDbusMessage->getUInt();
    uint height = g_pDbusMessage->getUInt();

    //LOG_DEBUG("DBUSC","new SetSourceRectangleCommand with arguments: " <<id <<" " << x <<" "<< y <<" "<< width <<" "<< height );
    bool status = m_executor->execute(new LayerSetSourceRectangleCommand(id, x, y, width, height));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetSurfaceDestinationRegion(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint x = g_pDbusMessage->getUInt();
    uint y = g_pDbusMessage->getUInt();
    uint width = g_pDbusMessage->getUInt();
    uint height = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new SurfaceSetDestinationRectangleCommand(id, x, y, width, height));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetSurfacePosition(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint x = g_pDbusMessage->getUInt();
    uint y = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new SurfaceSetPositionCommand(id, x, y));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetSurfacePosition(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint x = 0;
    uint y = 0;

    bool status = m_executor->execute(new SurfaceGetPositionCommand(id, &x, &y));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(x);
        g_pDbusMessage->appendUInt(y);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetSurfaceDimension(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint width = g_pDbusMessage->getUInt();
    uint height = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new SurfaceSetDimensionCommand(id, width, height));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetLayerDestinationRegion(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint x = g_pDbusMessage->getUInt();
    uint y = g_pDbusMessage->getUInt();
    uint width = g_pDbusMessage->getUInt();
    uint height = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new LayerSetDestinationRectangleCommand(id, x, y, width, height));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetLayerPosition(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint x = g_pDbusMessage->getUInt();
    uint y = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new LayerSetPositionCommand(id, x, y));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetLayerPosition(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint x = 0;
    uint y = 0;

    bool status = m_executor->execute(new LayerGetPositionCommand(id, &x, &y));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(x);
        g_pDbusMessage->appendUInt(y);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetLayerDimension(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint width = g_pDbusMessage->getUInt();
    uint height = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new LayerSetDimensionCommand(id, width, height));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetLayerDimension(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint width = 0;
    uint height = 0;

    bool status = m_executor->execute(new LayerGetDimensionCommand(id, &width, &height));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(width);
        g_pDbusMessage->appendUInt(height);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetSurfaceDimension(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint width = 0;
    uint height = 0;

    bool status = m_executor->execute(new SurfaceGetDimensionCommand(id, &width, &height));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(width);
        g_pDbusMessage->appendUInt(height);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetSurfaceOpacity(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    double param = g_pDbusMessage->getDouble();

    bool status = m_executor->execute(new SurfaceSetOpacityCommand(id, param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetLayerOpacity(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    double param = g_pDbusMessage->getDouble();

    bool status = m_executor->execute(new LayerSetOpacityCommand(id, param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetSurfacegroupOpacity(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    double param = g_pDbusMessage->getDouble();

    bool status = m_executor->execute(new SurfacegroupSetOpacityCommand(id, param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetLayergroupOpacity(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    double param = g_pDbusMessage->getDouble();

    bool status = m_executor->execute(new LayergroupSetOpacityCommand(id, param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetSurfaceOpacity(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    double param = 0.0;

    bool status = m_executor->execute(new SurfaceGetOpacityCommand(id, &param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendDouble(param);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetLayerOpacity(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    double param = 0.0;

    bool status = m_executor->execute(new LayerGetOpacityCommand(id, &param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendDouble(param);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetSurfaceOrientation(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint param = g_pDbusMessage->getUInt();
    OrientationType o = (OrientationType) param;

    bool status = m_executor->execute(new SurfaceSetOrientationCommand(id, o));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetSurfaceOrientation(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    OrientationType o;

    bool status = m_executor->execute(new SurfaceGetOrientationCommand(id, &o));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(o);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetLayerOrientation(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    uint param = g_pDbusMessage->getUInt();
    OrientationType o = (OrientationType) param;

    bool status = m_executor->execute(new LayerSetOrientationCommand(id, o));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetLayerOrientation(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    OrientationType o;

    bool status = m_executor->execute(new LayerGetOrientationCommand(id, &o));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(o);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetSurfacePixelformat(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    PixelFormat param;

    bool status = m_executor->execute(new SurfaceGetPixelformatCommand(id, &param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(param);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetSurfaceVisibility(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    const uint surfaceid = g_pDbusMessage->getUInt();
    bool newVis = g_pDbusMessage->getBool();

    bool status = m_executor->execute(new SurfaceSetVisibilityCommand(surfaceid, newVis));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetLayerVisibility(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    const uint layerid = g_pDbusMessage->getUInt();
    bool myparam = g_pDbusMessage->getBool();

    bool status = m_executor->execute(new LayerSetVisibilityCommand(layerid, myparam));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetSurfaceVisibility(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    bool param;

    bool status = m_executor->execute(new SurfaceGetVisibilityCommand(id, &param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        //LOG_DEBUG("DBUSCommunicator", "returning surfacevisibility: " << param);
        g_pDbusMessage->appendBool(param);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetLayerVisibility(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    bool param;

    bool status = m_executor->execute(new LayerGetVisibilityCommand(id, &param));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendBool(param);
        //LOG_DEBUG("DBUSCommunicator", "returning layervisibility: " << param);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetSurfacegroupVisibility(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint groupid = g_pDbusMessage->getUInt();
    bool myparam = g_pDbusMessage->getBool();

    bool status = m_executor->execute(new SurfacegroupSetVisibilityCommand(groupid, myparam));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetLayergroupVisibility(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint groupid = g_pDbusMessage->getUInt();
    bool myparam = g_pDbusMessage->getBool();

    bool status = m_executor->execute(new LayergroupSetVisibilityCommand(groupid, myparam));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}


void DBUSCommunicator::SetRenderOrderOfLayers(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    uint* array;
    int length;
    g_pDbusMessage->initReceive(msg);
    g_pDbusMessage->getArrayOfUInt(&length, &array);

    //LOG_DEBUG("DBUSCommunicator","Renderorder: Got " << length << " ids.");

    bool status = m_executor->execute(new ScreenSetRenderOrderCommand(array, length));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetSurfaceRenderOrderWithinLayer(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    uint* array;
    int length;

    g_pDbusMessage->initReceive(msg);
    uint layerid = g_pDbusMessage->getUInt();

    g_pDbusMessage->getArrayOfUInt(&length, &array);


    bool status = m_executor->execute(new LayerSetRenderOrderCommand(layerid, array, length));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetLayerType(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    Layer* l = m_executor->getScene()->getLayer(id);
    if (l != NULL)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(l->getLayerType());
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::GetLayertypeCapabilities(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    LayerType type = (LayerType) id;
    uint capabilities = m_executor->getLayerTypeCapabilities(type);
    //LOG_DEBUG("DBUSCommunicator", "GetLayertypeCapabilities: returning capabilities:" << capabilities);
    g_pDbusMessage->initReply(msg);
    g_pDbusMessage->appendUInt(capabilities);
    g_pDbusMessage->closeReply();
}

void DBUSCommunicator::GetLayerCapabilities(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();
    Layer* l = m_executor->getScene()->getLayer(id);
    if (l != NULL)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(l->getCapabilities());
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::FadeIn(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    g_pDbusMessage->initReply(msg);
    g_pDbusMessage->closeReply();
}

void DBUSCommunicator::SynchronizedFade(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    g_pDbusMessage->initReply(msg);
    g_pDbusMessage->closeReply();
}

void DBUSCommunicator::FadeOut(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    g_pDbusMessage->initReply(msg);
    g_pDbusMessage->closeReply();
}

void DBUSCommunicator::Exit(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    bool status = m_executor->execute(new ExitCommand());
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, INVALID_ARGUMENT);
    }
}

void DBUSCommunicator::CommitChanges(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    bool status = m_executor->execute(new CommitCommand());
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, INVALID_ARGUMENT);
    }
}

void DBUSCommunicator::CreateShader(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    char* vertname = g_pDbusMessage->getString();
    char* fragname = g_pDbusMessage->getString();
    uint id = 0;

    bool status = m_executor->execute(new ShaderCreateCommand(vertname, fragname, &id));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->appendUInt(id);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::DestroyShader(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint shaderid = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new ShaderDestroyCommand(shaderid));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetShader(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint surfaceId = g_pDbusMessage->getUInt();
    uint shaderid = g_pDbusMessage->getUInt();

    bool status = m_executor->execute(new SurfaceSetShaderCommand(surfaceId, shaderid));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::SetUniforms(DBusConnection* conn, DBusMessage* msg)
{
    (void)conn; // TODO: remove, only prevents warning

    g_pDbusMessage->initReceive(msg);
    uint id = g_pDbusMessage->getUInt();

    std::vector<string> uniforms;

    g_pDbusMessage->getArrayOfString(&uniforms);

    bool status = m_executor->execute(new ShaderSetUniformsCommand(id, uniforms));
    if (status)
    {
        g_pDbusMessage->initReply(msg);
        g_pDbusMessage->closeReply();
    }
    else
    {
        g_pDbusMessage->ReplyError(msg, SERVICE_ERROR, RESSOURCE_NOT_FOUND);
    }
}

void DBUSCommunicator::RemoveApplicationReference(char* owner)
{
    ApplicationReferenceMapIterator iter = m_executor->getApplicationReferenceMap()->find(IApplicationReference::generateApplicationHash(owner));
    ApplicationReferenceMapIterator iterEnd = m_executor->getApplicationReferenceMap()->end();

    if ( iter != iterEnd ) 
    {
        m_executor->removeApplicationReference((*iter).second);
    }
}


void DBUSCommunicator::AddClientWatch(DBusConnection *conn, char* sender) 
{
    DBusError err;
    dbus_error_init(&err);
    char rule[1024];
    sprintf(rule,"type='signal',sender='%s',interface='%s',member='%s',arg0='%s'",DBUS_INTERFACE_DBUS,DBUS_INTERFACE_DBUS,"NameOwnerChanged",sender);
    
    dbus_bus_add_match(conn,rule,&err);
    if (dbus_error_is_set(&err))
    {
        LOG_ERROR("DBUSCommunicator", "Could not add client watch "<< err.message);
        dbus_error_free(&err);
    }
}

void DBUSCommunicator::RemoveClientWatch(DBusConnection *conn, char* sender) 
{
    DBusError err;
    dbus_error_init(&err);
    char rule[1024];
    sprintf(rule,"type='signal',sender='%s',interface='%s',member='%s',arg0='%s'",DBUS_INTERFACE_DBUS,DBUS_INTERFACE_DBUS,"NameOwnerChanged",sender);
    
    dbus_bus_remove_match(conn,rule,&err);
    if (dbus_error_is_set(&err))
    {
        LOG_ERROR("DBUSCommunicator", "Could not remove client watch "<< err.message);
        dbus_error_free(&err);
    }
}

DBusHandlerResult DBUSCommunicator::delegateMessage(DBusConnection* conn, DBusMessage* msg) 
{
    DBusHandlerResult result = DBUS_HANDLER_RESULT_HANDLED;
    LOG_DEBUG("DBUSCommunicator","message received");
    const char *n = dbus_message_get_member(msg);
    char* owner = strdup(dbus_message_get_sender(msg));
    bool found = false;
    int i = 0;

    while (!found && strcmp(manager_methods[i].name, "") != 0)
    {
        if (n && strcmp(manager_methods[i].name, n) == 0)
        {
            MethodTable entry = manager_methods[i];
            LOG_DEBUG("DBUSCommunicator","got call for method:" << entry.name);
            CallBackMethod m = entry.function;
            LOG_DEBUG("DBUSCommunicator","enter method");
            (this->*m)(conn, msg);
            found = true;
        }
        i++;
    }

    if (dbus_message_is_method_call(msg, DBUS_INTERFACE_INTROSPECTABLE, "Introspect"))
    {
        LOG_DEBUG("DBUSCommunicator", "Introspection called");
        DBUSIntrospection introspectionString(manager_methods);
        introspectionString.process(conn, msg);
        g_pDbusMessage->setConnection(conn);
        found = true; // TODO: always true
    }
    
    if (dbus_message_is_signal(msg, DBUS_INTERFACE_DBUS, "NameOwnerChanged"))
    {
        char *name, *oldName, *newName;
        LOG_DEBUG("DBUSCommunicator","NameOwner Changed detected ");
        if (!dbus_message_get_args(msg, NULL,
            DBUS_TYPE_STRING, &name,
            DBUS_TYPE_STRING, &oldName,
            DBUS_TYPE_STRING, &newName,
            DBUS_TYPE_INVALID)) 
        {
            LOG_WARNING("DBUSCommunicator","Invalid arguments for NameOwnerChanged signal");
        } else if ( *newName == '\0' ) 
        {
            LOG_DEBUG("DBUSCommunicator","Client Disconnect detected " << name);
            RemoveClientWatch(conn,owner);
            RemoveApplicationReference(name);
        }
    }
    if (!found)
    {
        result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
    return result;
}

extern "C" ICommunicator* createDBUSCommunicator(ICommandExecutor* executor)
{
    return new DBUSCommunicator(executor);
}

extern "C" void destroyDBUSCommunicator(DBUSCommunicator* p)
{
    delete p;
}

