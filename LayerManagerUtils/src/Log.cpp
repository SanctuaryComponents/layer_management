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

#include "Log.h"
#include <iomanip>

Log* Log::instance = new Log();

Log::Log()
{
    // TODO Auto-generated constructor stub
    m_fileStream = new std::ofstream("/tmp/LayerManagerService.log");

}

Log::~Log()
{
    // TODO Auto-generated destructor stub
    m_fileStream->close();
    Log::instance = NULL;
}

void Log::warning (const std::string& moduleName, const std::basic_string<char>& output)
{
    log(LOG_WARNING, moduleName, output);
}

void Log::info (const std::string& moduleName, const std::basic_string<char>& output)
{
    log(LOG_INFO, moduleName, output);
}

void Log::error (const std::string& moduleName, const std::basic_string<char>& output)
{
    log(LOG_ERROR, moduleName, output);
}

void Log::debug (const std::string& moduleName, const std::basic_string<char>& output)
{
    log(LOG_DEBUG, moduleName, output);
}

void Log::log(LOG_MODES logMode, const std::string& moduleName, const std::basic_string<char>& output)
{
    switch (logMode)
    {
        case LOG_ERROR:
            LogToConsole("ERROR", moduleName, output);
            LogToFile("ERROR", moduleName, output);
            break;

        case LOG_WARNING:
            LogToConsole("WARNING", moduleName, output);
            LogToFile("WARNING", moduleName, output);
            break;

        case LOG_INFO:
            LogToConsole("INFO", moduleName, output);
            LogToFile("INFO", moduleName, output);
            break;

        case LOG_DEBUG:
            LogToConsole("DEBUG", moduleName, output);
            LogToFile("DEBUG", moduleName, output);
            break;
    }
}

void Log::LogToFile(std::string logMode, const std::string& moduleName,const std::basic_string<char>& output)
{
    *m_fileStream << "[" << moduleName << "][" << logMode << "] " << output << std::endl;
}

void Log::LogToConsole(std::string logMode, const std::string& moduleName, const std::basic_string<char>& output)
{
    static unsigned int maxLengthModuleName = 0;
    static unsigned int maxLengthLogModeName = 0;

    if (moduleName.length() > maxLengthModuleName)
    {
        maxLengthModuleName = moduleName.length();
    }

    if (logMode.length() > maxLengthLogModeName)
    {
        maxLengthLogModeName = logMode.length();
    }

    std::cout << std::setw(maxLengthModuleName)  << std::left << moduleName << " | "
              << std::setw(maxLengthLogModeName) << std::left << logMode    << " | "
              << output << std::endl;
}
