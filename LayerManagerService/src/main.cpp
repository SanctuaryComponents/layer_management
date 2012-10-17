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
#include "config.h"
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
#include <signal.h>
#include <execinfo.h> // for stacktrace
#include <sys/stat.h>

#include <list>
using std::list;

#include <string.h>
using std::string;

typedef list<string> tFileList;
typedef list<string>::iterator tFileListIterator;


const char* displayName = ":0";
const char* gPluginLookupPath = CMAKE_INSTALL_PREFIX"/lib/layermanager";
int displayWidth = 1280; // default value, override with -w argument
int displayHeight = 480; // default value, override with -h argument

const char* gRendererPluginDirectories[] = { "/renderer" };

uint gRendererPluginDirectoriesCount = sizeof(gRendererPluginDirectories) / sizeof(gRendererPluginDirectories[0]);

const char* gCommunicatorPluginDirectories[] = { "/communicator" };

uint gCommunicatorPluginDirectoriesCount = sizeof(gCommunicatorPluginDirectories) / sizeof(gCommunicatorPluginDirectories[0]);
                                                 
const char* gScenePluginDirectories[] = { "" };

uint gScenePluginDirectoriesCount = sizeof(gScenePluginDirectories) / sizeof(gScenePluginDirectories[0]);                                                 
                                                 
const char* USAGE_DESCRIPTION = "Usage:\t LayerManagerService [options]\n"
                                "options:\t\n\n"
                                "\t-w: Window Width\t\n"
                                "\t-h: Window Height\t\n"
                                "\t-d: displayName \t\n"
                                "\t-f: loglevel file \t 0 [default] \n\t\t\t\t[0=disabled,1=error,2=info,3=warning,4=debug]\n"
                                "\t-c: loglevel console \t 2 [default] \n\t\t\t\t[0=disabled,1=error,2=info,3=warning,4=debug]\n"
                                "\t-v: show version info\t\n"
                                "\nexample: LayerManagerService -w800 -h480 -d:0\n";

bool g_LayerManagerRunning;

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
        free(libraryHandle);
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
    //free(libraryHandle);

    return createFunction;
}

