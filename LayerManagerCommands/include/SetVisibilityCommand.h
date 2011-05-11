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

#ifndef _SETVISIBILITYCOMMAND_H_
#define _SETVISIBILITYCOMMAND_H_

#include "BaseCommandAsynchronous.h"
#include "ObjectType.h"

class SetVisibilityCommand : public BaseCommandAsynchronous
{
public:
    SetVisibilityCommand(const unsigned int givenid, ObjectType Objecttype, bool newvisibility);

    virtual ExecutionResult execute(ICommandExecutor* executor);
    virtual const std::string getString();

private:
    const unsigned int m_idtoSet;
    const ObjectType m_otype;
    const bool m_visibility;

    // for unit testing
    template <typename id_type, typename otype_type, typename visibility_type> friend class SetVisibilityCommandEqMatcherP3;
};


#endif /* _SETVISIBILITYCOMMAND_H_ */