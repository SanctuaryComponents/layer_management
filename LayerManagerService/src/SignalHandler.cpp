/***************************************************************************
 * Copyright 2012 BMW Car IT GmbH
 * Copyright (C) 2012 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
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

#include "SignalHandler.h"
#include "Log.h"
#include "config.h"

#include <signal.h>
#include <stdlib.h>   // exit

#ifdef HAVE_EXECINFO_H
    #include <execinfo.h>
#endif



//===========================================================================
// Prototypes
//===========================================================================
static t_ilm_bool gShutdownReceived = ILM_FALSE;

//===========================================================================
// Prototypes
//===========================================================================
void incomingSignalCallback(int sig);

//===========================================================================
// class implementation
//===========================================================================
SignalHandler::SignalHandler()
{
    LOG_DEBUG("LayerManagerService", "Enabled signal handling.");
    signal(SIGBUS, incomingSignalCallback);
    signal(SIGSEGV, incomingSignalCallback);
    signal(SIGTERM, incomingSignalCallback);
    signal(SIGINT, incomingSignalCallback);
    signal(SIGABRT, incomingSignalCallback);
}

SignalHandler::~SignalHandler()
{
    LOG_DEBUG("LayerManagerService", "Disabled signal handling.");
    signal(SIGBUS, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
}

t_ilm_bool SignalHandler::shutdownSignalReceived()
{
    return gShutdownReceived;
}

//===========================================================================
// signal handling functions
//===========================================================================
void printStackTrace()
{
#ifdef HAVE_BACKTRACE
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
#endif

    LOG_INFO("LayerManagerService", "Exiting application.")
    exit(-1);
}

void incomingSignalCallback(int sig)
{
    switch (sig)
    {
        case SIGTERM:
            LOG_INFO("LayerManagerService", "Signal SIGTERM received. Shutting down.");
            gShutdownReceived = ILM_TRUE;
            break;
            
        case SIGINT:
            LOG_INFO("LayerManagerService", "Signal SIGINT received. Shutting down.");
            gShutdownReceived = ILM_TRUE;
            break;
            
        case SIGBUS:
            LOG_ERROR("LayerManagerService", "Signal SIGBUS received. Shutting down.");
            gShutdownReceived = ILM_TRUE;
            printStackTrace();
            break;
            
        case SIGSEGV:
            LOG_ERROR("LayerManagerService", "Signal SIGSEGV received. Shutting down.");
            gShutdownReceived = ILM_TRUE;
            printStackTrace();
            break;
            
        case SIGABRT:
            LOG_ERROR("LayerManagerService", "Signal SIGABRT received. Shutting down.");
            gShutdownReceived = ILM_TRUE;
            printStackTrace();
            break;
            
        default:
            LOG_INFO("LayerManagerService", "Signal " << sig << " received.");
            break;
    }
}
