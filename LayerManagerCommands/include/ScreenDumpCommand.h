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
    /*!
     * \action    This command stores a bitmap file with the graphical content
     *            of a screen within the GENIVI LayerManagement
     * \frequency Used for screen management.
     * \param[in] givenfilename path and filename to store bitmap file
     * \param[in] id
     * \ingroup Commands
     */
    ScreenDumpCommand(char* givenfilename, unsigned int id = 0);

    /**
     * \brief default destructor
     */
    virtual ~ScreenDumpCommand() {}

    /**
     * \brief Execute this command.
     * \param[in] executor Pointer to instance executing the LayerManagement COmmands
     * \return ExecutionSuccess: execution successful
     * \return ExecutionSuccessRedraw: execution successful and screen needs to be redrawn
     * \return ExecutionFailed: execution failed
     * \return ExecutionFailedRedraw: execution unsuccessful and screen needs to be redrawn
     */
    virtual ExecutionResult execute(ICommandExecutor* executor);

    /**
     * \brief Get description string for this command.
     * \return String object with description of this command object
     */
    virtual const std::string getString();

private:
    std::string m_filename;
    const unsigned int m_id;
};

#endif /* _SCREENDUMPCOMMAND_H_ */
