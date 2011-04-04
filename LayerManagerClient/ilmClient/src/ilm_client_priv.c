/***************************************************************************
*
* Copyright 2010,2011 BMW Car IT GmbH
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*		http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
****************************************************************************/
#define _ILM_CLIENT_PRIV_C_
#include <dbus/dbus.h>
#include "ilm_types.h"
#include "ilm_client_priv.h"
#include <stdio.h>
#include <stdlib.h>
#define ILM_SERVICE_NAME "de.bmw.CompositingService"
#define ILM_PATH_COMPOSITE_SERVICE "/de/bmw/CompositingService"
#define ILM_INTERFACE_COMPOSITE_SERVICE "de.bmw.CompositingService"

void _ilm_setup_param(t_ilm_param* const paramStruc,t_ilm_int type, void* parameter )
{
	paramStruc->param = parameter;
	paramStruc->paramtype = type;
	paramStruc->paramlength = 1;
	paramStruc->isbasictype = ILM_TRUE;
}

void _ilm_setup_array(t_ilm_param* paramStruc,t_ilm_int type, t_ilm_int length,void* parameter )
{
	paramStruc->param = parameter;
	paramStruc->paramtype = type;
	paramStruc->paramlength = length;
	paramStruc->isbasictype = ILM_FALSE;
}


DBusMessage* _ilm_dbus_method_call(DBusConnection* const connection,const t_ilm_char *method,t_ilm_param* param, const t_ilm_uint paramlength)
{
	DBusMessage* message;
	DBusPendingCall* pending;
	DBusMessageIter msgIter;
	DBusMessageIter arrayIter;
	t_ilm_int i,j = 0;
    message = dbus_message_new_method_call (ILM_SERVICE_NAME,
                                            ILM_PATH_COMPOSITE_SERVICE,
                                            ILM_INTERFACE_COMPOSITE_SERVICE,
                                            method);
//    dbus_message_set_auto_start (message, TRUE);
//    dbus_message_set_destination(message,ILM_INTERFACE_COMPOSITE_SERVICE);
    if (paramlength > 0
    	&& param != NULL )
    {
    	dbus_message_iter_init_append(message,&msgIter);
    	for (i=0;i<paramlength;i++)
    	{
    		if ( param[i].isbasictype == ILM_TRUE)
    		{
    			dbus_message_iter_append_basic(&msgIter,param[i].paramtype,param[i].param);
    		} else {
    			char signature[2];
    			signature[0]= (char) param[i].paramtype;
    			signature[1]= 0;
    			dbus_message_iter_open_container(&msgIter,DBUS_TYPE_ARRAY, signature ,&arrayIter);
    			void* paramPointer = param[i].param;
    			for (	j=0;	j < param[i].paramlength;	j++)
    			{

                          switch (param[i].paramtype)
                          {
                                  case DBUS_TYPE_BOOLEAN       : dbus_message_iter_append_basic(&arrayIter,param[i].paramtype,&((t_ilm_bool*)(paramPointer))[j]); break;
                                  case DBUS_TYPE_BYTE          : dbus_message_iter_append_basic(&arrayIter,param[i].paramtype,&((t_ilm_byte*)(paramPointer))[j]); break;
                                  case DBUS_TYPE_DOUBLE        : dbus_message_iter_append_basic(&arrayIter,param[i].paramtype,&((t_ilm_float*)(paramPointer))[j]); break;
                                  case DBUS_TYPE_INT32         : dbus_message_iter_append_basic(&arrayIter,param[i].paramtype,&((t_ilm_int*)(paramPointer))[j]); break;
                                  case DBUS_TYPE_UINT32        : dbus_message_iter_append_basic(&arrayIter,param[i].paramtype,&((t_ilm_uint*)(paramPointer))[j]); break;
                                  case DBUS_TYPE_INT64         : dbus_message_iter_append_basic(&arrayIter,param[i].paramtype,&((t_ilm_long*)(paramPointer))[j]); break;
                                  case DBUS_TYPE_UINT64        : dbus_message_iter_append_basic(&arrayIter,param[i].paramtype,&((t_ilm_ulong*)(paramPointer))[j]); break;
                          }



    			}
    			dbus_message_iter_close_container(&msgIter,&arrayIter);
    		}
    	}
    }

    if (!dbus_connection_send_with_reply (connection, message, &pending, 500))
	{
		   return NULL;
	}
	dbus_connection_flush(connection);
	dbus_message_unref(message);
	if ( pending == NULL )
	{
		   return NULL;
	}
	// block until we recieve a reply
	dbus_pending_call_block(pending);
	// get the reply message
	message = dbus_pending_call_steal_reply(pending);
	dbus_pending_call_unref(pending);
    return message;
}

