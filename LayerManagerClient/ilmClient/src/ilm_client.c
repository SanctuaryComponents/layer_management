/**************************************************************************
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
#define _ILM_CLIENT_C_
#include "ilm_client.h"
#include "ilm_client_priv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INVALID_ID 0xFFFFFFFF

ilmErrorTypes ilm_init()
{
    ilmErrorTypes result = ILM_FAILED;
    // Only initialize if the client is not running
    if (g_ilm_init == ILM_FALSE)
    {
        // initialize the client structure
        g_ilm_client = malloc(sizeof(t_ilm_client));

        if (g_ilm_client)
        {
            g_ilm_init = ILM_TRUE;
            // initialize the dbus connection
            dbus_error_init(&g_ilm_client->dbus_error);
            g_ilm_client->dbus_type = DBUS_BUS_SESSION;
            g_ilm_client->dbus_connection = dbus_bus_get(g_ilm_client->dbus_type, &g_ilm_client->dbus_error);
            if (!g_ilm_client->dbus_connection)
            {
                ILM_ERROR("ilm_init","Can not setup dbus connection\n");
                dbus_error_free(&g_ilm_client->dbus_error);
            }
            else
            {
                // request name on connection
                dbus_bus_request_name(g_ilm_client->dbus_connection,
                        ILM_INTERFACE_COMPOSITE_CLIENT,
                        DBUS_NAME_FLAG_REPLACE_EXISTING,
                        &g_ilm_client->dbus_error);

                if (dbus_error_is_set(&g_ilm_client->dbus_error))
                {
                    ILM_ERROR("ilm_init","Can not request name\n");
                    dbus_error_free(&g_ilm_client->dbus_error);
                }
                else
                {
                    result = ILM_SUCCESS;
                }
            }
        }
    }
    return result;
}

ilmErrorTypes ilm_getPropertiesOfSurface(t_ilm_uint surfaceID, struct ilmSurfaceProperties* pSurfaceProperties)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    if (!pSurfaceProperties)
    {
        return returnValue;
    }

    // Setup parameter to send
    t_ilm_param param[12];
    _ilm_setup_param(&param[0], DBUS_TYPE_UINT32, &surfaceID);
    _ilm_setup_param(&param[1], DBUS_TYPE_DOUBLE, &pSurfaceProperties->opacity);
    _ilm_setup_param(&param[2], DBUS_TYPE_UINT32, &pSurfaceProperties->sourceX);
    _ilm_setup_param(&param[3], DBUS_TYPE_UINT32, &pSurfaceProperties->sourceY);
    _ilm_setup_param(&param[4], DBUS_TYPE_UINT32,
            &pSurfaceProperties->sourceWidth);
    _ilm_setup_param(&param[5], DBUS_TYPE_UINT32,
            &pSurfaceProperties->sourceHeight);
    _ilm_setup_param(&param[6], DBUS_TYPE_UINT32, &pSurfaceProperties->destX);
    _ilm_setup_param(&param[7], DBUS_TYPE_UINT32, &pSurfaceProperties->destY);
    _ilm_setup_param(&param[8], DBUS_TYPE_UINT32, &pSurfaceProperties->destWidth);
    _ilm_setup_param(&param[9], DBUS_TYPE_UINT32,
            &pSurfaceProperties->destHeight);
    _ilm_setup_param(&param[10], DBUS_TYPE_UINT32,
            &pSurfaceProperties->orientation);
    _ilm_setup_param(&param[11], DBUS_TYPE_BOOLEAN,
            &pSurfaceProperties->visibility);

	/* Setup message to send */
	DBusMessage *message;
	message = _ilm_dbus_method_call(g_ilm_client->dbus_connection,"GetPropertiesOfSurface",param,1);
	if (!message)
	{
		  ILM_ERROR("ilm_getPropertiesOfSurface","IPC Method call not possible can't setup remote message\n");
		  return ILM_ERROR_ON_CONNECTION;
	}
	ILM_CHECK_METHOD_ERROR(message);

    // read the parameters
    returnValue = _ilm_get_dbus_basic_elements(message, 11, &param[1]);
    _ilm_close_dbus_method_call(message);

    return returnValue;
}

