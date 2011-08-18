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
#include "CreateCommand.h"
#include "ICommandExecutor.h"
#include "Scene.h"
#include "Log.h"

CreateCommand::CreateCommand(unsigned int handle, ObjectType createType, PixelFormat pixelformat, uint OriginalWidth, uint OriginalHeight, uint* idReturn)
: m_createType(createType)
, m_nativeHandle(handle)
, m_pixelformat(pixelformat)
, m_originalWidth(OriginalWidth)
, m_originalHeight(OriginalHeight)
, m_idReturn(idReturn)
{
}

ExecutionResult CreateCommand::execute(ICommandExecutor* executor)
{
    Scene& scene = *(executor->getScene());
    bool result = false;

    switch(m_createType)
    {
        case TypeSurface:
            result = createSurface(scene);
            break;

        case TypeLayer:
            result = createLayer(scene);
            break;

        case TypeSurfaceGroup:
            result = createSurfaceGroup(scene);
            break;

        case TypeLayerGroup:
            result = createLayerGroup(scene);
            break;

        default:
            LOG_ERROR("CreateCommand", "could not create unknown type.");
            break;
    }
    return result ? ExecutionSuccessRedraw : ExecutionFailed;
}

bool CreateCommand::createSurface(IScene& scene)
{
    Surface *surface = scene.createSurface(*m_idReturn);
    if (!surface)
    {
        return false;
    }

    *m_idReturn = surface->getID();

    surface->nativeHandle = m_nativeHandle;
    surface->setPixelFormat(m_pixelformat);
    surface->OriginalSourceWidth = m_originalWidth;
    surface->OriginalSourceHeight = m_originalHeight;
    surface->setDestinationRegion(Rectangle(0, 0, m_originalWidth, m_originalHeight));
    surface->setSourceRegion(Rectangle(0, 0, m_originalWidth, m_originalHeight));

    LOG_DEBUG("CreateCommand", "Created surface with new id: " << surface->getID() << " handle " << m_nativeHandle << " pixelformat:" << m_pixelformat << " originalwidth:" << m_originalWidth<< " originalheighth:" << m_originalHeight);

    return true;
}

bool CreateCommand::createLayer(IScene& scene)
{
    Layer *l = scene.createLayer(*m_idReturn);
    if ( l == NULL )
    {
        return false;
    }

    *m_idReturn = l->getID();

    l->setDestinationRegion(Rectangle(0, 0, m_originalWidth, m_originalHeight));
    l->setSourceRegion(Rectangle(0, 0, m_originalWidth, m_originalHeight));
    l->OriginalSourceWidth = m_originalWidth;
    l->OriginalSourceHeight = m_originalHeight;

    LOG_DEBUG("CreateCommand", "created layer with id: " << l->getID() << "width: " << m_originalWidth << " height: " << m_originalHeight);
    return true;
}

bool CreateCommand::createSurfaceGroup(IScene& scene)
{
    SurfaceGroup *sg = scene.createSurfaceGroup(*m_idReturn);

    if ( sg == NULL )
    {
        return false;
    }

    *m_idReturn = sg->getID();

    LOG_DEBUG("CreateCommand", "created surfacegroup with id:" << sg->getID());
    return true;
}

bool CreateCommand::createLayerGroup(IScene& scene)
{
    LayerGroup *lg = scene.createLayerGroup(*m_idReturn);

    if ( lg == NULL )
    {
        return false;
    }

    *m_idReturn = lg->getID();

    LOG_DEBUG("CreateCommand", "created layergroup with id: " << lg->getID());
    return true;
}

const std::string CreateCommand::getString()
{
    std::stringstream description;
    description << "CreateCommand("
                << "createType=" << m_createType
                << ", nativeHandle=" << m_nativeHandle
                << ", pixelformat=" << m_pixelformat
                << ", OriginalWidth=" << m_originalWidth
                << ", OriginalHeight=" << m_originalHeight
                << ", idReturn=" << m_idReturn << "=" << *m_idReturn
                << ")";
    return description.str();
}
