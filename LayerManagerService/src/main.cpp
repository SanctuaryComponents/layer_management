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

#include "Layermanager.h"
#include "IRenderer.h"
#include "ICommunicator.h"
#include "ISceneProvider.h"
#include "ICommandExecutor.h"
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <dlfcn.h>
#include "Log.h"
#include <getopt.h>
#include <libgen.h> // basename

#include <list>
using std::list;

#include <string.h>
using std::string;

typedef list<string> tFileList;
typedef list<string>::iterator tFileListIterator;


const char* displayName = ":0";
int displayWidth = 1280; // default value, override with -w argument
int displayHeight = 480; // default value, override with -h argument

const char* gRendererPluginDirectories[] = { "/usr/lib/layermanager/renderer",
                                             "/usr/local/lib/layermanager/renderer"};
uint gRendererPluginDirectoriesCount = sizeof(gRendererPluginDirectories) / sizeof(gRendererPluginDirectories[0]);

const char* gCommunicatorPluginDirectories[] = { "/usr/lib/layermanager/communicator",
                                                 "/usr/local/lib/layermanager/communicator" };

uint gCommunicatorPluginDirectoriesCount = sizeof(gCommunicatorPluginDirectories) / sizeof(gCommunicatorPluginDirectories[0]);
                                                 
const char* gScenePluginDirectories[] = { "/usr/lib/layermanager",
                                          "/usr/local/lib/layermanager" };

uint gScenePluginDirectoriesCount = sizeof(gScenePluginDirectories) / sizeof(gScenePluginDirectories[0]);                                                 
                                                 


const char* USAGE_DESCRIPTION = "Usage:\t LayerManagerService [options]\n"
                                "options:\t\n\n"
                                "\t-w: Window Width\t\n"
                                "\t-h: Window Height\t\n"
                                "\t-d: displayName \t\n"
                                "\t-f: loglevel file \t 0 [default] \n\t\t\t\t[0=disabled,1=error,2=info,3=warning,4=debug]\n"
                                "\t-c: loglevel console \t 2 [default] \n\t\t\t\t[0=disabled,1=error,2=info,3=warning,4=debug]\n"
                                "\nexample: LayerManagerService -w800 -h480 -d:0\n";

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
    libraryHandle = dlopen(libname.c_str(), RTLD_NOW /*LAZY*/);
    const char* dlopen_error = dlerror();
    if (!libraryHandle || dlopen_error)
    {
        LOG_ERROR("LayerManagerService", "dlopen failed: " << dlopen_error);
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

    return createFunction;
}

void parseCommandLine(int argc, char **argv)
{
    while (optind < argc)
    {
        int option = getopt(argc, argv, "w::h::d::?::c::f::");
        switch (option)
        {
        case 'd':
            displayName = optarg;
            break;
        case 'w':
            displayWidth = atoi(optarg);
            break;
        case 'h':
            displayHeight = atoi(optarg);
            break;
        case 'c':
            if ( atoi(optarg) < LOG_MAX_LEVEL ) 
            {
                Log::consoleLogLevel = (LOG_MODES) atoi(optarg);
            }
            break;
        case 'f':
            if ( atoi(optarg) < LOG_MAX_LEVEL ) 
            {
                Log::fileLogLevel = (LOG_MODES) atoi(optarg);
            }
            break;
        case '?':   
        default:
            puts(USAGE_DESCRIPTION);
            exit(-1);
        }
    }
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

        bool regularFile = (entryType == DT_REG);
        bool sharedLibExtension = ("so" == entryName.substr(entryName.find_last_of(".") + 1));

        if (regularFile && sharedLibExtension)
        {
            LOG_INFO("LayerManagerService", "adding file " << entryName);
            fileList.push_back(dirName + "/" + entryName);
        }
        else
        {
            LOG_INFO("LayerManagerService", "ignoring file " << entryName);
        }
    }

    closedir(directory);
}

