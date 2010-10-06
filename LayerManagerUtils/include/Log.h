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

#ifndef _LOG_H_
#define _LOG_H_
#include "LogMessageBuffer.h"
#include <iostream>
#include <fstream>
typedef enum {
	LOG_INFO = 1,
	LOG_WARNING = 2,
	LOG_ERROR = 4,
	LOG_DEBUG = 8
} LOG_MODES;

class Log {

public:
	virtual ~Log();
	static Log* instance;
	void warning (const std::string moduleName, const std::basic_string<char>& output );
	void info (const std::string moduleName, const std::basic_string<char>& output );
	void error (const std::string moduleName, const std::basic_string<char>& output );
	void debug (const std::string moduleName, const std::basic_string<char>& output );
	void log(LOG_MODES logMode, const std::string moduleName, const std::basic_string<char>& output );
private:
	void LogToFile(std::string logMode, const std::string moduleName, const std::basic_string<char>& output);
	void LogToConsole(std::string logMode,const std::string moduleName,const std::basic_string<char>& output);
	Log();
	std::ofstream* m_fileStream;
};
#define LOG_ERROR(module, message) { \
           LogMessageBuffer oss_; \
           Log::instance->error(module, oss_.str(oss_<< message)); }
#define LOG_INFO(module, message) { \
           LogMessageBuffer oss_; \
           Log::instance->info(module, oss_.str(oss_<< message)); }
#define LOG_DEBUG(module, message) { \
           LogMessageBuffer oss_; \
           Log::instance->debug(module, oss_.str(oss_<< message)); }
#define LOG_WARNING(module, message) { \
           LogMessageBuffer oss_; \
           Log::instance->warning(module, oss_.str(oss_<< message)); }
#endif /* _LOG_H_ */
