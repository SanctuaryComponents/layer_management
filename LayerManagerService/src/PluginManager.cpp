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

#include "PluginManager.h"
#include "ICommandExecutor.h"
#include "Configuration.h"
#include "Scene.h"

#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <dlfcn.h>
#include "Log.h"
#include <libgen.h> // basename
#include <sys/stat.h>

#include <list>
using std::list;

#include <string.h>
using std::string;

typedef list<string> tFileList;
typedef list<string>::iterator tFileListIterator;

//===========================================================================
// plugin configuration
//===========================================================================
const char* gPluginLookupPath = NULL;

const char* gRendererPluginDirectories[] = { "/renderer" };

uint gRendererPluginDirectoriesCount = sizeof(gRendererPluginDirectories) / sizeof(gRendererPluginDirectories[0]);

const char* gCommunicatorPluginDirectories[] = { "/communicator" };

uint gCommunicatorPluginDirectoriesCount = sizeof(gCommunicatorPluginDirectories) / sizeof(gCommunicatorPluginDirectories[0]);

const char* gScenePluginDirectories[] = { "/sceneprovider" };

uint gScenePluginDirectoriesCount = sizeof(gScenePluginDirectories) / sizeof(gScenePluginDirectories[0]);

const char* gHealthPluginDirectories[] = { "/health" };

uint gHealthPluginDirectoriesCount = sizeof(gHealthPluginDirectories) / sizeof(gHealthPluginDirectories[0]);

//===========================================================================
// global functions for loading plugins
//===========================================================================
template<class T>
T* getCreateFunction(string libname)
{
    // cut off directories
    char* fileWithPath = const_cast<char*>(libname.c_str());
    string libFileName = basename(fileWithPath);
    LOG_DEBUG("LayerManagerService", "lib name without directory: " << libFileName);
    
    // cut off "lib" in front and cut off .so end"
    string createFunctionName = "create" + libFileName.substr(3, libFileName.length() - 6);
    LOG_DEBUG("LayerManagerService", "lib entry point name: " << createFunctionName);
    
    // open library
    void *libraryHandle;
    dlerror(); // Clear any existing error
    libraryHandle = dlopen(libname.c_str(), RTLD_NOW | RTLD_GLOBAL /*LAZY*/);
    const char* dlopen_error = dlerror();
    if (!libraryHandle || dlopen_error)
    {
        LOG_ERROR("LayerManagerService", "dlopen failed: " << dlopen_error);
        dlclose(libraryHandle);
        return 0;
    }
    
    // get entry point from shared lib
    dlerror(); // Clear any existing error
    LOG_DEBUG("LayerManagerService", "loading external function with name: " << createFunctionName);
    
    union
    {
        void* voidPointer;
        T* typedPointer;
    } functionPointer;
    
    // Note: direct cast is not allowed by ISO C++. e.g.
    // T* createFunction = reinterpret_cast<T*>(dlsym(libraryHandle, createFunctionName.c_str()));
    // compiler warning: "forbids casting between pointer-to-function and pointer-to-object"
    
    functionPointer.voidPointer = dlsym(libraryHandle, createFunctionName.c_str());
    T* createFunction = functionPointer.typedPointer;
    
    const char* dlsym_error = dlerror();
    if (!createFunction || dlsym_error)
    {
        LOG_ERROR("LayerManagerService", "Failed to load shared lib entry point: " << dlsym_error);
    }
    
    // Note: free these resources on shutdown
    //dlclose(libraryHandle);
    
    return createFunction;
}

