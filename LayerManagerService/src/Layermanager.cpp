/***************************************************************************
 *
 * Copyright 2010,2011 BMW Car IT GmbH
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

#include "Layermanager.h"
#include "Log.h"
#include <iomanip>
#include "LayerList.h"
#include "ICommand.h"
#include "ICommunicator.h"
#include "IRenderer.h"
#include "ISceneProvider.h"

Layermanager::Layermanager()
{
    m_pScene = new Scene();
    m_pRendererList = new RendererList();
    m_pCommunicatorList = new CommunicatorList();
    m_pSceneProviderList = new SceneProviderList();
    m_pApplicationReferenceMap = new ApplicationReferenceMap();
}

Layermanager::~Layermanager()
{
    if (m_pScene)
    {
        delete m_pScene;
    }

    if (m_pRendererList)
    {
        delete m_pRendererList;
    }

    if (m_pCommunicatorList)
    {
        delete m_pCommunicatorList;
    }
}

uint Layermanager::getLayerTypeCapabilities(const LayerType layertype) const
{
    uint capabilities = 0;

    // get the first renderer for now TODO
    // change to possibly load multiple renderers

    IRenderer* renderer = *m_pRendererList->begin();
    if (renderer)
    {
        capabilities = renderer->getLayerTypeCapabilities(layertype);
    }

    return capabilities;
}

uint Layermanager::getNumberOfHardwareLayers(const uint screenID) const
{
    uint numberOfHardwareLayers = 0;

    // get the first renderer for now TODO
    // change to possibly load multiple renderers
    IRenderer* renderer = *m_pRendererList->begin();

    if (renderer)
    {
        numberOfHardwareLayers = renderer->getNumberOfHardwareLayers(screenID);
    }

    return numberOfHardwareLayers;
}

uint* Layermanager::getScreenResolution(const uint screenID) const
{
    uint* returnValue = 0;

    // get the first renderer for now TODO
    // change to possibly load multiple renderers
    IRenderer* renderer = *m_pRendererList->begin();

    if (renderer)
    {
        returnValue = renderer->getScreenResolution(screenID);
    }

    return returnValue;
}

uint* Layermanager::getScreenIDs(uint* length) const
{
    uint* returnValue = 0;

    // get the first renderer for now TODO
    // change to possibly load multiple renderers
    IRenderer* renderer = *m_pRendererList->begin();
    if (renderer)
    {
        returnValue = renderer->getScreenIDs(length);
    }

    return returnValue;
}

// This function is used to propagate a signal to the window system
// via the renderer for a redraw because of a rendering property change
void Layermanager::signalRendererRedraw()
{
	IRenderer* renderer = *m_pRendererList->begin();
	if (NULL!=renderer)
		renderer->signalWindowSystemRedraw();
}

void Layermanager::addRenderer(IRenderer* renderer)
{
    if (renderer)
    {
        m_pRendererList->push_back(renderer);
    }

}

void Layermanager::removeRenderer(IRenderer* renderer)
{
    if (renderer)
    {
        m_pRendererList->remove(renderer);
    }
}

void Layermanager::addSceneProvider(ISceneProvider* sceneProvider)
{
    if (sceneProvider)
    {
        m_pSceneProviderList->push_back(sceneProvider);
    }

}

void Layermanager::removeSceneProvider(ISceneProvider* sceneProvider)
{
    if (sceneProvider)
    {
        m_pSceneProviderList->remove(sceneProvider);
    }
}
void Layermanager::addApplicationReference(IApplicationReference* reference)
{
    if (reference)
    {
        LOG_INFO("LayerManagerService", "Connect from application hash :" << reference->getApplicationHash() << " pid : " << reference->getProcessId()); 
        (*m_pApplicationReferenceMap)[reference->getApplicationHash()]=reference;
    }

}

void Layermanager::removeApplicationReference(IApplicationReference* reference)
{
    if (reference)
    {
        LOG_INFO("LayerManagerService", "Disconnect from application with hash :" << reference->getApplicationHash() << " pid : " << reference->getProcessId());         
        m_pApplicationReferenceMap->erase(reference->getApplicationHash());
    }
}



void Layermanager::addCommunicator(ICommunicator* communicator)
{
    if (communicator)
    {
        m_pCommunicatorList->push_back(communicator);
    }
}

void Layermanager::removeCommunicator(ICommunicator* communicator)
{
    if (communicator)
    {
        m_pCommunicatorList->remove(communicator);
    }
}

void Layermanager::printDebugInformation() const
{
    // print stuff about layerlist
    LOG_INFO("LayerManagerService", "Layer: ID |  X |  Y |  W |  H |Al.| Z \n");

    unsigned int length;
    unsigned int* LayerIDs;

    // loop the layers
    m_pScene->getLayerIDs(&length, &LayerIDs);

    for (unsigned int i = 0; i < length; i++)
    {
        Layer* currentLayer = m_pScene->getLayer(LayerIDs[i]);
        LOG_INFO("LayerManagerService", "      " << std::setw(4) << currentLayer->getID() << "\n");

        LOG_INFO("LayerManagerService", "    Surface:  ID |Al.| Z |  SVP: X |  Y |  W |  H     DVP:  X |  Y |  W |  H \n");

        // loop the surfaces of within each layer
        SurfaceListConstIterator iter = currentLayer->getAllSurfaces().begin();
        SurfaceListConstIterator iterEnd = currentLayer->getAllSurfaces().end();

        for (; iter != iterEnd; ++iter)
        {
            LOG_INFO("LayerManagerService", "            " << std::setw(4) << (*iter)->getID() << std::setw(4) << std::setprecision(3) << (*iter)->opacity << "\n");
        }
    }
}

bool Layermanager::executeCommand(ICommand* commandToBeExecuted)
{
    ExecutionResult status = ExecutionFailed;
    LOG_INFO("LayerManagerService", "executing " << commandToBeExecuted->getString());
    m_pScene->lockScene();
    status = commandToBeExecuted->execute(this);
    m_pScene->unlockScene();

    if (status == ExecutionFailedRedraw || status == ExecutionSuccessRedraw)
    {
    	// either a command that has changed the output was executed, or
    	// commit command was executed which contained commands that requested redraw was executed.
    	signalRendererRedraw();
    }

    return (status == ExecutionSuccess || status == ExecutionSuccessRedraw);
}

bool Layermanager::enqueueCommand(ICommand* commandToBeExecuted)
{
    unsigned int sizeBefore;
    unsigned int sizeAfter;
    LOG_DEBUG("LayerManagerService", "add command to queue: " << commandToBeExecuted->getString());
    m_pScene->lockScene();
    sizeBefore = m_pScene->m_toBeCommittedList.size();
    m_pScene->m_toBeCommittedList.push_back(commandToBeExecuted);
    sizeAfter = m_pScene->m_toBeCommittedList.size();
    m_pScene->unlockScene();

    // if queue size increased by 1, enqueue command was successful
    return (sizeAfter == sizeBefore + 1);
}

bool Layermanager::execute(ICommand* commandToBeExecuted)
{
    bool status = false;

    ExecutionType type = commandToBeExecuted->getExecutionType();

    switch (type)
    {
    case ExecuteSynchronous:
        status = executeCommand(commandToBeExecuted);
        delete commandToBeExecuted;
        break;

    case ExecuteAsynchronous:
        status = enqueueCommand(commandToBeExecuted);
        break;

    default:
        // TODO: error
        LOG_ERROR("LayerManagerService", "Command " << commandToBeExecuted->getString() << " was not processed");
        break;
    }

    return status;
}

bool Layermanager::startAllRenderers(const int width, const int height,
        const char *displayName)
{
    bool allStarted = false;

    RendererListIterator iter = m_pRendererList->begin();
    RendererListIterator iterEnd = m_pRendererList->end();
    for (; iter != iterEnd; ++iter)
    {
        IRenderer* renderer = *iter;
        if (renderer)
        {
            allStarted = renderer->start(width, height, displayName);
        }
        if (renderer == NULL || allStarted == false)
        {
          break;   
        }
    }
    if (!allStarted)
    {
        LOG_ERROR("LayerManagerService","Could not start Compositing Controllers");
    }
    return allStarted;
}

bool Layermanager::delegateScene()
{
    bool allStarted = true;

    SceneProviderListIterator iter = m_pSceneProviderList->begin();
    SceneProviderListIterator iterEnd = m_pSceneProviderList->end();

    for (; iter != iterEnd; ++iter)
    {
        ISceneProvider* sceneProvider = *iter;
        if (sceneProvider)
        {
            allStarted &= sceneProvider->delegateScene();
        }
        else
        {
            allStarted = false;
        }
    }
    if (!allStarted)
    {
        LOG_WARNING("LayerManagerService","Could not start delegate Scenes, initial Scene is lost");
    }
    return allStarted;
}

bool Layermanager::startAllCommunicators()
{
    bool allStarted = true;

    CommunicatorListIterator communicatorIter = m_pCommunicatorList->begin();
    CommunicatorListIterator communicatorIterEnd = m_pCommunicatorList->end();

    for (; communicatorIter != communicatorIterEnd; ++communicatorIter)
    {
        ICommunicator *communicator = *communicatorIter;
        if (communicator)
        {
            allStarted &= communicator->start();
        }
        else
        {
            allStarted = false;
        }
    }
    if (!allStarted)
    {
        LOG_ERROR("LayerManagerService","Could not start Communication Controllers");
    }
    return allStarted;
}

bool Layermanager::startManagement(const int width, const int height,
        const char* displayName)
{
    bool result = false;

    // 1. start renderers, no prerequisites
    // 2. execute scene provider plugins
    // 3. start communication (after scene is ready to use)
    result = startAllRenderers(width, height, displayName)
             && delegateScene()
             && startAllCommunicators();

    return result;
}

void Layermanager::stopAllRenderers()
{
    RendererListIterator iter = m_pRendererList->begin();
    RendererListIterator iterEnd = m_pRendererList->end();
    for (; iter != iterEnd; ++iter)
    {
        IRenderer *renderer = *iter;
        if (renderer)
        {
            renderer->stop();
        }
    }
}

void Layermanager::stopAllCommunicators()
{
    CommunicatorListIterator iter = m_pCommunicatorList->begin();
    CommunicatorListIterator iterEnd = m_pCommunicatorList->end();
    for (; iter != iterEnd; ++iter)
    {
        ICommunicator *communicator = *iter;
        if (communicator)
        {
            communicator->stop();
        }
    }
}

bool Layermanager::stopManagement()
{
    stopAllRenderers();
    stopAllCommunicators();
    return true; // TODO
}

