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

#ifndef _COMMANDEXECUTOR_H_
#define _COMMANDEXECUTOR_H_

#include "RendererList.h"
#include "CommunicatorList.h"
#include "LayerType.h"

class ICommand;
class Scene;

/**
 * Objects who implement this interface can be used to have command objects executed. Communication
 * classes must only know this interface of a class to be able to pass along command objects.
 *
 * Implementation detail: This interface is used to reduce dependency of communicators on the main layermanagement component
 */
class ICommandExecutor
{
public:
    /**
     * Have a command processed.
     * @param commandToBeExecuted The command to be processed
     */
    virtual bool execute(ICommand* commandToBeExecuted) = 0;

    virtual bool startManagement(const int width, const int height, const char* displayName) = 0;
    virtual bool stopManagement() = 0;

    virtual Scene* getScene(void) = 0;

    // renderer management
    virtual RendererList* getRendererList(void) = 0;
    virtual void addRenderer(IRenderer* renderer) = 0;
    virtual void removeRenderer(IRenderer* renderer) = 0;

    // communicator management
    virtual CommunicatorList* getCommunicatorList(void) = 0;
    virtual void addCommunicator(ICommunicator* communicator) = 0;
    virtual void removeCommunicator(ICommunicator* communicator) = 0;

    virtual unsigned int getLayerTypeCapabilities(const LayerType layertype) const = 0; // TODO: removeable, use default command processing ?
    virtual unsigned int getNumberOfHardwareLayers(const unsigned int screenID) const = 0; // TODO: removeable, use default command processing ?
    virtual unsigned int* getScreenResolution(const unsigned int screenID) const = 0; // TODO: removeable, use default command processing ?
    virtual unsigned int* getScreenIDs(unsigned int* length) const = 0; // TODO: removeable, use default command processing ?
};

#endif /* _COMMANDEXECUTOR_H_ */
