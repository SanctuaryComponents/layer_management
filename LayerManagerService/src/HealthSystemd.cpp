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
#include "HealthSystemd.h"
#include "config.h"
#include "Log.h"
#include <unistd.h>
#include <stdlib.h>

#ifdef WITH_SYSTEMD
    #include <systemd/sd-daemon.h>
#else
    #define sd_notify(a,b)
    #define sd_notifyf(a,b,c)
#endif


HealthSystemd::HealthSystemd()
: mIntervalInMs(-1)
{
    char* envVar = getenv("WATCHDOG_USEC");
    if (envVar)
    {
        // to ms, watchdog should trigger twice the frequency
        mIntervalInMs = atoi(envVar) / 2000;
        LOG_INFO("HealthSystemd", "Watchdog interval is " << mIntervalInMs << "ms");
    }
    else
    {
        LOG_INFO("HealthSystemd", "Watchdog interval not defined, watchdog disabled");
    }
}

void HealthSystemd::reportStartupComplete()
{
    LOG_INFO("HealthSystemd", "reporting startup complete");
    sd_notify(0, "READY=1");
}

void HealthSystemd::reportProcessId()
{
    if (watchdogEnabled())
    {
        sd_notifyf(0, "MAINPID=%d", getpid());
    }
}

int HealthSystemd::getWatchdogIntervalInMs()
{
    return mIntervalInMs;
}

void HealthSystemd::signalWatchdog()
{
    if (watchdogEnabled())
    {
        LOG_DEBUG("HealthSystemd", "Watchdog fired");
        sd_notify(0, "WATCHDOG=1");
    }
}

bool HealthSystemd::watchdogEnabled()
{
    return (mIntervalInMs > 0);
}
