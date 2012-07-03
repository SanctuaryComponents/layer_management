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
#include "introspection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // memcpy


//=============================================================================
// message handling
//=============================================================================
t_ilm_bool createMessage(t_ilm_const_string name)
{
    LOG_ENTER_FUNCTION;
    t_ilm_bool returnValue = ILM_FALSE;

    if (gpDbusState->isClient)
    {
        gpCurrentMessage->name = name;
        gpCurrentMessage->pMessage = dbus_message_new_method_call(ILM_SERVICE_NAME,
                                                            ILM_PATH_COMPOSITE_SERVICE,
                                                            ILM_INTERFACE_COMPOSITE_SERVICE,
                                                            gpCurrentMessage->name);
        dbus_message_iter_init_append(gpCurrentMessage->pMessage, &gpCurrentMessage->iter);

        if (NULL != gpCurrentMessage->pMessage)
        {
            returnValue = ILM_TRUE;
        }
    }
    else
    {
        gpCurrentMessage->pMessage = dbus_message_new_method_return(gpCurrentMessage->pMessage);
        dbus_message_iter_init_append(gpCurrentMessage->pMessage, &gpCurrentMessage->iter);
        returnValue = ILM_TRUE;
    }

    return returnValue;
}

t_ilm_bool sendMessage()
{
    LOG_ENTER_FUNCTION;
    t_ilm_bool returnValue = ILM_FALSE;

    if (gpDbusState->isClient)
    {
        gpCurrentMessage->pPending = NULL;
        returnValue = dbus_connection_send_with_reply(gpDbusState->connection,
                                                      gpCurrentMessage->pMessage,
                                                      &gpCurrentMessage->pPending,
                                                      DBUS_RECEIVE_TIMEOUT_IN_MS);
    }
    else
    {
        t_ilm_int serial = dbus_message_get_serial(gpCurrentMessage->pMessage);
        if (!dbus_connection_send(gpDbusState->connection, gpCurrentMessage->pMessage, &serial))
        {
            printf("DBUSIpcModule: Out Of Memory!\n");
            exit(1);
        }
    }

    dbus_connection_flush(gpDbusState->connection);

    dbus_message_unref(gpCurrentMessage->pMessage);
    gpCurrentMessage->pMessage = NULL;
    gpCurrentMessage->name = "";

    return returnValue;
}

t_ilm_bool sendError(t_ilm_const_string desc)
{
    LOG_ENTER_FUNCTION;
    t_ilm_bool returnValue = ILM_FALSE;

    if (!gpDbusState->isClient)
    {
        t_ilm_uint serial = 0;
        DBusMessage* errorMsg = dbus_message_new_error(gpCurrentMessage->pMessage,
                                                          DBUS_SERVICE_ERROR,
                                                          desc);

        if (!dbus_connection_send(gpDbusState->connection,
                                  errorMsg,
                                  &serial))
        {
            printf("DBUSIpcModule: Out Of Memory!\n");
            exit(1);
        }
        dbus_connection_flush(gpDbusState->connection);

        // free the reply
        dbus_message_unref(errorMsg);
        errorMsg = NULL;
    }

    return returnValue;
}

enum IpcMessageType receiveMessage(t_ilm_int timeoutInMs)
{
    //LOG_ENTER_FUNCTION;
    enum IpcMessageType returnValue = IpcMessageTypeNone;

    if (gpDbusState->isClient)
    {
        if (gpCurrentMessage->pPending)
        {
            // block until we receive a reply
            dbus_pending_call_block(gpCurrentMessage->pPending);

            // get the reply message
            gpCurrentMessage->pMessage = dbus_pending_call_steal_reply(gpCurrentMessage->pPending);
            dbus_pending_call_unref(gpCurrentMessage->pPending);
            gpCurrentMessage->pPending = NULL;

            dbus_message_iter_init(gpCurrentMessage->pMessage, &gpCurrentMessage->iter);
            gpCurrentMessage->sender = dbus_message_get_sender(gpCurrentMessage->pMessage);
            gpCurrentMessage->name = dbus_message_get_member(gpCurrentMessage->pMessage);
            returnValue = IpcMessageTypeCommand;
        }
    }
    else
    {
        gpCurrentMessage->pMessage = NULL;

        dbus_connection_read_write(gpDbusState->connection, timeoutInMs);
        gpCurrentMessage->pMessage = dbus_connection_pop_message(gpDbusState->connection);
        if (gpCurrentMessage->pMessage)
        {
            int messageType = dbus_message_get_type(gpCurrentMessage->pMessage);
            dbus_message_iter_init(gpCurrentMessage->pMessage, &gpCurrentMessage->iter);

            gpCurrentMessage->sender = dbus_message_get_sender(gpCurrentMessage->pMessage);

            if (dbus_message_is_signal(gpCurrentMessage->pMessage, DBUS_INTERFACE_DBUS, "NameOwnerChanged"))
            {
                char *name, *oldName, *newName;
                if (!dbus_message_get_args(gpCurrentMessage->pMessage, NULL,
                    DBUS_TYPE_STRING, &name,
                    DBUS_TYPE_STRING, &oldName,
                    DBUS_TYPE_STRING, &newName,
                    DBUS_TYPE_INVALID))
                {
                    returnValue = IpcMessageTypeError;
                }
                else
                {
                    if (*newName == '\0')
                    {
                        gpCurrentMessage->sender = oldName;
                        returnValue =  IpcMessageTypeDisconnect;
                    }
                    else
                    {
                        returnValue =  IpcMessageTypeNone;
                    }
                }
            }

            else if (DBUS_MESSAGE_TYPE_ERROR != messageType)
            {
                gpCurrentMessage->name = dbus_message_get_member(gpCurrentMessage->pMessage);
                returnValue = IpcMessageTypeCommand;
            }

            else if (dbus_message_is_method_call(gpCurrentMessage->pMessage, DBUS_INTERFACE_INTROSPECTABLE, "Introspect"))
            {
                sendIntrospectionResponse(gpDbusState->connection, gpCurrentMessage->pMessage);
                returnValue = IpcMessageTypeNone;
            }

            else
            {
                returnValue = IpcMessageTypeError;
            }
        }
    }

    return returnValue;
}

t_ilm_const_string getMessageName()
{
    LOG_ENTER_FUNCTION;
    return gpCurrentMessage->name;
}

t_ilm_const_string getSenderName()
{
    LOG_ENTER_FUNCTION;
    return gpCurrentMessage->sender;
}

t_ilm_bool isErrorMessage()
{
    LOG_ENTER_FUNCTION;
    int messageType = dbus_message_get_type(gpCurrentMessage->pMessage);
    return (DBUS_MESSAGE_TYPE_ERROR == messageType);
}

//=============================================================================
// print debug information
//=============================================================================
void printTypeName(int type)
{
    switch (type)
    {
    case DBUS_TYPE_ARRAY:
        printf("DBUS_TYPE_ARRAY\n");
        break;
    case DBUS_TYPE_BOOLEAN:
        printf("DBUS_TYPE_BOOLEAN\n");
        break;
    case DBUS_TYPE_DOUBLE:
        printf("DBUS_TYPE_DOUBLE\n");
        break;
    case DBUS_TYPE_INT32:
        printf("DBUS_TYPE_INT32\n");
        break;
    case DBUS_TYPE_STRING:
        {
            char text[1024];
            getString(text);
            printf("DBUS_TYPE_STRING (%s)\n", text);
        }
        break;
    case DBUS_TYPE_UINT32:
        printf("DBUS_TYPE_UINT\n");
        break;
    default:
        printf("unknown (%s)\n", (char*)&type);
        break;
    }
}

