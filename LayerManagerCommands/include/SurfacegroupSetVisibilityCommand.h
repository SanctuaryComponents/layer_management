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

#ifndef _SURFACEGROUPSETVISIBILITYCOMMAND_H_
#define _SURFACEGROUPSETVISIBILITYCOMMAND_H_

#include "ICommand.h"

class SurfacegroupSetVisibilityCommand : public ICommand
{
public:
    /*!
     * \action    This command sets the visibility of a surface group within the GENIVI LayerManagement
     * \frequency Used for surface group management.
     * \param[in] sender process id of application that sent this command
     * \param[in] givenid id ofsurface group
     * \param[in] newvisibility TRUE: all surfaces in group visible, FALSE: all surface in group invisible
     * \ingroup Commands
     */
    SurfacegroupSetVisibilityCommand(pid_t sender, const unsigned int givenid, bool newvisibility)
    : ICommand(ExecuteAsynchronous, sender)
    , m_idtoSet(givenid)
    , m_visibility(newvisibility)
    {}

    /**
     * \brief default destructor
     */
    virtual ~SurfacegroupSetVisibilityCommand() {}

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
    const unsigned int m_idtoSet;
    const bool m_visibility;

    // for unit testing
    template <typename id_type, typename visibility_type> friend class SurfacegroupSetVisibilityCommandEqMatcherP2;
};


#endif /* _SURFACEGROUPSETVISIBILITYCOMMAND_H_ */
