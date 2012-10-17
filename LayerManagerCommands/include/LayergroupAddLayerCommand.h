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

#ifndef _LAYERGROUPADDLAYERCOMMAND_H_
#define _LAYERGROUPADDLAYERCOMMAND_H_

#include "ICommand.h"

class LayergroupAddLayerCommand : public ICommand
{
public:
    /*!
     * \action    This command adds a layer to a layer group within the GENIVI LayerManagement
     * \frequency Called within initialisations in order to change properties of all group
     *            members at once at a later point in time, not needed for every layer.
     * \param[in] sender process id of application that sent this command
     * \param[in] layergroupid id of layer group
     * \param[in] layerid id of layer
     * \ingroup Commands
     */
    LayergroupAddLayerCommand(pid_t sender, unsigned int layergroupid, unsigned int layerid)
    : ICommand(ExecuteAsynchronous, sender)
    , m_layergroupid(layergroupid)
    , m_layerid(layerid)
    {}

    /**
     * \brief default destructor
     */
    virtual ~LayergroupAddLayerCommand() {}

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
    template <typename layerid_type, typename layergroupid_type> friend class LayergroupAddLayerCommandEqMatcherP2;
};


#endif /* _LAYERGROUPADDLAYERCOMMAND_H_ */
