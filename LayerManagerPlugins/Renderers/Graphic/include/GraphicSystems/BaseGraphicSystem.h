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

#ifndef _BASEGRAPHICSYSTEM_H_
#define _BASEGRAPHICSYSTEM_H_

#include "TextureBinders/ITextureBinder.h"
#include "WindowSystems/BaseWindowSystem.h"
#include "PlatformSurface.h"
#include "Surface.h"
#include "Layer.h"

template<class DisplayType, class WindowType>
class BaseGraphicSystem
{
public:
    virtual bool init(DisplayType display, WindowType window)=0;
    virtual ~BaseGraphicSystem()
    {
    };
    virtual void beginLayer(Layer* layer) = 0;
    virtual void endLayer() = 0;

    virtual bool needsRedraw(Layer *layer) = 0;
    virtual bool needsRedraw(LayerList layers) = 0;
    virtual void renderSWLayer(Layer* layer, bool clear) = 0;
    virtual void renderSWLayers(LayerList layers, bool clear) = 0;

    virtual void setBaseWindowSystem(BaseWindowSystem* windowSystem)
    {
        m_baseWindowSystem = windowSystem;
    }

    virtual void activateGraphicContext() = 0;
    virtual void releaseGraphicContext() = 0;
    virtual void clearBackground() = 0;
    virtual void swapBuffers() = 0;
    virtual void saveScreenShotOfFramebuffer(std::string fileToSave) = 0;

    virtual void setTextureBinder(ITextureBinder* binder)
    {
        m_binder = binder;
    }
    virtual ITextureBinder* getTextureBinder()
    {
        return m_binder;
    }
    virtual void renderSurface(Surface*)=0;

protected:
    BaseWindowSystem* m_baseWindowSystem;
    ITextureBinder* m_binder;
};

#endif /* _BASEGRAPHICSYSTEM_H_ */
