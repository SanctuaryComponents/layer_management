/***************************************************************************
*   Copyright (C) 2011 BMW Car IT GmbH.
*   Author: Michael Schuldt (michael.schuldt@bmw-carit.de)
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
* This file incorporates work covered by the following copyright and
* permission notice:
* Apache log4cxx
* Copyright 2004-2007 The Apache Software Foundation
*
* Licensed to the Apache Software Foundation (ASF) under one or more
* contributor license agreements.  See the NOTICE file distributed with
* this work for additional information regarding copyright ownership.
* The ASF licenses this file to You under the Apache License, Version 2.0
* (the "License"); you may not use this file except in compliance with
* the License.  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#ifndef _LOG_H_
#define _LOG_H_

#include "LogMessageBuffer.h"
#include <iostream>
#include <fstream>
#include <pthread.h>

typedef enum {
    LOG_DISABLED = 0,
    LOG_ERROR = 1,
    LOG_INFO = 2,
    LOG_WARNING = 3,
    LOG_DEBUG = 4,
    LOG_MAX_LEVEL
} LOG_MODES;

class Log
{
public:
    virtual ~Log();
    static Log* instance;
    static LOG_MODES fileLogLevel;
    static LOG_MODES consoleLogLevel;
    void warning (const std::string& moduleName, const std::basic_string<char>& output);
    void info (const std::string& moduleName, const std::basic_string<char>& output);
    void error (const std::string& moduleName, const std::basic_string<char>& output);
    void debug (const std::string& moduleName, const std::basic_string<char>& output);
    void log(LOG_MODES logMode, const std::string& moduleName, const std::basic_string<char>& output);

private:
    Log();
    void LogToFile(std::string logMode, const std::string& moduleName, const std::basic_string<char>& output);
    void LogToConsole(std::string logMode,const std::string& moduleName,const std::basic_string<char>& output);

private:
    std::ofstream* m_fileStream;
    pthread_mutex_t m_LogBufferMutex;
};

//#define LOG_ERROR(module, message)

#define LOG_ERROR(module, message) { \
           LogMessageBuffer oss_; \
           Log::instance->error(module, oss_.str(oss_<< message)); }

//#define LOG_INFO(module, message)

#define LOG_INFO(module, message) { \
           LogMessageBuffer oss_; \
           Log::instance->info(module, oss_.str(oss_<< message)); }

//#define LOG_DEBUG(module, message)

#define LOG_DEBUG(module, message) { \
           LogMessageBuffer oss_; \
           Log::instance->debug(module, oss_.str(oss_<< message)); }

//#define LOG_WARNING(module, message)

#define LOG_WARNING(module, message) { \
           LogMessageBuffer oss_; \
           Log::instance->warning(module, oss_.str(oss_<< message)); }

#endif /* _LOG_H_ */
