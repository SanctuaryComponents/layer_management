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
#ifndef __IPCMODULE_H__
#define __IPCMODULE_H__

#include "ilm_types.h"

//=============================================================================
// initialization
//=============================================================================
t_ilm_bool init(t_ilm_bool isClient);
t_ilm_bool destroy();

//=============================================================================
// messaging
//=============================================================================
t_ilm_bool createMessage(t_ilm_const_string name);
t_ilm_bool sendMessage();
t_ilm_bool sendError(t_ilm_const_string errorDescription);
enum IpcMessageType receiveMessage(t_ilm_int timeoutInMs);

t_ilm_const_string getMessageName();
t_ilm_const_string getSenderName();
t_ilm_bool isErrorMessage();

//=============================================================================
// message content
//=============================================================================
t_ilm_bool appendBool(const t_ilm_bool value);
t_ilm_bool getBool(t_ilm_bool* value);

t_ilm_bool appendDouble(const t_ilm_float value);
t_ilm_bool getDouble(t_ilm_float* value);

t_ilm_bool appendString(const char* value);
t_ilm_bool getString(char* value);

t_ilm_bool appendInt(const int value);
t_ilm_bool appendIntArray(const t_ilm_int* valueArray, t_ilm_int arraySize);
t_ilm_bool getIntArray(t_ilm_int** valueArray, t_ilm_int* arraySize);
t_ilm_bool getInt(t_ilm_int* value);

t_ilm_bool appendUint(const t_ilm_uint value);
t_ilm_bool appendUintArray(const t_ilm_uint* valueArray, t_ilm_int arraySize);
t_ilm_bool getUintArray(t_ilm_uint** valueArray, t_ilm_int* arraySize);
t_ilm_bool getUint(t_ilm_uint* value);

// TODO: signal mechanism?

#endif // __IPCMODULE_H__