void getSharedLibrariesFromDirectory(tFileList& fileList, string dirName)
{
    // open directory
    DIR *directory = opendir(dirName.c_str());
    if (!directory)
    {
        LOG_ERROR("LayerManagerService", "Error(" << errno << ") opening " << dirName);
        return;
    }
    
    // iterate content of directory
    struct dirent *itemInDirectory = 0;
    while ((itemInDirectory = readdir(directory)))
    {
        unsigned char entryType = itemInDirectory->d_type;
        string entryName = itemInDirectory->d_name;
        
        bool regularFile;
        bool sharedLibExtension = ("so" == entryName.substr(entryName.find_last_of(".") + 1));
        
        if ((entryType == DT_LNK) || (entryType == DT_UNKNOWN))
        {
            struct stat st;
            string fpath = dirName + "/" + itemInDirectory->d_name;
            
            if (!stat(fpath.c_str(), &st))
                regularFile = S_ISREG(st.st_mode);
            else
            {
                regularFile = false;
                LOG_WARNING("LayerManagerService", "Could not stat() file " << fpath << ": " << errno);
            }
        }
        else
        {
            regularFile = (entryType == DT_REG);
        }
        
        
        if (regularFile && sharedLibExtension)
        {
            LOG_DEBUG("LayerManagerService", "adding file " << entryName);
            fileList.push_back(dirName + "/" + entryName);
        }
        else
        {
            LOG_DEBUG("LayerManagerService", "ignoring file " << entryName);;
        }
    }
    
    closedir(directory);
}

void loadHealthPlugins(HealthMonitorList& healthMonitorList, ICommandExecutor* executor)
{
    tFileList sharedLibraryNameList;
    
    // search sceneprovider plugins in configured directories
    for (uint dirIndex = 0; dirIndex < gScenePluginDirectoriesCount; ++dirIndex)
    {
        char directoryName[1024];
        strncpy(directoryName, gPluginLookupPath, sizeof(directoryName) - 1);
        strncat(directoryName, gHealthPluginDirectories[dirIndex], sizeof(directoryName) - 1 - strlen(directoryName));
        LOG_DEBUG("LayerManagerService", "Searching for HealthMonitors in: " << directoryName);
        getSharedLibrariesFromDirectory(sharedLibraryNameList, directoryName);
    }
    
    LOG_DEBUG("LayerManagerService", sharedLibraryNameList.size() << " HealthMonitor plugins found");
    
    // iterate all communicator plugins and start them
    tFileListIterator iter = sharedLibraryNameList.begin();
    tFileListIterator iterEnd = sharedLibraryNameList.end();
    
    for (; iter != iterEnd; ++iter)
    {
        LOG_INFO("LayerManagerService", "Loading HealthMonitor plugin " << *iter);
        
        IHealthMonitor* (*createFunc)(ICommandExecutor* executor);
        createFunc = getCreateFunction<IHealthMonitor*(ICommandExecutor* executor)>(*iter);
        
        if (!createFunc)
        {
            LOG_DEBUG("LayerManagerService", "Entry point of HealthMonitor not found");
            continue;
        }
        
        LOG_DEBUG("LayerManagerService", "Creating HealthMonitor instance");
        IHealthMonitor* newHealthMonitor = createFunc(executor);
        
        if (!newHealthMonitor)
        {
            LOG_ERROR("LayerManagerService","HealthMonitor initialization failed. Entry Function not callable");
            continue;
        }
        
        healthMonitorList.push_back(newHealthMonitor);
    }
}


