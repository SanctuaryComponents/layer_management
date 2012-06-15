/**************************************************************************
 *
 * Copyright 2012 BMW Car IT GmbH
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License\0");
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
#include "ilm_client.h"
#include "ilm_types.h"
#include "IpcModuleLoader.h"
#include <stdio.h>

//=============================================================================
// global settings
//=============================================================================
const int gReceiveTimeout = 100; // in ms

// must be same as GraphicalObject::INVALID_ID, but this is defined in C++
// and can not be used here
#define INVALID_ID 0xFFFFFFFF

//=============================================================================
// global vars
//=============================================================================
struct IpcModule gIpcModule;


//=============================================================================
// logging
//=============================================================================
//#define LOG_ENTER_FUNCTION printf("--> ilmClient::%s\n", __PRETTY_FUNCTION__)
#define LOG_ENTER_FUNCTION


//=============================================================================
// implementation
//=============================================================================
ilmErrorTypes ilm_init()
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes result = ILM_FAILED;
    if (loadIpcModule(&gIpcModule))
    {
        int pid = getpid();

        if (gIpcModule.init(ILM_TRUE)
            && gIpcModule.createMessage("ServiceConnect")
            && gIpcModule.appendUint(pid)
            && gIpcModule.sendMessage()
            && gIpcModule.receiveMessage(gReceiveTimeout))
        {
            result = ILM_SUCCESS;
        }
    }

    return result;
}

ilmErrorTypes ilm_destroy()
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes result = ILM_FAILED;
    t_ilm_int pid = getpid();

    if (gIpcModule.createMessage("ServiceDisconnect")
        && gIpcModule.appendUint(pid)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && gIpcModule.destroy())
    {
        result = ILM_SUCCESS;
    }

    return result;
}

ilmErrorTypes ilm_getPropertiesOfSurface(t_ilm_uint surfaceID, struct ilmSurfaceProperties* pSurfaceProperties)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pSurfaceProperties
        && gIpcModule.createMessage("GetPropertiesOfSurface\0")
        && gIpcModule.appendUint(surfaceID)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getDouble(&pSurfaceProperties->opacity)
        && gIpcModule.getUint(&pSurfaceProperties->sourceX)
        && gIpcModule.getUint(&pSurfaceProperties->sourceY)
        && gIpcModule.getUint(&pSurfaceProperties->sourceWidth)
        && gIpcModule.getUint(&pSurfaceProperties->sourceHeight)
        && gIpcModule.getUint(&pSurfaceProperties->origSourceWidth)
        && gIpcModule.getUint(&pSurfaceProperties->origSourceHeight)
        && gIpcModule.getUint(&pSurfaceProperties->destX)
        && gIpcModule.getUint(&pSurfaceProperties->destY)
        && gIpcModule.getUint(&pSurfaceProperties->destWidth)
        && gIpcModule.getUint(&pSurfaceProperties->destHeight)
        && gIpcModule.getUint(&pSurfaceProperties->orientation)
        && gIpcModule.getBool(&pSurfaceProperties->visibility)
        && gIpcModule.getUint(&pSurfaceProperties->frameCounter))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_getPropertiesOfLayer(t_ilm_uint layerID, struct ilmLayerProperties* pLayerProperties)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLayerProperties
        && gIpcModule.createMessage("GetPropertiesOfLayer\0")
        && gIpcModule.appendUint(layerID)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getDouble(&pLayerProperties->opacity)
        && gIpcModule.getUint(&pLayerProperties->sourceX)
        && gIpcModule.getUint(&pLayerProperties->sourceY)
        && gIpcModule.getUint(&pLayerProperties->sourceWidth)
        && gIpcModule.getUint(&pLayerProperties->sourceHeight)
        && gIpcModule.getUint(&pLayerProperties->origSourceWidth)
        && gIpcModule.getUint(&pLayerProperties->origSourceHeight)
        && gIpcModule.getUint(&pLayerProperties->destX)
        && gIpcModule.getUint(&pLayerProperties->destY)
        && gIpcModule.getUint(&pLayerProperties->destWidth)
        && gIpcModule.getUint(&pLayerProperties->destHeight)
        && gIpcModule.getUint(&pLayerProperties->orientation)
        && gIpcModule.getBool(&pLayerProperties->visibility))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_getNumberOfHardwareLayers(t_ilm_uint screenID, t_ilm_uint* pNumberOfHardwareLayers)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pNumberOfHardwareLayers
        && gIpcModule.createMessage("GetNumberOfHardwareLayers\0")
        && gIpcModule.appendUint(screenID)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUint(pNumberOfHardwareLayers))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_getScreenResolution(t_ilm_uint screenID, t_ilm_uint* pWidth, t_ilm_uint* pHeight)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pWidth && pHeight
        && gIpcModule.createMessage("GetScreenResolution\0")
        && gIpcModule.appendUint(screenID)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUint(pWidth)
        && gIpcModule.getUint(pHeight))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_getLayerIDs(t_ilm_int* pLength, t_ilm_layer** ppArray)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLength && ppArray
        && gIpcModule.createMessage("ListAllLayerIDS\0\0")
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUintArray(ppArray, pLength))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_getLayerIDsOnScreen(t_ilm_uint screenId, t_ilm_int* pLength, t_ilm_layer** ppArray)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLength && ppArray
        && gIpcModule.createMessage("ListAllLayerIDsOnScreen\0")
        && gIpcModule.appendUint(screenId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUintArray(ppArray, pLength))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_getSurfaceIDs(t_ilm_int* pLength, t_ilm_surface** ppArray)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLength && ppArray
        && gIpcModule.createMessage("ListAllSurfaceIDS\0")
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUintArray(ppArray, pLength))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_getLayerGroupIDs(t_ilm_int* pLength, t_ilm_layergroup** ppArray)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLength && ppArray
        && gIpcModule.createMessage("ListAllLayerGroupIDS\0")
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUintArray(ppArray, pLength))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_getSurfaceGroupIDs(t_ilm_int* pLength, t_ilm_surfacegroup** ppArray)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLength && ppArray
        && gIpcModule.createMessage("ListAllSurfaceGroupIDS\0")
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUintArray(ppArray, pLength))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_getSurfaceIDsOnLayer(t_ilm_layer layer, t_ilm_int* pLength, t_ilm_surface** ppArray)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLength && ppArray
        && gIpcModule.createMessage("ListSurfaceofLayer\0")
        && gIpcModule.appendUint(layer)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUintArray(ppArray, pLength))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerCreate(t_ilm_layer* pLayerId)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLayerId && (INVALID_ID != *pLayerId))
    {
        if (gIpcModule.createMessage("CreateLayerFromId\0")
            && gIpcModule.appendUint(*pLayerId)
            && gIpcModule.sendMessage()
            && gIpcModule.receiveMessage(gReceiveTimeout)
            && !gIpcModule.isErrorMessage()
            && gIpcModule.getUint(pLayerId))
        {
            returnValue = ILM_SUCCESS;
        }
    }
    else
    {
        if (gIpcModule.createMessage("CreateLayer\0")
            && gIpcModule.sendMessage()
            && gIpcModule.receiveMessage(gReceiveTimeout)
            && !gIpcModule.isErrorMessage()
            && gIpcModule.getUint(pLayerId))
        {
            returnValue = ILM_SUCCESS;
        }
    }

    return returnValue;
}

ilmErrorTypes ilm_layerCreateWithDimension(t_ilm_layer* pLayerId, t_ilm_uint width, t_ilm_uint height)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLayerId && (INVALID_ID != *pLayerId))
    {
        if (gIpcModule.createMessage("CreateLayerFromIdWithDimension\0")
            && gIpcModule.appendUint(*pLayerId)
            && gIpcModule.appendUint(width)
            && gIpcModule.appendUint(height)
            && gIpcModule.sendMessage()
            && gIpcModule.receiveMessage(gReceiveTimeout)
            && !gIpcModule.isErrorMessage()
            && gIpcModule.getUint(pLayerId))
        {
            returnValue = ILM_SUCCESS;
        }
    }
    else
    {
        if (gIpcModule.createMessage("CreateLayerWithDimension\0")
            && gIpcModule.appendUint(width)
            && gIpcModule.appendUint(height)
            && gIpcModule.sendMessage()
            && gIpcModule.receiveMessage(gReceiveTimeout)
            && !gIpcModule.isErrorMessage()
            && gIpcModule.getUint(pLayerId))
        {
            returnValue = ILM_SUCCESS;
        }
    }

    return returnValue;
}

ilmErrorTypes ilm_layerRemove(t_ilm_layer layerId)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("RemoveLayer\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerAddSurface(t_ilm_layer layerId, t_ilm_surface surfaceId)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("AddSurfaceToLayer\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.appendUint(layerId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerRemoveSurface(t_ilm_layer layerId, t_ilm_surface surfaceId)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("RemoveSurfaceFromLayer\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.appendUint(layerId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerGetType(t_ilm_layer layerId, ilmLayerType* pLayerType)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLayerType
        && gIpcModule.createMessage("GetLayerType\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUint(pLayerType))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerSetVisibility(t_ilm_layer layerId, t_ilm_bool newVisibility)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetLayerVisibility\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.appendBool(newVisibility)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerGetVisibility(t_ilm_layer layerId, t_ilm_bool *pVisibility)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pVisibility
        && gIpcModule.createMessage("GetLayerVisibility\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getBool(pVisibility))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerSetOpacity(t_ilm_layer layerId, t_ilm_float opacity)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetLayerOpacity\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.appendDouble(opacity)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerGetOpacity(t_ilm_layer layerId, t_ilm_float *pOpacity)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pOpacity
        && gIpcModule.createMessage("GetLayerOpacity\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getDouble(pOpacity))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerSetSourceRectangle(t_ilm_layer layerId, t_ilm_uint x, t_ilm_uint y, t_ilm_uint width, t_ilm_uint height)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetLayerSourceRegion\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.appendUint(x)
        && gIpcModule.appendUint(y)
        && gIpcModule.appendUint(width)
        && gIpcModule.appendUint(height)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerSetDestinationRectangle(t_ilm_layer layerId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetLayerDestinationRegion\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.appendUint(x)
        && gIpcModule.appendUint(y)
        && gIpcModule.appendUint(width)
        && gIpcModule.appendUint(height)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerGetDimension(t_ilm_layer layerId, t_ilm_uint *pDimension)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pDimension
        && gIpcModule.createMessage("GetLayerDimension\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUint(&pDimension[0])
        && gIpcModule.getUint(&pDimension[1]))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerSetDimension(t_ilm_layer layerId, t_ilm_uint *pDimension)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pDimension
        && gIpcModule.createMessage("SetLayerDimension\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.appendUint(pDimension[0])
        && gIpcModule.appendUint(pDimension[1])
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerGetPosition(t_ilm_layer layerId, t_ilm_uint *pPosition)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pPosition
        && gIpcModule.createMessage("GetLayerPosition\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUint(&pPosition[0])
        && gIpcModule.getUint(&pPosition[1]))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerSetPosition(t_ilm_layer layerId, t_ilm_uint *pPosition)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pPosition
        && gIpcModule.createMessage("SetLayerPosition\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.appendUint(pPosition[0])
        && gIpcModule.appendUint(pPosition[1])
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerSetOrientation(t_ilm_layer layerId, ilmOrientation orientation)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetLayerOrientation\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.appendUint(orientation)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerGetOrientation(t_ilm_layer layerId, ilmOrientation *pOrientation)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pOrientation
        && gIpcModule.createMessage("GetLayerOrientation\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUint(pOrientation))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerSetChromaKey(t_ilm_layer layerId, t_ilm_int* color)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes error = ILM_FAILED;
    // TODO: Implement this on both server and client
    return error;
}

ilmErrorTypes ilm_layerSetRenderOrder(t_ilm_layer layerId, t_ilm_layer *pSurfaceId, t_ilm_int number)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pSurfaceId
        && gIpcModule.createMessage("SetSurfaceRenderOrderWithinLayer\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.appendUintArray(pSurfaceId, number)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerGetCapabilities(t_ilm_layer layerId, t_ilm_layercapabilities *pCapabilities)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pCapabilities
        && gIpcModule.createMessage("GetLayerCapabilities\0")
        && gIpcModule.appendUint(layerId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUint(pCapabilities))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layerTypeGetCapabilities(ilmLayerType layerType, t_ilm_layercapabilities *pCapabilities)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pCapabilities
        && gIpcModule.createMessage("GetLayertypeCapabilities\0")
        && gIpcModule.appendUint(layerType)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUint(pCapabilities))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layergroupCreate(t_ilm_layergroup *pLayergroup)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLayergroup && (INVALID_ID != *pLayergroup))
    {
        if (pLayergroup
            && gIpcModule.createMessage("CreateLayerGroupFromId\0")
            && gIpcModule.appendUint(*pLayergroup)
            && gIpcModule.sendMessage()
            && gIpcModule.receiveMessage(gReceiveTimeout)
            && !gIpcModule.isErrorMessage()
            && gIpcModule.getUint(pLayergroup))
        {
            returnValue = ILM_SUCCESS;
        }
    }
    else
    {
        if (pLayergroup
            && gIpcModule.createMessage("CreateLayerGroup\0")
            && gIpcModule.sendMessage()
            && gIpcModule.receiveMessage(gReceiveTimeout)
            && !gIpcModule.isErrorMessage()
            && gIpcModule.getUint(pLayergroup))
        {
            returnValue = ILM_SUCCESS;
        }
    }

    return returnValue;
}

ilmErrorTypes ilm_layergroupRemove(t_ilm_layergroup group)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("RemoveLayerGroup\0")
        && gIpcModule.appendUint(group)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layergroupAddLayer(t_ilm_layergroup group, t_ilm_layer layer)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("AddLayerToLayerGroup\0")
        && gIpcModule.appendUint(layer)
        && gIpcModule.appendUint(group)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layergroupRemoveLayer(t_ilm_layergroup group, t_ilm_layer layer)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("RemoveLayerFromLayerGroup\0")
        && gIpcModule.appendUint(layer)
        && gIpcModule.appendUint(group)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layergroupSetVisibility(t_ilm_layergroup group, t_ilm_bool newVisibility)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetLayergroupVisibility\0")
        && gIpcModule.appendUint(group)
        && gIpcModule.appendBool(newVisibility)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_layergroupSetOpacity(t_ilm_layergroup group, t_ilm_float opacity)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetLayergroupOpacity\0")
        && gIpcModule.appendUint(group)
        && gIpcModule.appendDouble(opacity)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceCreate(t_ilm_nativehandle nativehandle, t_ilm_int width, t_ilm_int height, ilmPixelFormat pixelFormat, t_ilm_surface* pSurfaceId)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pSurfaceId && (INVALID_ID != *pSurfaceId))
    {
        if (gIpcModule.createMessage("CreateSurfaceFromId\0")
            && gIpcModule.appendUint(nativehandle)
            && gIpcModule.appendUint(width)
            && gIpcModule.appendUint(height)
            && gIpcModule.appendUint(pixelFormat)
            && gIpcModule.appendUint(*pSurfaceId)
            && gIpcModule.sendMessage()
            && gIpcModule.receiveMessage(gReceiveTimeout)
            && !gIpcModule.isErrorMessage()
            && gIpcModule.getUint(pSurfaceId))
        {
            returnValue = ILM_SUCCESS;
        }
    }
    else
    {
        if (gIpcModule.createMessage("CreateSurface\0")
            && gIpcModule.appendUint(nativehandle)
            && gIpcModule.appendUint(width)
            && gIpcModule.appendUint(height)
            && gIpcModule.appendUint(pixelFormat)
            && gIpcModule.sendMessage()
            && gIpcModule.receiveMessage(gReceiveTimeout)
            && !gIpcModule.isErrorMessage()
            && gIpcModule.getUint(pSurfaceId))
        {
            returnValue = ILM_SUCCESS;
        }
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceInitialize(t_ilm_surface *pSurfaceId)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pSurfaceId && (INVALID_ID != *pSurfaceId))
    {
        if (gIpcModule.createMessage("InitializeSurfaceFromId\0")
            && gIpcModule.appendUint(*pSurfaceId)
            && gIpcModule.sendMessage()
            && gIpcModule.receiveMessage(gReceiveTimeout)
            && !gIpcModule.isErrorMessage()
            && gIpcModule.getUint(pSurfaceId))
        {
            returnValue = ILM_SUCCESS;
        }
    }
    else
    {
        if (gIpcModule.createMessage("InitializeSurface\0")
            && gIpcModule.sendMessage()
            && gIpcModule.receiveMessage(gReceiveTimeout)
            && !gIpcModule.isErrorMessage()
            && gIpcModule.getUint(pSurfaceId))
        {
            returnValue = ILM_SUCCESS;
        }
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceSetNativeContent(t_ilm_nativehandle nativehandle, t_ilm_int width, t_ilm_int height, ilmPixelFormat pixelFormat, t_ilm_surface surfaceId)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetSurfaceNativeContent\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.appendUint(nativehandle)
        && gIpcModule.appendUint(width)
        && gIpcModule.appendUint(height)
        && gIpcModule.appendUint(pixelFormat)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceRemove(t_ilm_surface surfaceId)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("RemoveSurface\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceSetVisibility(t_ilm_surface surfaceId, t_ilm_bool newVisibility)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetSurfaceVisibility\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.appendBool(newVisibility)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceGetVisibility(t_ilm_surface surfaceId, t_ilm_bool *pVisibility)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pVisibility
        && gIpcModule.createMessage("GetSurfaceVisibility\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getBool(pVisibility))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceSetOpacity(t_ilm_surface surfaceId, t_ilm_float opacity)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetSurfaceOpacity\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.appendDouble(opacity)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceGetOpacity(t_ilm_surface surfaceId, t_ilm_float *pOpacity)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pOpacity
        && gIpcModule.createMessage("GetSurfaceOpacity\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getDouble(pOpacity))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceSetSourceRectangle(t_ilm_surface surfaceId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetSurfaceSourceRegion\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.appendUint(x)
        && gIpcModule.appendUint(y)
        && gIpcModule.appendUint(width)
        && gIpcModule.appendUint(height)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceSetDestinationRectangle(t_ilm_surface surfaceId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetSurfaceDestinationRegion\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.appendUint(x)
        && gIpcModule.appendUint(y)
        && gIpcModule.appendUint(width)
        && gIpcModule.appendUint(height)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceGetDimension(t_ilm_surface surfaceId, t_ilm_uint *pDimension)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pDimension
        && gIpcModule.createMessage("GetSurfaceDimension\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUint(&pDimension[0])
        && gIpcModule.getUint(&pDimension[1]))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceSetDimension(t_ilm_surface surfaceId, t_ilm_uint *pDimension)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pDimension
        && gIpcModule.createMessage("SetSurfaceDimension\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.appendUint(pDimension[0])
        && gIpcModule.appendUint(pDimension[1])
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceGetPosition(t_ilm_surface surfaceId, t_ilm_uint *pPosition)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pPosition
        && gIpcModule.createMessage("GetSurfacePosition\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUint(&pPosition[0])
        && gIpcModule.getUint(&pPosition[1]))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceSetPosition(t_ilm_surface surfaceId, t_ilm_uint *pPosition)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pPosition
        && gIpcModule.createMessage("SetSurfacePosition\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.appendUint(pPosition[0])
        && gIpcModule.appendUint(pPosition[1])
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceSetOrientation(t_ilm_surface surfaceId, ilmOrientation orientation)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetSurfaceOrientation\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.appendUint(orientation)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceGetOrientation(t_ilm_surface surfaceId, ilmOrientation *pOrientation)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pOrientation
        && gIpcModule.createMessage("GetSurfaceOrientation\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUint(pOrientation))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceGetPixelformat(t_ilm_layer surfaceId, ilmPixelFormat *pPixelformat)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pPixelformat
        && gIpcModule.createMessage("GetSurfacePixelformat\0")
        && gIpcModule.appendUint(surfaceId)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUint(pPixelformat))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfaceSetChromaKey(t_ilm_surface surfaceId, t_ilm_int* pColor)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes error = ILM_FAILED;

    // TODO: Implement this on both server and client

    return error;
}

ilmErrorTypes ilm_surfaceInvalidateRectangle(t_ilm_surface surfaceId)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes error = ILM_FAILED;

    // TODO: Implement this on both server and client

    return error;
}

ilmErrorTypes ilm_surfacegroupCreate(t_ilm_surfacegroup *pSurfacegroup)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pSurfacegroup && (INVALID_ID != *pSurfacegroup))
    {
        if (gIpcModule.createMessage("CreateSurfaceGroupFromId\0")
            && gIpcModule.appendUint(*pSurfacegroup)
            && gIpcModule.sendMessage()
            && gIpcModule.receiveMessage(gReceiveTimeout)
            && !gIpcModule.isErrorMessage()
            && gIpcModule.getUint(pSurfacegroup))
        {
            returnValue = ILM_SUCCESS;
        }
    }
    else
    {
        if (pSurfacegroup
            && gIpcModule.createMessage("CreateSurfaceGroup\0")
            && gIpcModule.sendMessage()
            && gIpcModule.receiveMessage(gReceiveTimeout)
            && !gIpcModule.isErrorMessage()
            && gIpcModule.getUint(pSurfacegroup))
        {
            returnValue = ILM_SUCCESS;
        }
    }

    return returnValue;
}

ilmErrorTypes ilm_surfacegroupRemove(t_ilm_surfacegroup group)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("RemoveSurfaceGroup\0")
        && gIpcModule.appendUint(group)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfacegroupAddSurface(t_ilm_surfacegroup group, t_ilm_surface surface)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("AddSurfaceToSurfaceGroup\0")
        && gIpcModule.appendUint(surface)
        && gIpcModule.appendUint(group)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfacegroupRemoveSurface(t_ilm_surfacegroup group, t_ilm_surface surface)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("RemoveSurfaceFromSurfaceGroup\0")
        && gIpcModule.appendUint(surface)
        && gIpcModule.appendUint(group)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfacegroupSetVisibility(t_ilm_surfacegroup group, t_ilm_bool newVisibility)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetSurfacegroupVisibility\0")
        && gIpcModule.appendUint(group)
        && gIpcModule.appendBool(newVisibility)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_surfacegroupSetOpacity(t_ilm_surfacegroup group, t_ilm_float opacity)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("SetSurfacegroupOpacity\0")
        && gIpcModule.appendUint(group)
        && gIpcModule.appendDouble(opacity)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_displaySetRenderOrder(t_ilm_display display, t_ilm_layer *pLayerId, const t_ilm_uint number)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLayerId
        && gIpcModule.createMessage("SetRenderOrderOfLayers\0")
        && gIpcModule.appendUintArray(pLayerId, number)
        && gIpcModule.appendUint(display)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_getScreenIDs(t_ilm_uint* pNumberOfIDs, t_ilm_uint** ppIDs)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pNumberOfIDs && ppIDs
        && gIpcModule.createMessage("GetScreenIDs\0")
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage()
        && gIpcModule.getUintArray(ppIDs, pNumberOfIDs))
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_takeScreenshot(t_ilm_uint screen, t_ilm_const_string filename)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("ScreenShot\0")
        && gIpcModule.appendUint(screen)
        && gIpcModule.appendString(filename)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_takeLayerScreenshot(t_ilm_const_string filename, t_ilm_layer layerid)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("ScreenShotOfLayer\0")
        && gIpcModule.appendString(filename)
        && gIpcModule.appendUint(layerid)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_takeSurfaceScreenshot(t_ilm_const_string filename, t_ilm_surface surfaceid)
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("ScreenShotOfSurface\0")
        && gIpcModule.appendString(filename)
        && gIpcModule.appendUint(surfaceid)
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

ilmErrorTypes ilm_commitChanges()
{
    LOG_ENTER_FUNCTION;
    ilmErrorTypes returnValue = ILM_FAILED;

    if (gIpcModule.createMessage("CommitChanges\0")
        && gIpcModule.sendMessage()
        && gIpcModule.receiveMessage(gReceiveTimeout)
        && !gIpcModule.isErrorMessage())
    {
        returnValue = ILM_SUCCESS;
    }

    return returnValue;
}