ilmErrorTypes ilm_getPropertiesOfLayer(t_ilm_uint layerID, struct ilmLayerProperties* pLayerProperties)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    if (!pLayerProperties)
    {
        return returnValue;
    }

    // Setup parameter to send
    t_ilm_param param[12];
    _ilm_setup_param(&param[0], DBUS_TYPE_UINT32, &layerID);
    _ilm_setup_param(&param[1], DBUS_TYPE_DOUBLE, &pLayerProperties->opacity);
    _ilm_setup_param(&param[2], DBUS_TYPE_UINT32, &pLayerProperties->sourceX);
    _ilm_setup_param(&param[3], DBUS_TYPE_UINT32, &pLayerProperties->sourceY);
    _ilm_setup_param(&param[4], DBUS_TYPE_UINT32, &pLayerProperties->sourceWidth);
    _ilm_setup_param(&param[5], DBUS_TYPE_UINT32,
            &pLayerProperties->sourceHeight);
    _ilm_setup_param(&param[6], DBUS_TYPE_UINT32, &pLayerProperties->destX);
    _ilm_setup_param(&param[7], DBUS_TYPE_UINT32, &pLayerProperties->destY);
    _ilm_setup_param(&param[8], DBUS_TYPE_UINT32, &pLayerProperties->destWidth);
    _ilm_setup_param(&param[9], DBUS_TYPE_UINT32, &pLayerProperties->destHeight);
    _ilm_setup_param(&param[10], DBUS_TYPE_UINT32,
            &pLayerProperties->orientation);
    _ilm_setup_param(&param[11], DBUS_TYPE_BOOLEAN,
            &pLayerProperties->visibility);

	/* Setup message to send */
	DBusMessage *message;
	message = _ilm_dbus_method_call(g_ilm_client->dbus_connection,"GetPropertiesOfLayer", param, 1);
	if (!message)
	{
		  ILM_ERROR("ilm_getPropertiesOfLayer","IPC Method call not possible can't setup remote message\n");
		  return ILM_ERROR_ON_CONNECTION;
	}
	ILM_CHECK_METHOD_ERROR(message);

    if (!message)
    {
        ILM_ERROR("ilm_getPropertiesOfLayer","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    ILM_CHECK_METHOD_ERROR(message);

    // read the parameters
    returnValue = _ilm_get_dbus_basic_elements(message, 11, &param[1]);
    _ilm_close_dbus_method_call(message);

    return returnValue;
}

ilmErrorTypes ilm_getNumberOfHardwareLayers(t_ilm_uint screenID, t_ilm_uint* pNumberOfHardwareLayers)
{
    ilmErrorTypes error = ILM_FAILED;

    // Setup parameter to send
    t_ilm_param screenParam[2];
    _ilm_setup_param(&screenParam[0], DBUS_TYPE_UINT32, &screenID);
    _ilm_setup_param(&screenParam[1], DBUS_TYPE_UINT32, pNumberOfHardwareLayers);

    // Setup message to send
    DBusMessage *message;
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetNumberOfHardwareLayers", screenParam, 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        // read the parameters
        error = _ilm_get_dbus_basic_elements(message, 1, &screenParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_getNumberOfHardwareLayers","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_getScreenResolution(t_ilm_uint screenID, t_ilm_uint* pWidth, t_ilm_uint* pHeight)
{
    ilmErrorTypes error = ILM_FAILED;

    // Setup parameter to send
    t_ilm_param screenParam[3];
    _ilm_setup_param(&screenParam[0], DBUS_TYPE_UINT32, &screenID);
    _ilm_setup_param(&screenParam[1], DBUS_TYPE_UINT32, pWidth);
    _ilm_setup_param(&screenParam[2], DBUS_TYPE_UINT32, pHeight);

    // Setup message to send
    DBusMessage *message;
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetScreenResolution", screenParam, 3);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        // read the parameters
        error = _ilm_get_dbus_basic_elements(message, 2, &screenParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_getScreenResolution","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_getLayerIDs(t_ilm_int* pLength, t_ilm_layer** ppArray)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "ListAllLayerIDS", NULL, 0);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        // read the parameters
        t_ilm_int arrayLen = 0;
        if (_ilm_get_dbus_array_length(message, DBUS_TYPE_UINT32, &arrayLen) == ILM_SUCCESS)
        {
            *pLength = arrayLen;
            *ppArray = malloc(sizeof(t_ilm_layer) * arrayLen);
            error = _ilm_get_dbus_array_elements(message, DBUS_TYPE_UINT32, *ppArray);
        }
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_getLayerIDs","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_getLayerIDsOnScreen(t_ilm_uint screenId, t_ilm_int* pLength, t_ilm_layer** ppArray)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;
    t_ilm_param param[1];

    _ilm_setup_param(&param[0], DBUS_TYPE_UINT32, &screenId);

    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "ListAllLayerIDsOnScreen", param, 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        // read the parameters
        t_ilm_int arrayLen = 0;
        if (_ilm_get_dbus_array_length(message, DBUS_TYPE_UINT32, &arrayLen) == ILM_SUCCESS)
        {
            *pLength = arrayLen;
            *ppArray = malloc(sizeof(t_ilm_layer) * arrayLen);
            error = _ilm_get_dbus_array_elements(message, DBUS_TYPE_UINT32, *ppArray);
        }
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_getLayerIDs","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_getSurfaceIDs(t_ilm_int* pLength, t_ilm_surface** ppArray)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection,
            "ListAllSurfaceIDS", NULL, 0);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        // read the parameters
        t_ilm_int arrayLen = 0;
        if (_ilm_get_dbus_array_length(message, DBUS_TYPE_UINT32, &arrayLen) == ILM_SUCCESS)
        {
            *pLength = arrayLen;
            *ppArray = malloc(sizeof(t_ilm_surface) * arrayLen);
            error = _ilm_get_dbus_array_elements(message, DBUS_TYPE_UINT32, *ppArray);
        }
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_getSurfaceIDs", "IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_getLayerGroupIDs(t_ilm_int* pLength, t_ilm_layergroup** ppArray)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "ListAllLayerGroupIDS", NULL, 0);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        // read the parameters
        t_ilm_int arrayLen = 0;
        if (_ilm_get_dbus_array_length(message, DBUS_TYPE_UINT32, &arrayLen) == ILM_SUCCESS)
        {
            *pLength = arrayLen;
            *ppArray = malloc(sizeof(t_ilm_int) * arrayLen);
            error = _ilm_get_dbus_array_elements(message, DBUS_TYPE_UINT32, *ppArray);
        }
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_getLayerGroupIDs","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_getSurfaceGroupIDs(t_ilm_int* pLength, t_ilm_surfacegroup** ppArray)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "ListAllSurfaceGroupIDS", NULL, 0);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        // read the parameters
        t_ilm_int arrayLen = 0;
        if (_ilm_get_dbus_array_length(message, DBUS_TYPE_UINT32, &arrayLen) == ILM_SUCCESS)
        {
            *pLength = arrayLen;
            *ppArray = malloc(sizeof(t_ilm_int) * arrayLen);
            error = _ilm_get_dbus_array_elements(message, DBUS_TYPE_UINT32, *ppArray);
        }
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_getSurfaceGroupIDs","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_getSurfaceIDsOnLayer(t_ilm_layer layer, t_ilm_int* pLength, t_ilm_surface** ppArray)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam;
    _ilm_setup_param(&layerParam, DBUS_TYPE_UINT32, &layer);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "ListSurfaceofLayer", &layerParam, 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        // read the parameters
        t_ilm_int arrayLen = 0;
        if (_ilm_get_dbus_array_length(message, DBUS_TYPE_UINT32, &arrayLen) == ILM_SUCCESS)
        {
            *pLength = arrayLen;
            *ppArray = malloc(sizeof(t_ilm_int) * arrayLen);
            error = _ilm_get_dbus_array_elements(message, DBUS_TYPE_UINT32, *ppArray);
        }
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_getSurfaceIDsOnLayer","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerCreate(t_ilm_layer* pLayerId)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[1];
    _ilm_setup_param(layerParam, DBUS_TYPE_UINT32, pLayerId);

    // Setup Call
    if (*pLayerId != INVALID_ID)
    {
        message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "CreateLayerFromId", layerParam, 1);
    }
    else
    {
        message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "CreateLayer", NULL, 0);
    }

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, layerParam);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerCreate","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerCreateWithDimension(t_ilm_layer* pLayerId, t_ilm_uint width, t_ilm_uint height)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup Call
    if (*pLayerId != INVALID_ID)
    {
        // Setup parameter to send
        t_ilm_param layerParam[3];
       _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, pLayerId);
       _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &width);
       _ilm_setup_param(&layerParam[2], DBUS_TYPE_UINT32, &height);
        message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "CreateLayerFromIdWithDimension", layerParam, 3);
    }
    else
    {
        t_ilm_param layerParam[2];
       _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &width);
       _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &height);
        message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "CreateLayerWithDimension", layerParam, 2);
    }

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        t_ilm_param layerParam[1];
       _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, pLayerId);
        error = _ilm_get_dbus_basic_elements(message, 1, layerParam);
       _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerCreate","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerRemove(t_ilm_layer layerId)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam;
    _ilm_setup_param(&layerParam, DBUS_TYPE_UINT32, &layerId);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "RemoveLayer", &layerParam, 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerRemove", "IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerAddSurface(t_ilm_layer layerId, t_ilm_surface surfaceId)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &layerId);

    // Call Method
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "AddSurfaceToLayer", &layerParam[0], 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerAddSurface","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerRemoveSurface(t_ilm_layer layerId, t_ilm_surface surfaceId)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &layerId);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "RemoveSurfaceFromLayer", layerParam, 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerRemoveSurface","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerGetType(t_ilm_layer layerId, ilmLayerType* pLayerType)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, pLayerType);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetLayerType", layerParam, 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, &layerParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerGetType","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerSetVisibility(t_ilm_layer layerId, t_ilm_bool newVisibility)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_BOOLEAN, &newVisibility);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetLayerVisibility", layerParam, 2);

    if (message)
    {
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerSetVisibility","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }
    return error;
}

