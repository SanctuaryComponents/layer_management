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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // memcpy

t_ilm_bool appendUint(const t_ilm_uint value)
{
    LOG_ENTER_FUNCTION;
    //printf("%u\n", value);
    return dbus_message_iter_append_basic(&gpCurrentMessage->iter, DBUS_TYPE_UINT32, &value);
}

t_ilm_bool appendUintArray(const t_ilm_uint* valueArray, t_ilm_int arraySize)
{
    LOG_ENTER_FUNCTION;

    t_ilm_bool returnValue = ILM_FALSE;
    char signature[2] = { DBUS_TYPE_UINT32, 0 };
    DBusMessageIter arrayIter;

    returnValue = dbus_message_iter_open_container(&gpCurrentMessage->iter, DBUS_TYPE_ARRAY, signature, &arrayIter);

    t_ilm_int index = 0;
    for (index = 0; index < arraySize; ++index)
    {
        returnValue &= dbus_message_iter_append_basic(&arrayIter, DBUS_TYPE_UINT32, &valueArray[index]);
    }

    returnValue &= dbus_message_iter_close_container(&gpCurrentMessage->iter, &arrayIter);

    //printf("%s\n", returnValue ? "SUCCESS" : "FAIL");
    return returnValue;
}

t_ilm_bool getUintArray(t_ilm_uint** valueArray, t_ilm_int* arraySize)
{
    LOG_ENTER_FUNCTION;
    t_ilm_bool returnValue = ILM_FALSE;

    t_ilm_int type = dbus_message_iter_get_arg_type(&gpCurrentMessage->iter);

    if (DBUS_TYPE_ARRAY == type)
    {
        returnValue = ILM_TRUE;

        DBusMessageIter arrayIter;
        int index = 0;

        dbus_message_iter_recurse(&gpCurrentMessage->iter, &arrayIter);

        // get pointer to dbus internal array data (zero copy)
        t_ilm_uint* dbusArrayPointer = NULL;
        dbus_message_iter_get_fixed_array(&arrayIter, &dbusArrayPointer, arraySize);

        // create callers buffer, copy data to buffer
        *valueArray = malloc(sizeof(t_ilm_uint) * (*arraySize));
        memcpy(*valueArray, dbusArrayPointer, sizeof(t_ilm_uint) * (*arraySize));
    }
    else
    {
        printf("ERROR: expected: DBUS_TYPE_ARRAY, received ");
        printTypeName(type);
    }

    //printf("%s\n", returnValue ? "SUCCESS" : "FAIL");
    return returnValue;
}

t_ilm_bool getUint(t_ilm_uint* value)
{
    LOG_ENTER_FUNCTION;
    t_ilm_bool returnValue = ILM_FALSE;

    t_ilm_int type = dbus_message_iter_get_arg_type(&gpCurrentMessage->iter);

    if (DBUS_TYPE_UINT32 == type)
    {
        dbus_message_iter_get_basic(&gpCurrentMessage->iter, value);
        dbus_message_iter_next(&gpCurrentMessage->iter);
        returnValue = ILM_TRUE;
    }
    else
    {
        printf("ERROR: expected: DBUS_TYPE_UINT32, received ");
        printTypeName(type);
    }
    //printf("%s - %u\n", returnValue ? "SUCCESS" : "FAIL", *value);
    return returnValue;
}
