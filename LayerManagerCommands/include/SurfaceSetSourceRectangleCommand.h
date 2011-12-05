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

#ifndef _SURFACESETSOURCERECTANGLECOMMAND_H_
#define _SURFACESETSOURCERECTANGLECOMMAND_H_

#include "BaseCommandAsynchronous.h"

class SurfaceSetSourceRectangleCommand : public BaseCommandAsynchronous
{
public:
	SurfaceSetSourceRectangleCommand(int id, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

    virtual ExecutionResult execute(ICommandExecutor* executor);
    virtual const std::string getString();

private:
    const unsigned int m_id;
    const unsigned int m_x;
    const unsigned int m_y;
    const unsigned int m_width;
    const unsigned int m_height;

    // for unit testing
    template <typename id_type, typename x_type, typename y_type, typename width_type, typename height_type> friend class SurfaceSetSourceRectangleCommandEqMatcherP5;
};


#endif /* _SURFACESETSOURCERECTANGLECOMMAND_H_ */