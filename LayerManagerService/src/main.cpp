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
#include "Configuration.h"
#include "SignalHandler.h"
#include "IRenderer.h"
#include "ICommunicator.h"
#include "ISceneProvider.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

int main(int argc, char **argv)
{
    // collect all configuration settings
    Configuration configuration(argc, argv);

    // setup logging
    Log::consoleLogLevel = (LOG_MODES)configuration.getLogLevelConsole();
    Log::fileLogLevel = (LOG_MODES)configuration.getLogLevelFile();
    Log::dltLogLevel = (LOG_MODES)configuration.getLogLevelTrace();

    LOG_INFO("LayerManagerService", "Starting Layermanager (version: " << ILM_VERSION << ")");
    
    // log configuration options
    configuration.logAllSettings();

    {
        SignalHandler signalHandler;
        Layermanager layermanager(configuration);

        if (layermanager.startManagement())
        {
            LOG_INFO("LayerManagerService", "Startup complete. EnterMainloop");

            while (!signalHandler.shutdownSignalReceived())
            {
                CommunicatorListIterator commIter = layermanager.getCommunicatorList()->begin();
                CommunicatorListIterator commIterEnd = layermanager.getCommunicatorList()->end();
                for (; commIter != commIterEnd; ++commIter)
                {
                    (*commIter)->process(-1);
                }
            }
            LOG_DEBUG("LayerManagerService", "Stopping service.");
            layermanager.stopManagement();
        }
    }

    LOG_INFO("LayerManagerService", "Shutdown complete.");
    Log::closeInstance();
    return 0;
}