ilmErrorTypes ilm_layerGetVisibility(t_ilm_layer layerId, t_ilm_bool *pVisibility)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_BOOLEAN, pVisibility);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetLayerVisibility", &layerParam[0], 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, &layerParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerGetVisibility","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }
    return error;
}

ilmErrorTypes ilm_layerSetOpacity(t_ilm_layer layerId, t_ilm_float opacity)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_DOUBLE, &opacity);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetLayerOpacity", layerParam, 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerSetOpacity","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerGetOpacity(t_ilm_layer layerId, t_ilm_float *pOpacity)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_DOUBLE, pOpacity);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetLayerOpacity", layerParam, 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, &layerParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerGetOpacity","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerSetSourceRectangle(t_ilm_layer layerId, t_ilm_uint x, t_ilm_uint y, t_ilm_uint width, t_ilm_uint height)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[5];

    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &x);
    _ilm_setup_param(&layerParam[2], DBUS_TYPE_UINT32, &y);
    _ilm_setup_param(&layerParam[3], DBUS_TYPE_UINT32, &width);
    _ilm_setup_param(&layerParam[4], DBUS_TYPE_UINT32, &height);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetLayerSourceRegion", layerParam, 5);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerSetSourceRectangle","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerSetDestinationRectangle(t_ilm_layer layerId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height)
{
    //printf("ilm_layerSetDestinationRectangle with %i %i %i %i %i\n", layerId, x, y, width, height);

    ilmErrorTypes error = ILM_FAILED;

    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[5];

    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &x);
    _ilm_setup_param(&layerParam[2], DBUS_TYPE_UINT32, &y);
    _ilm_setup_param(&layerParam[3], DBUS_TYPE_UINT32, &width);
    _ilm_setup_param(&layerParam[4], DBUS_TYPE_UINT32, &height);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetLayerDestinationRegion", &layerParam[0], 5);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerSetDestinationRectangle","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerGetDimension(t_ilm_layer layerId, t_ilm_uint *pDimension)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[3];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &pDimension[0]);
    _ilm_setup_param(&layerParam[2], DBUS_TYPE_UINT32, &pDimension[1]);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetLayerDimension", &layerParam[0], 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 2, &layerParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerGetDimension","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerSetDimension(t_ilm_layer layerId, t_ilm_uint *pDimension)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[3];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &pDimension[0]);
    _ilm_setup_param(&layerParam[2], DBUS_TYPE_UINT32, &pDimension[1]);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetLayerDimension", &layerParam[0], 3);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerSetDimension","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerGetPosition(t_ilm_layer layerId, t_ilm_uint *pPosition)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[3];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &pPosition[0]);
    _ilm_setup_param(&layerParam[2], DBUS_TYPE_UINT32, &pPosition[1]);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetLayerPosition", &layerParam[0], 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 2, &layerParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerGetPosition","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerSetPosition(t_ilm_layer layerId, t_ilm_uint *pPosition)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[3];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &pPosition[0]);
    _ilm_setup_param(&layerParam[2], DBUS_TYPE_UINT32, &pPosition[1]);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetLayerPosition", &layerParam[0], 3);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerSetPosition","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerSetOrientation(t_ilm_layer layerId, ilmOrientation orientation)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &orientation);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetLayerOrientation", &layerParam[0], 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerSetOrientation","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerGetOrientation(t_ilm_layer layerId, ilmOrientation *pOrientation)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;
    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, pOrientation);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetLayerOrientation", &layerParam[0], 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, &layerParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerGetOrientation","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerSetChromaKey(t_ilm_layer layerId, t_ilm_int* color)
{
    ilmErrorTypes error = ILM_FAILED;
    // TODO: Implement this on both server and client
    return error;
}

