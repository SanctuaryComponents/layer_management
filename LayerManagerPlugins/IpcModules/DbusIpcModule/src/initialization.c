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
#include "IpcModule.h"
#include "common.h"
#include "DBUSConfiguration.h"
#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>


//=============================================================================
// prototypes
//=============================================================================
t_ilm_bool initService();

t_ilm_bool destroyClient();
t_ilm_bool destroyService();


//=============================================================================
// setup
//=============================================================================
t_ilm_bool init(t_ilm_bool isClient)
{
    LOG_ENTER_FUNCTION;

    gpCurrentMessage = malloc(sizeof(dbusmessage));
    gpDbusState = malloc(sizeof(dbus));

    gpDbusState->initialized = ILM_FALSE;
    gpDbusState->isClient = isClient;

    dbus_error_init(&gpDbusState->error);

    char* useSessionBus = getenv("LM_USE_SESSION_BUS");
    if (useSessionBus && strcmp(useSessionBus, "enable") == 0 )
    {
        gpDbusState->type = DBUS_BUS_SESSION;
        printf("DbusIpcmodule: using session bus\n");
    }
    else
    {
        gpDbusState->type = DBUS_BUS_SYSTEM;
        printf("DbusIpcmodule: using system bus\n");
    }

    gpDbusState->connection = dbus_bus_get(gpDbusState->type, &gpDbusState->error);

    if (dbus_error_is_set(&gpDbusState->error) || !gpDbusState->connection)
    {
        printf("DbusIpcmodule: Connection error\n");
        dbus_error_free(&gpDbusState->error);
        exit(1);
    }

    if (gpDbusState->isClient)
    {
        gpDbusState->initialized = ILM_TRUE;
    }
    else
    {
        gpDbusState->initialized = initService();
    }


    return gpDbusState->initialized;
}

t_ilm_bool destroy()
{
    LOG_ENTER_FUNCTION;

    if (gpDbusState->initialized)
    {
        if (gpDbusState->isClient)
        {
            destroyClient();
        }
        else
        {
            destroyService();
        }
    }

    free(gpCurrentMessage);
    free(gpDbusState);

    return !gpDbusState->initialized;
}


//=============================================================================
// service specific
//=============================================================================
t_ilm_bool initService()
{
    LOG_ENTER_FUNCTION;

    t_ilm_bool result = ILM_TRUE;

    printf("DbusIpcmodule: registering dbus address %s\n", DBUS_SERVICE_PREFIX);
    int ret = dbus_bus_request_name(gpDbusState->connection, DBUS_SERVICE_PREFIX, DBUS_NAME_FLAG_REPLACE_EXISTING, &gpDbusState->error);

    if (dbus_error_is_set(&gpDbusState->error))
    {
        printf("DbusIpcmodule: Name Error %s\n", gpDbusState->error.message);
        dbus_error_free(&gpDbusState->error);
        result = ILM_FALSE;
    }

    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret)
    {
        printf("DbusIpcmodule: Not Primary Owner of %s (error %d)\n", DBUS_SERVICE_PREFIX, ret);
        result = ILM_FALSE;
    }

    const char* rule = "type='signal',"
                       "sender='"DBUS_INTERFACE_DBUS"',"
                       "interface='"DBUS_INTERFACE_DBUS"',"
                       "member='NameOwnerChanged'";

    dbus_bus_add_match(gpDbusState->connection, rule, &gpDbusState->error);
    if (dbus_error_is_set(&gpDbusState->error))
    {
        printf("DbusIpcModule: Could not add client watch, error: %s\n", gpDbusState->error.message);
        dbus_error_free(&gpDbusState->error);
    }

    return result;
}


t_ilm_bool destroyService()
{
    LOG_ENTER_FUNCTION;

    DBusError err;
    dbus_error_init(&err);

    t_ilm_bool errorset = dbus_error_is_set(&err);
    if (errorset)
    {
        printf("DbusIpcmodule: there was an dbus error\n");
    }

    dbus_bus_name_has_owner(gpDbusState->connection, DBUS_SERVICE_PREFIX, &err);
    errorset = dbus_error_is_set(&err);
    if (errorset)
    {
        printf("DbusIpcmodule: there was an dbus error\n");
    }
    dbus_error_init(&err);
    dbus_bus_release_name(gpDbusState->connection, DBUS_SERVICE_PREFIX, &err);
}


//=============================================================================
// client specific
//=============================================================================

t_ilm_bool destroyClient()
{
    LOG_ENTER_FUNCTION;
    dbus_connection_unref(gpDbusState->connection);
    dbus_error_free(&gpDbusState->error);
    gpDbusState->initialized = ILM_FALSE;
}
