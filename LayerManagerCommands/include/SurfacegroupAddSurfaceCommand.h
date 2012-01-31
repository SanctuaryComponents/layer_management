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

#ifndef _SURFACEGROUPADDSURFACECOMMAND_H_
#define _SURFACEGROUPADDSURFACECOMMAND_H_

#include "BaseCommandAsynchronous.h"

class SurfacegroupAddSurfaceCommand : public BaseCommandAsynchronous
{
public:
    /*!
     * \action    This command adds a surface to a surface group within the
     *            GENIVI LayerManagement
     * \frequency Called within initializations in order to change properties of
     *            all group members at once at a later point in time, not needed
     *            for every surface.
     * \param[in] surfacegroupid id of surface group
     * \param[in] surfaceid id of surface
     * \ingroup Commands
     */
    SurfacegroupAddSurfaceCommand(unsigned int surfacegroupid, unsigned int surfaceid);

    /**
     * \brief default destructor
     */
    virtual ~SurfacegroupAddSurfaceCommand() {}

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
    const unsigned int m_surfacegroupid;
    const unsigned int m_surfaceid;

    // for unit testing
    template <typename surfaceid_type, typename surfacegroupid_type> friend class SurfacegroupAddSurfaceCommandEqMatcherP2;
};

#endif /* _SURFACEGROUPADDSURFACECOMMAND_H_ */
