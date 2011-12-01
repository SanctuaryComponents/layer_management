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

#ifndef _SURFACESETORIENTATIONCOMMAND_H_
#define _SURFACESETORIENTATIONCOMMAND_H_

#include "BaseCommandAsynchronous.h"
#include "OrientationType.h"

class SurfaceSetOrientationCommand : public BaseCommandAsynchronous
{
public:
	SurfaceSetOrientationCommand(unsigned int id, OrientationType Orientation);

    virtual ExecutionResult execute(ICommandExecutor* executor);
    virtual const std::string getString();

private:
    const unsigned int m_id;
    const OrientationType m_orientation;

    // for unit testing
    template <typename id_type, typename Orientation_type> friend class SurfaceSetOrientationCommandEqMatcherP2;
};


#endif /* _SURFACESETORIENTATIONCOMMAND_H_ */
