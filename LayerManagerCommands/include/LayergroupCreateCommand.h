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

#ifndef _LAYERGROUPCREATECOMMAND_H_
#define _LAYERGROUPCREATECOMMAND_H_

#include "ICommand.h"
#include "IScene.h"

class LayergroupCreateCommand : public ICommand
{
public:
    /*!
     * \action    This command creates a layer group within the GENIVI LayerManagement
     * \frequency Called within initializations in order to change properties of all
     *            group members at once at a later point in time.
     * \param[in] sender process id of application that sent this command
     * \param[in] idReturn location to store layer group id on execution;
     *                     pre-initialized value will be interpreted as id request
     * \ingroup Commands
     */
    LayergroupCreateCommand(pid_t sender, uint* idReturn)
    : ICommand(ExecuteSynchronous, sender)
    , m_idReturn(idReturn)
    {}

    /**
     * \brief default destructor
     */
    virtual ~LayergroupCreateCommand() {}

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
    uint* m_idReturn;
};


#endif /* _CREATECOMMAND_H_ */