void loadScenePlugins(SceneProviderList& sceneProviderList, ICommandExecutor* executor)
{
    tFileList sharedLibraryNameList;
    
    // search sceneprovider plugins in configured directories
    for (uint dirIndex = 0; dirIndex < gScenePluginDirectoriesCount; ++dirIndex)
    {
        char directoryName[1024];
        strncpy(directoryName, gPluginLookupPath, sizeof(directoryName) - 1);
        strncat(directoryName, gScenePluginDirectories[dirIndex], sizeof(directoryName) - 1 - strlen(directoryName));
        LOG_DEBUG("LayerManagerService", "Searching for SceneProviders in: " << directoryName);
        getSharedLibrariesFromDirectory(sharedLibraryNameList, directoryName);
    }
    
    LOG_DEBUG("LayerManagerService", sharedLibraryNameList.size() << " SceneProvider plugins found");
    
    // iterate all communicator plugins and start them
    tFileListIterator iter = sharedLibraryNameList.begin();
    tFileListIterator iterEnd = sharedLibraryNameList.end();
    
    for (; iter != iterEnd; ++iter)
    {
        LOG_INFO("LayerManagerService", "Loading SceneProvider plugin " << *iter);
        
        ISceneProvider* (*createFunc)(ICommandExecutor*);
        createFunc = getCreateFunction<ISceneProvider*(ICommandExecutor*)>(*iter);
        
        if (!createFunc)
        {
            LOG_DEBUG("LayerManagerService", "Entry point of SceneProvider not found");
            continue;
        }
        
        LOG_DEBUG("LayerManagerService", "Creating SceneProvider instance");
        ISceneProvider* newSceneProvider = createFunc(executor);
        
        if (!newSceneProvider)
        {
            LOG_ERROR("LayerManagerService","SceneProvider initialization failed. Entry Function not callable");
            continue;
        }
        
        sceneProviderList.push_back(newSceneProvider);
    }
}


void loadCommunicatorPlugins(CommunicatorList& communicatorList, ICommandExecutor* executor)
{
    tFileList sharedLibraryNameList;
    
    // search communicator plugins in configured directories
    for (uint dirIndex = 0; dirIndex < gCommunicatorPluginDirectoriesCount; ++dirIndex)
    {
        char directoryName[1024];
        strncpy(directoryName, gPluginLookupPath, sizeof(directoryName) - strlen(directoryName));
        strncat(directoryName, gCommunicatorPluginDirectories[dirIndex], sizeof(directoryName) -strlen(directoryName));
        LOG_DEBUG("LayerManagerService", "Searching for communicator in: " << directoryName);
        getSharedLibrariesFromDirectory(sharedLibraryNameList, directoryName);
    }
    
    LOG_DEBUG("LayerManagerService", sharedLibraryNameList.size() << " Communicator plugins found");
    
    // iterate all communicator plugins and start them
    tFileListIterator iter = sharedLibraryNameList.begin();
    tFileListIterator iterEnd = sharedLibraryNameList.end();
    
    for (; iter != iterEnd; ++iter)
    {
        LOG_INFO("LayerManagerService", "Loading Communicator plugin " << *iter);
        
        ICommunicator* (*createFunc)(ICommandExecutor*);
        createFunc = getCreateFunction<ICommunicator*(ICommandExecutor*)>(*iter);
        
        if (!createFunc)
        {
            LOG_DEBUG("LayerManagerService", "Entry point of Communicator not found");
            continue;
        }
        
        LOG_DEBUG("LayerManagerService", "Creating Communicator instance");
        ICommunicator* newCommunicator = createFunc(executor);
        
        if (!newCommunicator)
        {
            LOG_ERROR("LayerManagerService","Communicator initialization failed. Entry Function not callable");
            continue;
        }
        
        communicatorList.push_back(newCommunicator);
    }
}

