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

#ifndef _DESTROYSHADERCOMMAND_H_
#define _DESTROYSHADERCOMMAND_H_

#include "BaseCommandSynchronous.h"
#include "LogicalGraphicsObject.h"
#include "Scene.h"
#include "LayerMap.h"
#include "SurfaceMap.h"


class DestroyShaderCommand : public BaseCommandSynchronous
{
public:
    DestroyShaderCommand(unsigned int shaderid);

    virtual ExecutionResult execute(ICommandExecutor* executor);
    virtual const std::string getString();

    unsigned int getShaderID() const;

private:
    /// detach shader from all surfaces, surface groups, etc... from surfaces
    void removeShaderFromAllSurfaces(const SurfaceMap & surfaceMap, Shader* shader);

    void removeShaderFromAllLayers(const LayerMap & layerMap, Shader *& shader);

private:
    const unsigned int m_id;
};

#endif /* _DESTROYSHADERCOMMAND_H_ */