ilmErrorTypes ilm_layerSetRenderOrder(t_ilm_layer layerId, t_ilm_layer *pSurfaceId, t_ilm_int number)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_array(&layerParam[1], DBUS_TYPE_UINT32, number, pSurfaceId);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetSurfaceRenderOrderWithinLayer", &layerParam[0], 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerSetRenderOrder","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;

}

ilmErrorTypes ilm_layerGetCapabilities(t_ilm_layer layerId, t_ilm_layercapabilities *pCapabilities)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, pCapabilities);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetLayerCapabilities", &layerParam[0], 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, &layerParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerGetCapabilities","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layerTypeGetCapabilities(ilmLayerType layerType, t_ilm_layercapabilities *pCapabilities)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layerType);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, pCapabilities);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetLayertypeCapabilities", layerParam, 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, &layerParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerTypeGetCapabilities","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layergroupCreate(t_ilm_layergroup *pLayergroup)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[1];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, pLayergroup);
    if (*pLayergroup != INVALID_ID)
    {
        message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "CreateLayerGroupFromId", layerParam, 1);
    }
    else
    {
        message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "CreateLayerGroup", NULL, 0);
    }

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, &layerParam[0]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layergroupCreate","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layergroupRemove(t_ilm_layergroup group)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam;
    _ilm_setup_param(&layerParam, DBUS_TYPE_UINT32, &group);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "RemoveLayerGroup", &layerParam, 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layergroupRemove","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layergroupAddLayer(t_ilm_layergroup group, t_ilm_layer layer)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layer);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &group);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "AddLayerToLayerGroup", &layerParam[0], 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layergroupAddLayer","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layergroupRemoveLayer(t_ilm_layergroup group, t_ilm_layer layer)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &layer);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &group);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "RemoveLayerFromLayerGroup", &layerParam[0], 2);
    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layergroupRemoveLayer","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layergroupSetVisibility(t_ilm_layergroup group, t_ilm_bool newVisibility)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &group);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_BOOLEAN, &newVisibility);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetLayergroupVisibility", &layerParam[0], 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layergroupSetVisibility","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_layergroupSetOpacity(t_ilm_layergroup group, t_ilm_float opacity)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;
    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &group);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_DOUBLE, &opacity);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetLayergroupOpacity", layerParam, 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layergroupSetOpacity","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceCreate(t_ilm_nativehandle nativehandle, t_ilm_int width, t_ilm_int height, ilmPixelFormat pixelFormat, t_ilm_surface* pSurfaceId)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[5];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &nativehandle);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_UINT32, &width);
    _ilm_setup_param(&surfaceParam[2], DBUS_TYPE_UINT32, &height);
    _ilm_setup_param(&surfaceParam[3], DBUS_TYPE_UINT32, &pixelFormat);
    _ilm_setup_param(&surfaceParam[4], DBUS_TYPE_UINT32, pSurfaceId);

    if (*pSurfaceId == INVALID_ID)
    {
        // Setup Call
        message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "CreateSurface", &surfaceParam[0], 4);
    }
    else
    {
        message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "CreateSurfaceFromId", &surfaceParam[0], 5);
    }

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, &surfaceParam[4]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceCreate","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceRemove(t_ilm_surface surfaceId)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam;
    _ilm_setup_param(&surfaceParam, DBUS_TYPE_UINT32, &surfaceId);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "RemoveSurface", &surfaceParam, 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceRemove","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceSetVisibility(t_ilm_surface surfaceId, t_ilm_bool newVisibility)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[2];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_BOOLEAN, &newVisibility);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetSurfaceVisibility", surfaceParam, 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceSetVisibility","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceGetVisibility(t_ilm_surface surfaceId, t_ilm_bool *pVisibility)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[2];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_BOOLEAN, pVisibility);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetSurfaceVisibility", &surfaceParam[0], 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, &surfaceParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceSetVisibility","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}
