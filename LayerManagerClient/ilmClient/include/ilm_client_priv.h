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
#ifndef _ILM_CLIENT_PRIV_H_
#define _ILM_CLIENT_PRIV_H_

#include <dbus/dbus.h> // TODO: This file should only be included from ilmClient.c
#include "ilm_types.h"

typedef struct _ilm_param
{
    t_ilm_int paramtype;
    t_ilm_int paramlength;
    void* param;
    t_ilm_bool isbasictype;
} t_ilm_param;

#ifdef _ILM_CLIENT_C_

#define ILM_INTERFACE_COMPOSITE_CLIENT "org.genivi.layermanagementclient"

typedef struct s_ilmClient
{
    DBusConnection *dbus_connection;
    DBusBusType     dbus_type;
    DBusError       dbus_error;
} t_ilm_client;

static t_ilm_bool g_ilm_init = ILM_FALSE;
static t_ilm_client* g_ilm_client;

DBusMessage* _ilm_dbus_method_call(DBusConnection* const connection, const t_ilm_char *method, t_ilm_param* param, const t_ilm_uint paramlength);
ilmErrorTypes _ilm_get_dbus_array_length(DBusMessage* const message, const t_ilm_int type, t_ilm_int* const length);
ilmErrorTypes _ilm_get_dbus_array_elements(DBusMessage* const message, const t_ilm_int type,void* array);
ilmErrorTypes _ilm_get_dbus_basic_elements(DBusMessage* const message, t_ilm_uint length,t_ilm_param* param);
void _ilm_setup_param(t_ilm_param* paramStruc,t_ilm_int type, void* parameter );
void _ilm_setup_array(t_ilm_param* paramStruc,t_ilm_int type, t_ilm_int length,void* parameter );

t_ilm_bool _ilm_get_boolean_element(DBusMessageIter *iter);
t_ilm_byte _ilm_get_byte_element(DBusMessageIter *iter);
t_ilm_uint _ilm_get_uint32_element(DBusMessageIter *iter);
t_ilm_int _ilm_get_int32_element(DBusMessageIter *iter);
t_ilm_ulong _ilm_get_uint64_element(DBusMessageIter *iter);
t_ilm_long _ilm_get_int64_element(DBusMessageIter *iter);
t_ilm_float _ilm_get_float_element(DBusMessageIter *iter);

void _ilm_close_dbus_method_call(const DBusMessage* message);

#define ILM_ERROR(method,error) \
    fprintf (stderr,"ilm client | %s | %s\n",method,error)

#define ILM_CHECK_METHOD_ERROR(message) \
if (message){ \
    t_ilm_int messageType = dbus_message_get_type(message); \
    if (messageType == DBUS_MESSAGE_TYPE_ERROR) \
    { \
        t_ilm_param errorparam[1]; \
        char *errorbuf; \
        char writeoutbuf[256]; \
        memset(writeoutbuf,0,256); \
        _ilm_setup_param(errorparam, DBUS_TYPE_STRING, &errorbuf); \
        _ilm_get_dbus_basic_elements(message, 1, errorparam); \
        sprintf(writeoutbuf,"%s : %s\n",dbus_message_get_error_name(message), errorbuf); \
        fprintf (stderr,"ilm client | %s | %s\n",__func__, writeoutbuf ); \
        return ILM_FAILED; \
    } \
}

#endif /* _ILM_CLIENT_C_ */
#endif /* _ILM_CLIENT_PRIV_H_ */

