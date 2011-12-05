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

#ifndef _SURFACESETSHADERCOMMAND_H_
#define _SURFACESETSHADERCOMMAND_H_

#include "BaseCommandSynchronous.h"

class SurfaceSetShaderCommand : public BaseCommandSynchronous
{
public:
    SurfaceSetShaderCommand(unsigned int id,unsigned  int shaderid);
    virtual ~SurfaceSetShaderCommand() {}

    virtual ExecutionResult execute(ICommandExecutor* executor);
    virtual const std::string getString();

    unsigned int getID() const;
    unsigned int getShaderID() const;

private:
    const unsigned int m_id;
    const unsigned int m_shaderid;
};

#endif /* _SURFACESETSHADERCOMMAND_H_ */

