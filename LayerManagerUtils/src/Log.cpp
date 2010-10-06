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
#include "Log.h"

Log* Log::instance = new Log();

Log::~Log() {
	// TODO Auto-generated destructor stub
	m_fileStream->close();
	Log::instance = NULL;
}
Log::Log() {
	// TODO Auto-generated constructor stub
	m_fileStream = new std::ofstream("log.txt");

}

void Log::warning (const std::string moduleName,const std::basic_string<char>& output )
{
	log(LOG_WARNING,moduleName,output);
}
void Log::info (const std::string moduleName,const std::basic_string<char>& output )
{
	log(LOG_INFO,moduleName,output);
}
void Log::error (const std::string moduleName,const std::basic_string<char>& output )
{
	log(LOG_ERROR,moduleName,output);
}
void Log::debug (const std::string moduleName,const std::basic_string<char>& output )
{
	log(LOG_DEBUG,moduleName,output);
}

void Log::log(LOG_MODES logMode, const std::string moduleName,const std::basic_string<char>& output )
{

	if (logMode == LOG_ERROR)
	{
		LogToConsole("ERROR",moduleName,output);
	}
	else if (logMode == LOG_DEBUG)
	{
		LogToConsole("DEBUG",moduleName,output);
	}
	else if (logMode == LOG_INFO)
	{
		LogToConsole("INFO",moduleName,output);
	}
	switch (logMode )
	{
			case LOG_ERROR : LogToFile("ERROR",moduleName,output); break;
			case LOG_WARNING : LogToFile("WARNING",moduleName,output); break;
			case LOG_INFO : LogToFile("INFO",moduleName,output); break;
			case LOG_DEBUG : LogToFile("DEBUG",moduleName,output); break;
	}

}
void Log::LogToFile(std::string logMode,const std::string moduleName,const std::basic_string<char>& output)
{
	*m_fileStream << "[" << moduleName << "][" << logMode << "] " << output << std::endl;
}
void Log::LogToConsole(std::string logMode,const std::string moduleName,const std::basic_string<char>& output)
{
	std::cout << "[" << moduleName << "][" << logMode << "] " <<  output << std::endl;
}
