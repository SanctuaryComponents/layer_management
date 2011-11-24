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
#include "SurfaceCreateCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

SurfaceCreateCommand::SurfaceCreateCommand(unsigned int handle, PixelFormat pixelformat, uint OriginalWidth, uint OriginalHeight, uint* idReturn)
: m_nativeHandle(handle)
, m_pixelformat(pixelformat)
, m_originalWidth(OriginalWidth)
, m_originalHeight(OriginalHeight)
, m_idReturn(idReturn)
{
}

ExecutionResult SurfaceCreateCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());
    ExecutionResult result = ExecutionFailed;

    Surface* surface = scene.createSurface(*m_idReturn);

    if (surface)
    {
        *m_idReturn = surface->getID();

        surface->nativeHandle = m_nativeHandle;
        surface->setPixelFormat(m_pixelformat);
        surface->OriginalSourceWidth = m_originalWidth;
        surface->OriginalSourceHeight = m_originalHeight;
        surface->setDestinationRegion(Rectangle(0, 0, m_originalWidth, m_originalHeight));
        surface->setSourceRegion(Rectangle(0, 0, m_originalWidth, m_originalHeight));

        LOG_DEBUG("CreateCommand", "Created surface with new id: " << surface->getID() << " handle " << m_nativeHandle << " pixelformat:" << m_pixelformat << " originalwidth:" << m_originalWidth<< " originalheighth:" << m_originalHeight);

        result = ExecutionSuccess;
    }

    return result;
}

const std::string SurfaceCreateCommand::getString()
{
    std::stringstream description;
    description << "SurfaceCreateCommand("
                << "nativeHandle=" << m_nativeHandle
                << ", pixelformat=" << m_pixelformat
                << ", OriginalWidth=" << m_originalWidth
                << ", OriginalHeight=" << m_originalHeight
                << ", idReturn=" << m_idReturn << "=" << *m_idReturn
                << ")";
    return description.str();
}
