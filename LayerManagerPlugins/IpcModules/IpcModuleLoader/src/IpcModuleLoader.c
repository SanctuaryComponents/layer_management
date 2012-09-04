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
#include "IpcModuleLoader.h"
#include "IpcModule.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <errno.h>
#include <dirent.h> // DIR
#include <string.h> // strcpy, strcat, strstr

//=============================================================================
// logging
//=============================================================================
//#define LOG_ENTER_FUNCTION printf("--> ilmCommunicatorControl::%s\n", __PRETTY_FUNCTION__)
#define LOG_ENTER_FUNCTION


//=============================================================================
// global variables
//=============================================================================
const char* gDefaultPluginLookupPath = CMAKE_INSTALL_PREFIX"/lib/layermanager";
const char* gCommunicatorPluginDirectory = "/ipcmodules";


//=============================================================================
// plugin loading
//=============================================================================
t_ilm_bool loadSymbolTable(struct IpcModule* ipcModule, char* path, char* file)
{
    t_ilm_bool returnValue = ILM_FALSE;
    void* pluginLibHandle = 0;
    char fullFilePath[1024];
    fullFilePath[0] = '\0';

    struct ApiFunction
    {
        const char* name;
        void** funcPtr;
    };

    struct ApiFunction ApiFunctionTable[] =
    {
        { "init",            (void**)&ipcModule->init },
        { "destroy",         (void**)&ipcModule->destroy },
        { "createMessage",   (void**)&ipcModule->createMessage },
        { "sendMessage",     (void**)&ipcModule->sendMessage },
        { "destroyMessage",  (void**)&ipcModule->destroyMessage },
        { "sendError",       (void**)&ipcModule->sendError },
        { "isErrorMessage",  (void**)&ipcModule->isErrorMessage },
        { "receiveMessage",  (void**)&ipcModule->receiveMessage },
        { "getMessageName",  (void**)&ipcModule->getMessageName },
        { "getSenderName",   (void**)&ipcModule->getSenderName },
        { "appendBool",      (void**)&ipcModule->appendBool },
        { "getBool",         (void**)&ipcModule->getBool },
        { "appendDouble",    (void**)&ipcModule->appendDouble },
        { "getDouble",       (void**)&ipcModule->getDouble },
        { "appendString",    (void**)&ipcModule->appendString },
        { "getString",       (void**)&ipcModule->getString },
        { "appendInt",       (void**)&ipcModule->appendInt },
        { "getInt",          (void**)&ipcModule->getInt },
        { "appendIntArray",  (void**)&ipcModule->appendIntArray },
        { "getIntArray",     (void**)&ipcModule->getIntArray },
        { "appendUint",      (void**)&ipcModule->appendUint },
        { "getUint",         (void**)&ipcModule->getUint },
        { "appendUintArray", (void**)&ipcModule->appendUintArray },
        { "getUintArray",    (void**)&ipcModule->getUintArray }
    };

    const unsigned int apiFunctionCount = sizeof (ApiFunctionTable) / sizeof(struct ApiFunction);
    unsigned int symbolCount = 0;

    strcat(fullFilePath, path);
    strcat(fullFilePath, "/");
    strcat(fullFilePath, file);

    pluginLibHandle = dlopen(fullFilePath, RTLD_LAZY);

    if (pluginLibHandle)
    {
        unsigned int i = 0;
        for (i = 0; i < apiFunctionCount; ++i)
        {
            struct ApiFunction* func = &ApiFunctionTable[i];

            *func->funcPtr = dlsym(pluginLibHandle, func->name);
            if (*func->funcPtr)
            {
                symbolCount++;
            }
        }
    }

    if (symbolCount == apiFunctionCount)
    {
        returnValue = ILM_TRUE;
    }
    else
    {
        printf("Error in %s: found %d symbols, expected %d symbols.\n", fullFilePath, symbolCount, apiFunctionCount);
        if (0 != errno)
        {
            printf("--> error: %s\n", strerror(errno));
        }
        printf("--> not a valid ipc module\n");
        if (pluginLibHandle)
        {
            dlclose(pluginLibHandle);
        }
    }

    return returnValue;
}

t_ilm_bool loadIpcModule(struct IpcModule* communicator)
{
    t_ilm_bool result = ILM_FALSE;

    // find communicator client plugin
    char* pluginLookupPath = getenv("LM_PLUGIN_PATH");
    if  (pluginLookupPath)
    {
       gDefaultPluginLookupPath = pluginLookupPath;
    }

    char path[1024];
    strcpy(path, gDefaultPluginLookupPath);
    strcat(path, gCommunicatorPluginDirectory);

    // open directory
    DIR *directory = opendir(path);
    if (directory)
    {
        // iterate content of directory
        struct dirent *itemInDirectory = 0;
        while ((itemInDirectory = readdir(directory)) && !result)
        {
            char* fileName = itemInDirectory->d_name;

            if (strstr(fileName, ".so"))
            {
                result = loadSymbolTable(communicator, path, fileName);
            }
        }

        closedir(directory);
    }
    else
    {
        printf("IpcModuleLoader: Error opening plugin dir %s\n", path);
    }

    return result;
}
