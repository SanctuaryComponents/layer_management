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
#include "ScreenShotCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

ScreenShotCommand::ScreenShotCommand(char* givenfilename, ScreenShotType type, unsigned int id)
: m_filename(givenfilename)
, m_id(id)
, m_type(type)
{
}

ExecutionResult ScreenShotCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());
    RendererList& m_rendererList = *(executor->getRendererList());

    bool status = false;

    LOG_INFO("LayerManager","making screenshot, output file: " << m_filename);

    // check for invalid screen, surface or layer ids
    switch (m_type)
    {
        case ScreenshotOfDisplay:
            status = (m_id == 0);
            break;

        case ScreenshotOfLayer:
            status = scene.getLayer(m_id);
            break;

        case ScreenshotOfSurface:
            status = scene.getSurface(m_id);
            break;

        case ScreenShotNone:
            status = true;
            break;
    }

    if (!status)
    {
        return ExecutionFailed;
    }

    // call screen shot on all renderers for now TODO
    RendererListConstIterator iter = m_rendererList.begin();
    RendererListConstIterator iterEnd = m_rendererList.end();

    for (; iter != iterEnd; ++iter)
    {
        IRenderer* renderer = *iter;

        if (renderer)
        {
            switch (m_type)
            {
            case ScreenshotOfDisplay:
            {
                renderer->doScreenShot(m_filename);
                break;
            }

            case ScreenshotOfLayer:
            {
                renderer->doScreenShotOfLayer(m_filename, m_id);
                break;
            }

            case ScreenshotOfSurface:
            {
                renderer->doScreenShotOfSurface(m_filename, m_id);
                break;
            }

            case ScreenShotNone:
                break;

            default:
                status = false;
                break;
            }
        }
    }
    return status ? ExecutionSuccessRedraw : ExecutionFailed;
}

const std::string ScreenShotCommand::getString()
{
    std::stringstream description;
    description << "ScreenShotCommand("
                << "filename=" << m_filename
                << ", id=" << m_id
                << ", type=" << m_type
                << ")";
    return description.str();
}