ilmErrorTypes ilm_surfaceSetOpacity(t_ilm_surface surfaceId, t_ilm_float opacity)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[2];

    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_DOUBLE, &opacity);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetSurfaceOpacity", &surfaceParam[0], 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceSetOpacity","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;

}

ilmErrorTypes ilm_surfaceGetOpacity(t_ilm_surface surfaceId, t_ilm_float *pOpacity)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[2];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_DOUBLE, pOpacity);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetSurfaceOpacity", &surfaceParam[0], 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, &surfaceParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceGetOpacity","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceSetSourceRectangle(t_ilm_surface surfaceId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[5];

    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &x);
    _ilm_setup_param(&layerParam[2], DBUS_TYPE_UINT32, &y);
    _ilm_setup_param(&layerParam[3], DBUS_TYPE_UINT32, &width);
    _ilm_setup_param(&layerParam[4], DBUS_TYPE_UINT32, &height);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetSurfaceSourceRegion", layerParam, 5);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_layerSetSourceRectangle","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceSetDestinationRectangle(t_ilm_surface surfaceId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[5];

    _ilm_setup_param(&layerParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &x);
    _ilm_setup_param(&layerParam[2], DBUS_TYPE_UINT32, &y);
    _ilm_setup_param(&layerParam[3], DBUS_TYPE_UINT32, &width);
    _ilm_setup_param(&layerParam[4], DBUS_TYPE_UINT32, &height);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetSurfaceDestinationRegion", &layerParam[0], 5);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceSetDestinationRectangle","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceGetDimension(t_ilm_surface surfaceId, t_ilm_uint *pDimension)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[3];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_UINT32, &pDimension[0]);
    _ilm_setup_param(&surfaceParam[2], DBUS_TYPE_UINT32, &pDimension[1]);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetSurfaceDimension", &surfaceParam[0], 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 2, &surfaceParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceGetDimension","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceSetDimension(t_ilm_surface surfaceId, t_ilm_uint *pDimension)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[3];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_UINT32, &pDimension[0]);
    _ilm_setup_param(&surfaceParam[2], DBUS_TYPE_UINT32, &pDimension[1]);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetSurfaceDimension", &surfaceParam[0], 3);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceSetDimension","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceGetPosition(t_ilm_surface surfaceId, t_ilm_uint *pPosition)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[3];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_UINT32, &pPosition[0]);
    _ilm_setup_param(&surfaceParam[2], DBUS_TYPE_UINT32, &pPosition[1]);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetSurfacePosition", &surfaceParam[0], 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 2, &surfaceParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceGetPosition","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceSetPosition(t_ilm_surface surfaceId, t_ilm_uint *pPosition)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[3];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_UINT32, &pPosition[0]);
    _ilm_setup_param(&surfaceParam[2], DBUS_TYPE_UINT32, &pPosition[1]);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetSurfacePosition", &surfaceParam[0], 3);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceSetPosition","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceSetOrientation(t_ilm_surface surfaceId, ilmOrientation orientation)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[2];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_UINT32, &orientation);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetSurfaceOrientation", &surfaceParam[0], 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceSetOrientation","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceGetOrientation(t_ilm_surface surfaceId, ilmOrientation *pOrientation)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[2];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_UINT32, pOrientation);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetSurfaceOrientation", &surfaceParam[0], 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, &surfaceParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceGetOrientation","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceGetPixelformat(t_ilm_layer surfaceId, ilmPixelFormat *pPixelformat)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[2];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &surfaceId);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_UINT32, pPixelformat);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetSurfacePixelformat", &surfaceParam[0], 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, &surfaceParam[1]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfaceGetPixelformat","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfaceSetChromaKey(t_ilm_surface surfaceId, t_ilm_int* pColor)
{
    ilmErrorTypes error = ILM_FAILED;

    // TODO: Implement this on both server and client

    return error;
}

