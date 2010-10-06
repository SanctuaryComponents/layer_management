/***************************************************************************
*
* Copyright 2010 BMW Car IT GmbH
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

#include "Layermanager.h"
#include "IRenderer.h"
#include "BaseCommunicator.h"
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <dlfcn.h>
#include <string.h>
#include "Log.h"
#include <getopt.h>
const char* displayName = "Tegra:VGA0";
int displayWidth = 1280;
int displayHeight = 480;
const char* USAGE_DESCRIPTION =
		"Usage:\t layerManager [options]\n"
		"Option:\t\n\n"
		"\t-w: Window Width\t\n"
		"\t-h: Window Height\t\n"
		"\t-d: displayName \t\n";
template<class T>
T* getCreateFunction(std::string libname)
{
	void *libraryHandle;
	libraryHandle = dlopen (libname.c_str(), RTLD_LAZY);
	if (NULL == libraryHandle) {
		LOG_ERROR("Main", dlerror());
	}
	// cut off directories
	int lastSlashPosition = libname.rfind('/')+1;
	int lengthOfFilenameWithoutDirectories = libname.length()-lastSlashPosition;
	std::string createfunctionname = libname.substr(lastSlashPosition,lengthOfFilenameWithoutDirectories);
	LOG_DEBUG("without directory:", createfunctionname);
	// cut off "lib" in front and cut off .so end"
	createfunctionname = "create" + createfunctionname.substr(3, createfunctionname.length()-6);
	dlerror();    /* Clear any existing error */
	LOG_DEBUG("loading external function with name:", createfunctionname);
	T* createFunction = (T*)  dlsym(libraryHandle, createfunctionname.c_str());
	const char* dlsym_error = dlerror();
	if (dlsym_error)
	{
			LOG_ERROR("Main", "Cannot load symbol create: " << dlsym_error);
	}
	return createFunction;
}
void parsecommandline(int argc, char **argv)
{
	while (optind < argc)
	{
		int option = getopt(argc,argv,"w::h::d::?::");
		switch (option)
		{
			case 'd' :
				displayName = optarg;
				break;
			case 'w':
				displayWidth = atoi(optarg);
				break;
			case 'h':
				displayHeight = atoi(optarg);
				break;
			case '?':
			default:
				exit(-1);
		}
	}
}


std::string getfirstFileFromDirectory(std::string dirName){
	DIR *dp;
	std::string returnValue;
	struct dirent *dirp;
	if((dp  = opendir(dirName.c_str())) == NULL) {
		LOG_ERROR("Main", "Error(" << errno << ") opening " << dirName);
		returnValue = "";
	}else{

		while ((dirp = readdir(dp)) != NULL) {
			if (strcmp(dirp->d_name,".")==0)
				continue;
			if (strcmp(dirp->d_name,"..")==0)
				continue;
			if (strstr (dirp->d_name, ".so")==0)
				continue;
			LOG_INFO("Main", "loading file " << dirName << "/" << dirp->d_name);
			returnValue = std::string(dirName + "/" + dirp->d_name);
			break;
		}
		closedir(dp);
	}
	return returnValue;
	//error
}

BaseCommunicator* loadCommunicator(Layermanager* executor, ILayerList* layerList){
	std::string sharedLibraryName = getfirstFileFromDirectory("/usr/lib/layermanager/communicator");
	BaseCommunicator* (*createFunc)(Layermanager*, ILayerList*)  = getCreateFunction<BaseCommunicator*(Layermanager*, ILayerList*)>(sharedLibraryName);
	LOG_DEBUG("Main", "instantiating communicator");
	BaseCommunicator* ret = createFunc(executor, layerList);
	LOG_DEBUG("Main", "found communicator");
	return ret;
}

IRenderer* loadRenderer(LayerList* list)
{	IRenderer* ret = NULL;
	LOG_DEBUG("Main", "Searching for renderer.");
	std::string sharedLibraryName = getfirstFileFromDirectory("/usr/lib/layermanager/renderer");
	if (sharedLibraryName==""){
		LOG_ERROR("Main","No renderer found!");
	}else{
		LOG_DEBUG("Main", "Shared Renderer library " << sharedLibraryName << " found !");
		IRenderer* (*createFunc)(LayerList*)  = getCreateFunction<IRenderer*(LayerList*)>(sharedLibraryName);
		if ( NULL != createFunc )
		{
			LOG_DEBUG("Main", "Entry point of Renderer found !");
			ret = createFunc(list);
			if ( NULL == ret )
			{
				LOG_ERROR("Main","Render could not initiliazed. Entry Function not callable !");
			}
		} else {
			LOG_ERROR("Main","Render could not initiliazed. Entry Function not found !");
		}
	}
	return ret;
}


int main(int argc, char **argv)
{

	parsecommandline(argc,(char**)argv);
	LOG_INFO("Main", "Starting Layermanager.");

	// Create Singleton Instance of Layermanager
	Layermanager *manager = Layermanager::instance;

	LOG_INFO("Main", "Loading communicator.");
	// Create a communication mechanism to use
	BaseCommunicator* comm = loadCommunicator(manager,&manager->layerlist);
//comm->start();
//while(true)
//		{
//			sleep(2000);
//		}

	LOG_INFO("Main", "Loading renderer.");
	// Create Grafikcontroller to use
	IRenderer* renderer = loadRenderer(&manager->layerlist);

	// inform the layermanager about the renderer
	manager->addRenderer(renderer);

	// inform the layermanager about the communication class
	manager->addCommunicator(comm);
	if ( true == manager->startManagement(displayWidth,displayHeight,displayName) )
	{
		// must stay within main method or else application would completely exit
		LOG_INFO("Main", "Enter Mainloop.");
		while(true)
		{
			sleep(2000);
		}
	} else {
		LOG_ERROR("Main", "Exiting Application.");
		return -1;
	}
	LOG_INFO("Main", "Exiting Application.");
	// cleanup
	delete renderer;
	delete comm;
	delete manager;

	return 0;
}