void loadScenePlugins(SceneProviderList& sceneProviderList, ICommandExecutor* executor)
{
    tFileList sharedLibraryNameList;

    // search sceneprovider plugins in configured directories
    for (uint dirIndex = 0; dirIndex < gScenePluginDirectoriesCount; ++dirIndex)
    {
        const char* directoryName = gScenePluginDirectories[dirIndex];
        LOG_DEBUG("LayerManagerService", "Searching for SceneProviders in: " << directoryName);
        getSharedLibrariesFromDirectory(sharedLibraryNameList, directoryName);
    }

    LOG_INFO("LayerManagerService", sharedLibraryNameList.size() << " SceneProvider plugins found");

    // iterate all communicator plugins and start them
    tFileListIterator iter = sharedLibraryNameList.begin();
    tFileListIterator iterEnd = sharedLibraryNameList.end();

    for (; iter != iterEnd; ++iter)
    {
        LOG_DEBUG("LayerManagerService", "Loading SceneProvider library " << *iter);

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
        const char* directoryName = gCommunicatorPluginDirectories[dirIndex];
        LOG_DEBUG("LayerManagerService", "Searching for communicator in: " << directoryName);
        getSharedLibrariesFromDirectory(sharedLibraryNameList, directoryName);
    }

    LOG_INFO("LayerManagerService", sharedLibraryNameList.size() << " Communicator plugins found");

    // iterate all communicator plugins and start them
    tFileListIterator iter = sharedLibraryNameList.begin();
    tFileListIterator iterEnd = sharedLibraryNameList.end();

    for (; iter != iterEnd; ++iter)
    {
        LOG_DEBUG("LayerManagerService", "Loading Communicator library " << *iter);

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
        const char* directoryName = gRendererPluginDirectories[dirIndex];
        LOG_DEBUG("LayerManagerService", "Searching for renderer in: " << directoryName);
        getSharedLibrariesFromDirectory(sharedLibraryNameList, directoryName);
    }

    LOG_INFO("LayerManagerService", sharedLibraryNameList.size() << " Renderer plugins found");

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
        LOG_DEBUG("LayerManagerService", "Loading Renderer library " << *iter);
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

int main(int argc, char **argv)
{
    parseCommandLine(argc, (char**) argv);

    LOG_INFO("LayerManagerService", "Starting Layermanager.");

    LOG_INFO("LayerManagerService", "Creating Layermanager.");
    ICommandExecutor* pManager = new Layermanager();
    IScene* pScene = pManager->getScene();

    // Create and load Renderer plugins
    LOG_INFO("LayerManagerService", "Loading renderer plugins.");
    RendererList rendererList;
    loadRendererPlugins(rendererList, pScene);

    // register renderer plugins at layermanager
    LOG_INFO("LayerManagerService", "Adding " << rendererList.size() << " Renderer plugins to Layermanager.");
    RendererListIterator rendererIter = rendererList.begin();
    RendererListIterator rendererIterEnd = rendererList.end();
    for (; rendererIter != rendererIterEnd; ++rendererIter)
    {
        pManager->addRenderer(*rendererIter);
    }

    // Create and load communicator plugins
    LOG_INFO("LayerManagerService", "Loading communicator plugins.");
    CommunicatorList communicatorList;
    loadCommunicatorPlugins(communicatorList, pManager);

    // register communicator plugins at layermanager
    LOG_INFO("LayerManagerService", "Adding " << communicatorList.size() << " Communicator plugin(s) to Layermanager.");
    CommunicatorListIterator commIter = communicatorList.begin();
    CommunicatorListIterator commIterEnd = communicatorList.end();
    for (; commIter != commIterEnd; ++commIter)
    {
        pManager->addCommunicator(*commIter);
    }
    LOG_INFO("LayerManagerService", "Loading scene provider plugins.");
    SceneProviderList sceneProviderList;
    loadScenePlugins(sceneProviderList, pManager);

    // register communicator plugins at layermanager
    LOG_INFO("LayerManagerService", "Adding " << sceneProviderList.size() << " SceneProvider plugin(s) to Layermanager.");
    SceneProviderListIterator sceneProviderIter = sceneProviderList.begin();
    SceneProviderListIterator sceneProviderIterEnd = sceneProviderList.end();
    for (; sceneProviderIter != sceneProviderIterEnd; ++sceneProviderIter)
    {
        pManager->addSceneProvider(*sceneProviderIter);
    }
    bool started = pManager->startManagement(displayWidth, displayHeight, displayName);

    if (!started)
    {
        LOG_ERROR("LayerManagerService", "Exiting Application.");
        return -1;
    }

    // must stay within main method or else application would completely exit
    LOG_INFO("LayerManagerService", "Startup complete. EnterMainloop");
    while (true)
    {
        sleep(2000);
    }

    LOG_INFO("LayerManagerService", "Exiting Application.");

    // cleanup
    pManager->stopManagement();
    //delete pRenderer; TODO
    //delete pCommunicator; TODO
    delete pManager;

    return 0;
}
