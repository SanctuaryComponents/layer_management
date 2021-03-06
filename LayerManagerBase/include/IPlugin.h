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

#ifndef __IPLUGIN_H__
#define __IPLUGIN_H__

#include "ilm_types.h"

#define REGISTER_PLUGIN(PLUGIN) \
    extern "C" IPlugin* create ## PLUGIN(ICommandExecutor& executor, Configuration& config); \
    static bool PLUGIN ## _instance = PluginManager::registerStaticPluginCreateFunction(create ## PLUGIN);

#define DECLARE_LAYERMANAGEMENT_PLUGIN(name) \
extern "C" IPlugin* create ## name(ICommandExecutor& executor, Configuration& config) \
{ \
    return new name(executor, config); \
}

class IPlugin
{
public:
    virtual PluginApi pluginGetApi() const = 0;
    virtual t_ilm_const_string pluginGetName() const = 0;
    virtual HealthCondition pluginGetHealth() = 0;

    virtual ~IPlugin() {}
};

#endif // __IPLUGIN_H__
