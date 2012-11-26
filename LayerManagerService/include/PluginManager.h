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
#ifndef __PLUGINMANAGER_H__
#define __PLUGINMANAGER_H__

#include "ilm_types.h"
#include "RendererList.h"
#include "CommunicatorList.h"
#include "SceneProviderList.h"
#include "HealthMonitorList.h"

class ICommandExecutor;
class Configuration;

class PluginManager
{
public:
    PluginManager(ICommandExecutor& executor, Configuration& config);
    ~PluginManager();
    
    RendererList* getRendererList();
    CommunicatorList* getCommunicatorList();
    SceneProviderList* getSceneProviderList();
    HealthMonitorList* getHealthMonitorList();

private:
    void createAndStartAllPlugins();
    void stopAndDestroyAllPlugins();
    
private:
    ICommandExecutor& mExecutor;
    Configuration& mConfiguration;
    RendererList mRendererList;
    CommunicatorList mCommunicatorList;
    SceneProviderList mSceneProviderList;
    HealthMonitorList mHealthMonitorList;
};

#endif // __PLUGINMANAGER_H__