void loadRendererPlugins(RendererList& rendererList, IScene* pScene)
{
    tFileList sharedLibraryNameList;
    
    // search communicator plugins in configured directories
    for (uint dirIndex = 0; dirIndex < gRendererPluginDirectoriesCount; ++dirIndex)
    {
        char directoryName[1024];
        strncpy(directoryName, gPluginLookupPath, sizeof(directoryName) - 1);
        strncat(directoryName, gRendererPluginDirectories[dirIndex], sizeof(directoryName) - 1 - strlen(directoryName));
        LOG_DEBUG("LayerManagerService", "Searching for renderer in: " << directoryName);
        getSharedLibrariesFromDirectory(sharedLibraryNameList, directoryName);
    }
    
    LOG_DEBUG("LayerManagerService", sharedLibraryNameList.size() << " Renderer plugins found");
    
    // currently the use of only one renderer is enforced
    if (sharedLibraryNameList.size() > 1)
    {
        LOG_WARNING("LayerManagerService", "more than 1 Renderer plugin found. using only " << sharedLibraryNameList.front());
        while (sharedLibraryNameList.size() > 1)
        {
            sharedLibraryNameList.pop_back();
        }
    }
    
    // iterate all renderer plugins and start them
    tFileListIterator iter = sharedLibraryNameList.begin();
    tFileListIterator iterEnd = sharedLibraryNameList.end();
    
    for (; iter != iterEnd; ++iter)
    {
        LOG_INFO("LayerManagerService", "Loading Renderer plugin " << *iter);
        IRenderer* (*createFunc)(IScene*);
        createFunc = getCreateFunction<IRenderer*(IScene*)>(*iter);
        if (!createFunc)
        {
            LOG_DEBUG("LayerManagerService", "Entry point of Renderer not found");
            continue;
        }
        
        LOG_DEBUG("LayerManagerService", "Creating Renderer instance");
        IRenderer* newRenderer = createFunc(pScene);
        if (!newRenderer)
        {
            LOG_ERROR("LayerManagerService","Renderer initialization failed. Entry Function not callable");
            continue;
        }
        
        rendererList.push_back(newRenderer);
    }
}


//===========================================================================
// class implementation
//===========================================================================
PluginManager::PluginManager(ICommandExecutor& executor, Configuration& config)
: mExecutor(executor)
, mConfiguration(config)
{
    createAndStartAllPlugins();
}

PluginManager::~PluginManager()
{
    stopAndDestroyAllPlugins();
}

RendererList* PluginManager::getRendererList()
{
    return &mRendererList;
}

CommunicatorList* PluginManager::getCommunicatorList()
{
    return &mCommunicatorList;
}

SceneProviderList* PluginManager::getSceneProviderList()
{
    return &mSceneProviderList;
}

HealthMonitorList* PluginManager::getHealthMonitorList()
{
    return &mHealthMonitorList;
}

void PluginManager::createAndStartAllPlugins()
{
    gPluginLookupPath = mConfiguration.getPluginPath().c_str();
    
    LOG_INFO("LayerManagerService", "Used plugin directory is " << gPluginLookupPath);
    
    LOG_DEBUG("LayerManagerService", "Loading renderer plugins.");
    loadRendererPlugins(mRendererList, mExecutor.getScene());
    
    LOG_DEBUG("LayerManagerService", "Loading communicator plugins.");
    loadCommunicatorPlugins(mCommunicatorList, &mExecutor);
    
    LOG_DEBUG("LayerManagerService", "Loading scene provider plugins.");
    loadScenePlugins(mSceneProviderList, &mExecutor);

    LOG_DEBUG("LayerManagerService", "Loading health monitor plugins.");
    loadHealthPlugins(mHealthMonitorList, &mExecutor);
}

void PluginManager::stopAndDestroyAllPlugins()
{
    LOG_DEBUG("LayerManagerService", "Removing all scene provider plugins.")
    {
        SceneProviderListIterator iter = mSceneProviderList.begin();
        SceneProviderListIterator iterEnd = mSceneProviderList.end();
        for (; iter != iterEnd; ++iter)
        {
            ISceneProvider* sceneProvider = *iter;
            delete sceneProvider;
        }
        mSceneProviderList.clear();
    }
    
    LOG_DEBUG("LayerManagerService", "Removing all communicator plugins.")
    {
        CommunicatorListIterator iter = mCommunicatorList.begin();
        CommunicatorListIterator iterEnd = mCommunicatorList.end();
        for (; iter != iterEnd; ++iter)
        {
            ICommunicator* comm = *iter;
            delete comm;
        }
        mCommunicatorList.clear();
    }
    
    LOG_DEBUG("LayerManagerService", "Removing all renderer plugins.")
    {
        RendererListIterator iter = mRendererList.begin();
        RendererListIterator iterEnd = mRendererList.end();
        for (; iter != iterEnd; ++iter)
        {
            IRenderer* renderer = *iter;
            delete renderer;
        }
        mRendererList.clear();
    }
}