ilmErrorTypes ilm_surfaceInvalidateRectangle(t_ilm_surface surfaceId)
{
    ilmErrorTypes error = ILM_FAILED;

    // TODO: Implement this on both server and client

    return error;
}

ilmErrorTypes ilm_surfacegroupCreate(t_ilm_surfacegroup *pSurfacegroup)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[1];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, pSurfacegroup);

    // Setup Call
    if (*pSurfacegroup != INVALID_ID)
    {
        message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "CreateSurfaceGroupFromId", surfaceParam, 1);
    }
    else
    {
        message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "CreateSurfaceGroup", surfaceParam, 0);
    }

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = _ilm_get_dbus_basic_elements(message, 1, &surfaceParam[0]);
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfacegroupCreate","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfacegroupRemove(t_ilm_surfacegroup group)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam;
    _ilm_setup_param(&surfaceParam, DBUS_TYPE_UINT32, &group);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "RemoveSurfaceGroup", &surfaceParam, 1);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfacegroupRemove","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfacegroupAddSurface(t_ilm_surfacegroup group, t_ilm_surface surface)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[2];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &surface);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_UINT32, &group);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "AddSurfaceToSurfaceGroup", &surfaceParam[0], 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfacegroupAddSurface","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfacegroupRemoveSurface(t_ilm_surfacegroup group, t_ilm_surface surface)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[2];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &surface);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_UINT32, &group);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "RemoveSurfaceFromSurfaceGroup", &surfaceParam[0], 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfacegroupRemoveSurface","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfacegroupSetVisibility(t_ilm_surfacegroup group, t_ilm_bool newVisibility)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[2];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &group);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_BOOLEAN, &newVisibility);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetSurfacegroupVisibility", &surfaceParam[0], 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfacegroupSetVisibility","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_surfacegroupSetOpacity(t_ilm_surfacegroup group, t_ilm_float opacity)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param surfaceParam[2];
    _ilm_setup_param(&surfaceParam[0], DBUS_TYPE_UINT32, &group);
    _ilm_setup_param(&surfaceParam[1], DBUS_TYPE_DOUBLE, &opacity);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetSurfacegroupOpacity", surfaceParam, 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_surfacegroupSetOpacity","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_displaySetRenderOrder(t_ilm_display display, t_ilm_layer *pLayerId, const t_ilm_uint number)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param layerParam[2];
    _ilm_setup_array(&layerParam[0], DBUS_TYPE_UINT32, number, pLayerId);
    _ilm_setup_param(&layerParam[1], DBUS_TYPE_UINT32, &display);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "SetRenderOrderOfLayers", layerParam, 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_displaySetRenderOrder","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_getScreenIDs(t_ilm_uint* pNumberOfIDs, t_ilm_uint** ppIDs)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "GetScreenIDs", NULL, 0);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);

        // read the parameters
        t_ilm_int arrayLen = 0;
        if (_ilm_get_dbus_array_length(message, DBUS_TYPE_UINT32, &arrayLen) == ILM_SUCCESS)
        {
            *pNumberOfIDs = arrayLen;
            *ppIDs = malloc(sizeof(t_ilm_int) * arrayLen);
            error = _ilm_get_dbus_array_elements(message, DBUS_TYPE_UINT32,
                    *ppIDs);
        }
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_getScreenIDsr","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_takeScreenshot(t_ilm_uint screen, t_ilm_const_string filename)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param param[2];
    _ilm_setup_param(&param[0], DBUS_TYPE_UINT32, &screen);
    _ilm_setup_param(&param[1], DBUS_TYPE_STRING, &filename);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "ScreenShot", param, 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_doScreenShot","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_takeLayerScreenshot(t_ilm_const_string filename, t_ilm_layer layerid)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param param[2];
    _ilm_setup_param(&param[0], DBUS_TYPE_STRING, &filename);
    _ilm_setup_param(&param[1], DBUS_TYPE_UINT32, &layerid);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "ScreenShotOfLayer", param, 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_takeLayerScreenshot","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_takeSurfaceScreenshot(t_ilm_const_string filename, t_ilm_surface surfaceid)
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup parameter to send
    t_ilm_param param[2];
    _ilm_setup_param(&param[0], DBUS_TYPE_STRING, &filename);
    _ilm_setup_param(&param[1], DBUS_TYPE_UINT32, &surfaceid);

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "ScreenShotOfSurface", param, 2);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_takeLayerScreenshot","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_commitChanges()
{
    ilmErrorTypes error = ILM_FAILED;
    DBusMessage *message;

    // Setup Call
    message = _ilm_dbus_method_call(g_ilm_client->dbus_connection, "CommitChanges", NULL, 0);

    if (message)
    {
        ILM_CHECK_METHOD_ERROR(message);
        error = ILM_SUCCESS;
        _ilm_close_dbus_method_call(message);
    }
    else
    {
        ILM_ERROR("ilm_commitChanges","IPC Method call not possible can't setup remote message\n");
        return ILM_ERROR_ON_CONNECTION;
    }

    return error;
}

ilmErrorTypes ilm_destroy()
{
    ilmErrorTypes result = ILM_FAILED;

    if (g_ilm_client && g_ilm_init == ILM_TRUE)
    {
        if (g_ilm_client->dbus_connection)
        {
            dbus_connection_unref(g_ilm_client->dbus_connection);
        }
        dbus_error_free(&g_ilm_client->dbus_error);
        free(g_ilm_client);
        g_ilm_init = ILM_FALSE;
        result = ILM_SUCCESS;
    }
    return result;
}

#undef _ILM_CLIENT_C_
