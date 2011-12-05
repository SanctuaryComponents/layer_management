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
#ifndef _ICOMMAND_H_
#define _ICOMMAND_H_

#include <string>
#include "ExecutionType.h"

class ICommandExecutor;

class ICommand
{
public:
    virtual ~ICommand() {}

    virtual ExecutionResult execute(ICommandExecutor* executor) = 0;

    virtual ExecutionType getExecutionType() = 0;

    virtual const std::string getString() = 0;
};

#endif // _ICOMMAND_H_