void parseCommandLine(int argc, char **argv)
{
    while (optind < argc)
    {
        int option = getopt(argc, argv, "w::h::d::?::c::f::v::");
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
        case 'v':
            printf("LayerManagerService\nVersion: %s\n", ILM_VERSION);
            exit(-1);
            break;
        case '?':   
        default:
            printf("LayerManagerService\nVersion: %s\n", ILM_VERSION);
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

void printStackTrace()
{
    const int maxStackSize = 64;
    void* stack[maxStackSize];

    size_t count = backtrace(stack, maxStackSize);
    char **lines = backtrace_symbols(stack, count);

    LOG_INFO("LayerManagerService", "--------------------------------------------------");
    for (unsigned int i = 0; i < count; ++i)
    {
        LOG_INFO("LayerManagerService", "Stack-Trace [" << i << "]: " << lines[i]);
    }
    LOG_INFO("LayerManagerService", "--------------------------------------------------");

    LOG_INFO("LayerManagerService", "Exiting application.")
    exit(-1);
}

void signalHandler(int sig)
{
    switch (sig)
    {
    case SIGTERM:
        g_LayerManagerRunning = false;
        LOG_INFO("LayerManagerService", "Signal SIGTERM received. Shutting down.");
        break;

    case SIGINT:
        g_LayerManagerRunning = false;
        LOG_INFO("LayerManagerService", "Signal SIGINT received. Shutting down.");
        break;

    case SIGBUS:
        g_LayerManagerRunning = false;
        LOG_ERROR("LayerManagerService", "Signal SIGBUS received. Shutting down.");
        printStackTrace();
        break;

    case SIGSEGV:
        g_LayerManagerRunning = false;
        LOG_ERROR("LayerManagerService", "Signal SIGSEGV received. Shutting down.");
        printStackTrace();
        break;
    case SIGABRT:
        g_LayerManagerRunning = false;
        LOG_ERROR("LayerManagerService", "Signal SIGABRT received. Shutting down.");
        printStackTrace();
        break;

    default:
        LOG_INFO("LayerManagerService", "Signal " << sig << " received.");
        break;
    }
}

int main(int argc, char **argv)
{
    // setup signal handler and global flag to handle shutdown
    g_LayerManagerRunning = true;

    LOG_DEBUG("LayerManagerService", "Setup signal handling.");
    signal(SIGBUS, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGABRT, signalHandler);

    parseCommandLine(argc, (char**) argv);
    char* pluginLookupPath = getenv("LM_PLUGIN_PATH");
    LOG_INFO("LayerManagerService", "Starting Layermanager (version: " << ILM_VERSION << ")");

    std::stringstream commandLine;
    for (int i = 0; i < argc; ++i)
    {
        commandLine << argv[i] << " ";
    }
    LOG_INFO("LayerManagerService", "Command Line: " << commandLine.str());

    buildConfigurationFlag configFlags[] = { buildConfigurationFlags };
    int configFlagCount = sizeof(configFlags) / sizeof(configFlags[0]);
    for (int i = 0; i < configFlagCount; ++i)
    {
        buildConfigurationFlag& flag = configFlags[i];
        switch (flag.type)
        {
        case DEBUG_FLAG:
            LOG_DEBUG("LayerManagerService", flag.description);
            break;
        case INFO_FLAG:
            LOG_INFO("LayerManagerService", flag.description);
            break;
        }
    }

    if  (pluginLookupPath != NULL ) 
    {
        gPluginLookupPath = pluginLookupPath;
    }
    LOG_INFO("LayerManagerService", "Used plugin directory is " << gPluginLookupPath);

    LOG_DEBUG("LayerManagerService", "Creating Layermanager.");
    ICommandExecutor* pManager = new Layermanager();
    IScene* pScene = pManager->getScene();

    // Create and load Renderer plugins
    LOG_DEBUG("LayerManagerService", "Loading renderer plugins.");
    RendererList rendererList;
    loadRendererPlugins(rendererList, pScene);

    // register renderer plugins at layermanager
    LOG_DEBUG("LayerManagerService", "Using " << rendererList.size() << " Renderer plugins");
    RendererListIterator rendererIter = rendererList.begin();
    RendererListIterator rendererIterEnd = rendererList.end();
    for (; rendererIter != rendererIterEnd; ++rendererIter)
    {
        pManager->addRenderer(*rendererIter);
    }

    // Create and load communicator plugins
    LOG_DEBUG("LayerManagerService", "Loading communicator plugins.");
    CommunicatorList communicatorList;
    loadCommunicatorPlugins(communicatorList, pManager);

    // register communicator plugins at layermanager
    LOG_DEBUG("LayerManagerService", "Using " << communicatorList.size() << " Communicator plugins");
    CommunicatorListIterator commIter = communicatorList.begin();
    CommunicatorListIterator commIterEnd = communicatorList.end();
    for (; commIter != commIterEnd; ++commIter)
    {
        pManager->addCommunicator(*commIter);
    }
    LOG_DEBUG("LayerManagerService", "Loading scene provider plugins.");
    SceneProviderList sceneProviderList;
    loadScenePlugins(sceneProviderList, pManager);

    // register communicator plugins at layermanager
    LOG_DEBUG("LayerManagerService", "Using " << sceneProviderList.size() << " SceneProvider plugins");
    SceneProviderListIterator sceneProviderIter = sceneProviderList.begin();
    SceneProviderListIterator sceneProviderIterEnd = sceneProviderList.end();
    for (; sceneProviderIter != sceneProviderIterEnd; ++sceneProviderIter)
    {
        pManager->addSceneProvider(*sceneProviderIter);
    }
    bool started = pManager->startManagement(displayWidth, displayHeight, displayName);

    if (started)
    {
        LOG_INFO("LayerManagerService", "Startup complete. EnterMainloop");
    
        while (g_LayerManagerRunning)
        {
            CommunicatorListIterator commIter = communicatorList.begin();
            CommunicatorListIterator commIterEnd = communicatorList.end();
            for (; commIter != commIterEnd; ++commIter)
            {
                (*commIter)->process(-1);
            }
        }
    }

    LOG_INFO("LayerManagerService", "Exiting Application.");

    LOG_DEBUG("LayerManagerService", "Stopping service.")
    pManager->stopManagement();

    LOG_DEBUG("LayerManagerService", "Removing all scene provider plugins.")
    {
        SceneProviderList* pList = pManager->getSceneProviderList();
        SceneProviderListIterator iter = pList->begin();
        SceneProviderListIterator iterEnd = pList->end();

        for (; iter != iterEnd; ++iter)
        {
            ISceneProvider* sceneProvider = *iter;
            delete sceneProvider;
        }

        pList->clear();
    }

    LOG_DEBUG("LayerManagerService", "Removing all communicator plugins.")
    {
        CommunicatorList* pList = pManager->getCommunicatorList();
        CommunicatorListIterator iter = pList->begin();
        CommunicatorListIterator iterEnd = pList->end();

        for (; iter != iterEnd; ++iter)
        {
            ICommunicator* comm = *iter;
            delete comm;
        }

        pList->clear();
    }

    LOG_DEBUG("LayerManagerService", "Removing all renderer plugins.")
    {
        RendererList* pList = pManager->getRendererList();
        RendererListIterator iter = pList->begin();
        RendererListIterator iterEnd = pList->end();

        for (; iter != iterEnd; ++iter)
        {
            IRenderer* renderer = *iter;
            delete renderer;
        }

        pList->clear();
    }

    LOG_DEBUG("LayerManagerService", "Removing manager.")
    delete pManager;

    // reset signal handling to default
    LOG_DEBUG("LayerManagerService", "Remove signal handling.");
    signal(SIGBUS, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGABRT, SIG_DFL);

    LOG_INFO("LayerManagerService", "Shutdown complete.")    
    Log::closeInstance();
    return (int)started - 1;
}
