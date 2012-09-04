/**************************************************************************
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
#ifndef __IPCMODULELOADER_H_
#define __IPCMODULELOADER_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "ilm_types.h"

struct IpcModule
{
    t_ilm_bool (*init)(t_ilm_bool);
    t_ilm_bool (*destroy)();

    t_ilm_bool (*createMessage)(t_ilm_const_string);
    t_ilm_bool (*sendMessage)();
    t_ilm_bool (*destroyMessage)();
    t_ilm_bool (*sendError)(t_ilm_const_string);
    enum IpcMessageType (*receiveMessage)(int); // timeout in ms

    t_ilm_const_string (*getMessageName)();
    t_ilm_const_string (*getSenderName)();
    t_ilm_bool (*isErrorMessage)();

    t_ilm_bool (*appendBool)(const t_ilm_bool);
    t_ilm_bool (*getBool)(t_ilm_bool*);

    t_ilm_bool (*appendDouble)(const double);
    t_ilm_bool (*getDouble)(double*);

    t_ilm_bool (*appendString)(const char*);
    t_ilm_bool (*getString)(char*);

    t_ilm_bool (*appendInt)(const int);
    t_ilm_bool (*getInt)(int*);
    t_ilm_bool (*appendIntArray)(const int*, int);
    t_ilm_bool (*getIntArray)(int**, int*);

    t_ilm_bool (*appendUint)(const unsigned int);
    t_ilm_bool (*getUint)(unsigned int*);
    t_ilm_bool (*appendUintArray)(const unsigned int*, int);
    t_ilm_bool (*getUintArray)(unsigned int**, int*);
};

t_ilm_bool loadIpcModule(struct IpcModule* communicator);

#ifdef __cplusplus
}  // extern "C"
#endif // __cplusplus

#endif // __IPCMODULELOADER_H_
