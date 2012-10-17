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

#ifndef _LAYERGROUPREMOVELAYERCOMMAND_H_
#define _LAYERGROUPREMOVELAYERCOMMAND_H_

#include "ICommand.h"

class LayergroupRemoveLayerCommand : public ICommand
{
public:
    /*!
     * \action    This command removes a layer from a layer group within the GENIVI LayerManagement
     * \frequency Infrequent.
     * \param[in] sender process id of application that sent this command
     * \param[in] layergroupid id of layer group
     * \param[in] layerid id lf layer
     * \ingroup Commands
     */
    LayergroupRemoveLayerCommand(pid_t sender, unsigned int layergroupid, unsigned int layerid)
    : ICommand(ExecuteSynchronous, sender)
    , m_layergroupid(layergroupid)
    , m_layerid(layerid)
    {}

    /**
     * \brief default destructor
     */
    virtual ~LayergroupRemoveLayerCommand() {}

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
    const unsigned  m_layergroupid;
    const unsigned  m_layerid;

    // for unit testing
    template <typename layerid_type, typename layergroupid_type> friend class LayergroupRemoveLayerCommandEqMatcherP2;
};

#endif /* _LAYERGROUPREMOVELAYERCOMMAND_H_ */
