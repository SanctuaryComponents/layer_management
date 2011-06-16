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

#ifndef BEAGLERENDERER_H_
#define BEAGLERENDERER_H_

#include "BaseRenderer.h"
#include "TextureBinders/BeagleEglImage.h"
#include "GraphicSystems/GLESGraphicSystem.h"
#include "WindowSystems/NullWindowSystem.h"
#include "ShaderProgramBeagle.h"

class BeagleRenderer : public BaseRenderer
{
public:
    BeagleRenderer(Scene* pScene)
    : BaseRenderer(pScene)
    {
        LOG_DEBUG("BeagleRenderer", "Creating Renderer");
        ITextureBinder* binder = new BeagleEglImage();
        BaseGraphicSystem* graphicSystem = new GLESGraphicsystem( ShaderProgramBeagle::createProgram, binder);
        m_windowSystem = new NullWindowSystem(pScene, graphicSystem);
    }
};

extern "C" BaseRenderer* createBeagleRenderer(Scene* pScene) {
    return new BeagleRenderer(pScene);
}

extern "C" void destroyBeagleRenderer(BeagleRenderer* p) {
    delete p;
}

#endif /* BEAGLERENDERER_H_ */
