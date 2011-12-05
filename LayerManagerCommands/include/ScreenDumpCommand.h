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

#ifndef _SCREENDUMPCOMMAND_H_
#define _SCREENDUMPCOMMAND_H_

#include "BaseCommandSynchronous.h"
#include <string>

class ScreenDumpCommand: public BaseCommandSynchronous
{
public:
    ScreenDumpCommand(char* givenfilename, unsigned int id = 0);
    virtual ~ScreenDumpCommand() {}

    virtual ExecutionResult execute(ICommandExecutor* executor);
    virtual const std::string getString();

private:
    std::string m_filename;
    const unsigned int m_id;
};

#endif /* _SCREENDUMPCOMMAND_H_ */
