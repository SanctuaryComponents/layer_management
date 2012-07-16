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
#include "socketShared.h"

//-----------------------------------------------------------------------------
// append simple data types
//-----------------------------------------------------------------------------

t_ilm_bool appendGenericValue(const char protocolType, const char size, const void* value)
{
    struct SocketMessage* msg = &gState.outgoingMessage;

    // size check: is message size reached
    if (sizeof(msg->paket) - sizeof(msg->paket.data)  // header
        + msg->index + size                           // + data
        > SOCKET_MAX_MESSAGE_SIZE)
    {
        pritnf("Error: max message size exceeded.\n");
        return ILM_FALSE;
    }

    // append protocol type
    msg->paket.data[msg->index] = protocolType;
    msg->index += sizeof(protocolType);

    // append size of data
    msg->paket.data[msg->index] = size;
    msg->index += sizeof(size);

    // append data
    memcpy(&msg->paket.data[msg->index], value, size);
    msg->index += size;

    return ILM_TRUE;
}

t_ilm_bool appendUint(const t_ilm_uint value)
{
    return appendGenericValue(SOCKET_MESSAGE_TYPE_UINT, sizeof(t_ilm_uint), &value);
}

t_ilm_bool appendInt(const t_ilm_int value)
{
    return appendGenericValue(SOCKET_MESSAGE_TYPE_INT, sizeof(t_ilm_int), &value);
}

t_ilm_bool appendBool(const t_ilm_bool value)
{
    return appendGenericValue(SOCKET_MESSAGE_TYPE_BOOL, sizeof(t_ilm_bool), &value);
}

t_ilm_bool appendDouble(const t_ilm_float value)
{
    return appendGenericValue(SOCKET_MESSAGE_TYPE_DOUBLE, sizeof(t_ilm_float), &value);
}

t_ilm_bool appendString(t_ilm_const_string value)
{
    return appendGenericValue(SOCKET_MESSAGE_TYPE_STRING, strlen(value), value);
}

//-----------------------------------------------------------------------------
// append array data types
//-----------------------------------------------------------------------------

t_ilm_bool appendGenericArray(const char arraySize, const char protocolType, const char size, const void* value)
{
    t_ilm_bool result = ILM_TRUE;

    struct SocketMessage* msg = &gState.outgoingMessage;

    // TODO: size check: is message size reached?

    // append array type
    msg->paket.data[msg->index] = SOCKET_MESSAGE_TYPE_ARRAY;
    msg->index += sizeof(protocolType);

    // append size of array
    msg->paket.data[msg->index] = arraySize;
    msg->index += sizeof(arraySize);

    // append data for each array entry
    char i = 0;
    for (i = 0; i < arraySize; ++i)
    {
        result &= appendGenericValue(protocolType, size, value + i * size);
    }

    return result;
}

t_ilm_bool appendUintArray(const t_ilm_uint* valueArray, t_ilm_int arraySize)
{
    return appendGenericArray(arraySize, SOCKET_MESSAGE_TYPE_UINT, sizeof(t_ilm_uint), valueArray);
}

t_ilm_bool appendIntArray(const t_ilm_int* valueArray, t_ilm_int arraySize)
{
    return appendGenericArray(arraySize, SOCKET_MESSAGE_TYPE_INT, sizeof(t_ilm_int), valueArray);
}

t_ilm_bool appendBoolArray(const t_ilm_bool* valueArray, t_ilm_int arraySize)
{
    return appendGenericArray(arraySize, SOCKET_MESSAGE_TYPE_BOOL, sizeof(t_ilm_bool), valueArray);
}

t_ilm_bool appendDoubleArray(const t_ilm_float* valueArray, t_ilm_int arraySize)
{
    return appendGenericArray(arraySize, SOCKET_MESSAGE_TYPE_DOUBLE, sizeof(t_ilm_float), valueArray);
}

// TODO appendStringArray()
