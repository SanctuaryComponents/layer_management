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

#ifndef _LAYERMANAGER_H_
#define _LAYERMANAGER_H_

#include "ICommandExecutor.h"
#include "NotificationQueue.h"
#include "Scene.h"

class ICommand;
class IRenderer;
class ICommunicator;
class ISceneProvider;
class IApplicationReference;

class Layermanager: public ICommandExecutor
{
public:
    Layermanager();
    virtual ~Layermanager();

    void signalRendererRedraw();

    virtual bool execute(ICommand* commandToBeExecuted);
    virtual uint getLayerTypeCapabilities(const LayerType layertype) const;
    virtual uint getNumberOfHardwareLayers(const uint screenID) const;
    virtual uint* getScreenResolution(const uint screenID) const;
    virtual uint* getScreenIDs(uint* length) const;

    virtual void addRenderer(IRenderer* renderer);
    virtual void removeRenderer(IRenderer* renderer);

    virtual void addCommunicator(ICommunicator* communicator);
    virtual void removeCommunicator(ICommunicator* communicator);

    virtual void addSceneProvider(ISceneProvider* sceneProvider);
    virtual void removeSceneProvider(ISceneProvider* sceneProvider);

    virtual void addApplicationReference(t_ilm_client_handle client, IApplicationReference* reference);
    virtual void removeApplicationReference(t_ilm_client_handle client);
    virtual t_ilm_uint getSenderPid(t_ilm_client_handle client);
    
    virtual bool startManagement(const int width, const int height, const char* displayName);
    virtual bool stopManagement();

    virtual Scene* getScene(void);
    virtual RendererList* getRendererList(void);
    virtual CommunicatorList* getCommunicatorList(void);
    virtual SceneProviderList* getSceneProviderList(void);

    virtual void addClientNotification(GraphicalObject* object, t_ilm_notification_mask mask);
    virtual NotificationQueue& getClientNotificationQueue();
    virtual ApplicationReferenceMap* getApplicationReferenceMap(void);

private:
    void printDebugInformation() const;
    bool startAllRenderers(const int width, const int height, const char *displayName);
    bool startAllCommunicators();
    bool delegateScene();
    void stopAllRenderers();
    void stopAllCommunicators();
    bool executeCommand(ICommand* commandToBeExecuted);
    bool enqueueCommand(ICommand* commandToBeExecuted);

private:
    Scene* m_pScene;
    RendererList* m_pRendererList;
    CommunicatorList* m_pCommunicatorList;
    SceneProviderList* m_pSceneProviderList;
    NotificationQueue m_clientNotificationQueue;
    ApplicationReferenceMap* m_pApplicationReferenceMap;
};

inline Scene* Layermanager::getScene(void)
{
    return m_pScene;
}

inline RendererList* Layermanager::getRendererList(void)
{
    return m_pRendererList;
}

inline CommunicatorList* Layermanager::getCommunicatorList(void)
{
    return m_pCommunicatorList;
}

inline SceneProviderList* Layermanager::getSceneProviderList(void)
{
    return m_pSceneProviderList;
}

inline ApplicationReferenceMap* Layermanager::getApplicationReferenceMap(void)
{
    return m_pApplicationReferenceMap;
}

inline void Layermanager::addClientNotification(GraphicalObject* object, t_ilm_notification_mask newMask)
{
    if (m_clientNotificationQueue.find(object) != m_clientNotificationQueue.end())
    {
        m_clientNotificationQueue[object] = (t_ilm_notification_mask)(m_clientNotificationQueue[object] | newMask);
    }
    else
    {
        m_clientNotificationQueue[object] = newMask;
    }
}

inline NotificationQueue& Layermanager::getClientNotificationQueue()
{
    return m_clientNotificationQueue;
}

#endif /* _LAYERMANAGER_H_ */
