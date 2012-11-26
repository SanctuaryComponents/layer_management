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

#ifndef __HEALTHSYSTEMD_H__
#define __HEALTHSYSTEMD_H__

#include "IHealth.h"
#include "ThreadBase.h"

class HealthSystemd : public IHealth, protected ThreadBase
{
public:
    HealthSystemd(ICommandExecutor& executor, Configuration& config);

    // from IHealth
    virtual t_ilm_bool start();
    virtual t_ilm_bool stop();

private:
    void reportStartupComplete();
    void reportProcessId();
    int getWatchdogIntervalInMs();
    void signalWatchdog();
    bool watchdogEnabled();

    // from ThreadBase
    virtual t_ilm_bool threadMainLoop();
    
private:
    int mIntervalInMs;
};

#endif // __HEALTHSYSTEMD_H__