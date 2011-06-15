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

#include "LayerCapabilities.h"
#include "BaseRenderer.h"
#include "Log.h"

bool BaseRenderer::debugMode = true;

BaseRenderer::BaseRenderer(Scene* pScene) : m_pScene(pScene)
{
    LOG_DEBUG("BaseRenderer", "Creating Renderer");
}

BaseRenderer::~BaseRenderer()
{
}

uint BaseRenderer::getLayerTypeCapabilities(LayerType layerType)
{
    uint result = 0;
    switch(layerType)
    {
        case Software_2D:
            result = LayerPosition | LayerScalable | LayerOrientable | LayerComposedByAlpha;
            break;

        case Software_2_5D:
            result = LayerPosition | LayerScalable | LayerOrientable | LayerComposedByAlpha;
            break;

        default :
            break;
    }
    return result;
}

