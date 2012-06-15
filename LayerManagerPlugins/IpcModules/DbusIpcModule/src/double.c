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


t_ilm_bool appendDouble(const t_ilm_float value)
{
    LOG_ENTER_FUNCTION;
    //printf("%lf\n", value);
    return dbus_message_iter_append_basic(&gpCurrentMessage->iter, DBUS_TYPE_DOUBLE, &value);
}

t_ilm_bool getDouble(t_ilm_float* value)
{
    LOG_ENTER_FUNCTION;
    t_ilm_bool returnValue = ILM_FALSE;

    t_ilm_int type = dbus_message_iter_get_arg_type(&gpCurrentMessage->iter);

    if (DBUS_TYPE_DOUBLE == type)
    {
        dbus_message_iter_get_basic(&gpCurrentMessage->iter, value);
        dbus_message_iter_next(&gpCurrentMessage->iter);
        returnValue = ILM_TRUE;
    }
    else
    {
        printf("ERROR: expected: DBUS_TYPE_DOUBLE,received ");
        printTypeName(type);
    }

    //printf("%s - %lf\n", returnValue ? "SUCCESS" : "FAIL", *value);
    return returnValue;
}