ilmErrorTypes _ilm_get_dbus_basic_elements(DBusMessage* const message, const t_ilm_uint length,t_ilm_param* param)
{
	ilmErrorTypes error = ILM_FAILED;
	DBusMessageIter msgIter;
	if (dbus_message_iter_init(message, &msgIter))
	{
	   if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&msgIter) )
	   {
		   t_ilm_int index = 0;
		   for (index = 0;index < length;index++)
		   {
			   t_ilm_int argType = dbus_message_iter_get_arg_type(&msgIter);
			   if (argType==param[index].paramtype)
			   {
				   dbus_message_iter_get_basic(&msgIter,param[index].param);
				   dbus_message_iter_next(&msgIter);
			   } else {
				 return ILM_FAILED;
			   }
		   }
		   error = ILM_SUCCESS;
	   }
	}
	return error;
}

void _ilm_close_dbus_method_call(DBusMessage* const message)
{
	if (message != NULL)
	{
		dbus_message_unref(message);
	}
}
t_ilm_bool _ilm_get_boolean_element(DBusMessageIter *iter)
{
	t_ilm_bool result;
	dbus_message_iter_get_basic(iter,&result);
	return result;
}
t_ilm_byte _ilm_get_byte_element(DBusMessageIter *iter)
{
	t_ilm_byte result;
	dbus_message_iter_get_basic(iter,&result);
	return result;
}

t_ilm_uint _ilm_get_uint32_element(DBusMessageIter *iter)
{
	t_ilm_uint result;
	dbus_message_iter_get_basic(iter,&result);
	return result;
}

t_ilm_int _ilm_get_int32_element(DBusMessageIter *iter)
{
	t_ilm_int result;
	dbus_message_iter_get_basic(iter,&result);
	return result;
}

t_ilm_ulong _ilm_get_uint64_element(DBusMessageIter *iter)
{
	t_ilm_ulong result;
	dbus_message_iter_get_basic(iter,&result);
	return result;
}

t_ilm_long _ilm_get_int64_element(DBusMessageIter *iter)
{
	t_ilm_long result;
	dbus_message_iter_get_basic(iter,&result);
	return result;
}

t_ilm_float _ilm_get_float_element(DBusMessageIter *iter)
{
	t_ilm_float result;
	dbus_message_iter_get_basic(iter,&result);
	return result;
}

ilmErrorTypes _ilm_get_dbus_array_elements(DBusMessage* const message,const t_ilm_int type,void* array)
{
	ilmErrorTypes error = ILM_FAILED;
	DBusMessageIter arrayIter;
	DBusMessageIter msgIter;
	if (dbus_message_iter_init(message, &msgIter))
	{
	   if (DBUS_TYPE_ARRAY == dbus_message_iter_get_arg_type(&msgIter) )
	   {
		   t_ilm_int index = 0;

		   dbus_message_iter_recurse(&msgIter,&arrayIter);
		   while (dbus_message_iter_get_arg_type(&arrayIter)!=DBUS_TYPE_INVALID)
		   {
			   if (dbus_message_iter_get_arg_type(&arrayIter)==type)
			   {
				   switch (type)
				   {
					   case DBUS_TYPE_BOOLEAN	: ((t_ilm_bool*)array)[index] = _ilm_get_boolean_element(&arrayIter); break;
					   case DBUS_TYPE_BYTE		: ((t_ilm_byte*)array)[index] = _ilm_get_byte_element(&arrayIter); break;
					   case DBUS_TYPE_DOUBLE	: ((t_ilm_float*)array)[index] = _ilm_get_float_element(&arrayIter); break;
					   case DBUS_TYPE_INT32		: ((t_ilm_int*)array)[index] = _ilm_get_int32_element(&arrayIter); break;
					   case DBUS_TYPE_UINT32	: ((t_ilm_uint*)array)[index] = _ilm_get_uint32_element(&arrayIter); break;
					   case DBUS_TYPE_INT64		: ((t_ilm_long*)array)[index] = _ilm_get_int64_element(&arrayIter); break;
					   case DBUS_TYPE_UINT64	: ((t_ilm_ulong*)array)[index] = _ilm_get_uint64_element(&arrayIter); break;
				   }
				   index++;
			   }
			   dbus_message_iter_next(&arrayIter);
		   }
		   error = ILM_SUCCESS;
	   }
	}
	return error;
}

ilmErrorTypes _ilm_get_dbus_array_length(DBusMessage* const message, const t_ilm_int type,t_ilm_int* const length)
{
	ilmErrorTypes error = ILM_FAILED;
	DBusMessageIter arrayIter;
	DBusMessageIter msgIter;
	if (dbus_message_iter_init(message, &msgIter))
	{
	   if (DBUS_TYPE_ARRAY == dbus_message_iter_get_arg_type(&msgIter) )
	   {
		   t_ilm_int arrayLen = 0;

		   dbus_message_iter_recurse(&msgIter,&arrayIter);
		   while (dbus_message_iter_get_arg_type(&arrayIter)!=DBUS_TYPE_INVALID)
		   {
			   if (dbus_message_iter_get_arg_type(&arrayIter)==type)
			   {
				   arrayLen++;
			   }
			   dbus_message_iter_next(&arrayIter);
		   }
		   *length = arrayLen;
		   error = ILM_SUCCESS;
	   }
	}
	return error;
}

#undef _ILM_CLIENT_PRIV_C_
