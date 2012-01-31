/***************************************************************************
 *
 * Copyright 2012 BMW Car IT GmbH
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

#ifndef _ICOMMUNICATORCLIENT_H_
#define _ICOMMUNICATORCLIENT_H_

enum CommunicatorClientResult
{
    COMMUNICATOR_CLIENT_SUCCESS,
    COMMUNICATOR_CLIENT_FAIL
};

class ICommunicatorClient
{
public:
    CommunicatorClientResult transmitCommand(ICommand* pCommand) = 0;
    /*
    CommunicatorClientResult Commit
    CommunicatorClientResult Debug
    CommunicatorClientResult Exit

    CommunicatorClientResult ScreenDump
    CommunicatorClientResult ScreenSetRenderOrder

    CommunicatorClientResult LayerAddSurface
    CommunicatorClientResult LayerCreate(int* layerId);
    CommunicatorClientResult LayerDump
    CommunicatorClientResult LayerGetDimension
    CommunicatorClientResult LayerGetOpacity
    CommunicatorClientResult LayerGetOrientation
    CommunicatorClientResult LayerGetPosition
    CommunicatorClientResult LayerGetVisibility
    CommunicatorClientResult LayerRemove
    CommunicatorClientResult LayerRemoveSurface
    CommunicatorClientResult LayerSetDestinationRectangle
    CommunicatorClientResult LayerSetDimension
    CommunicatorClientResult LayerSetOpacity
    CommunicatorClientResult LayerSetOrientation
    CommunicatorClientResult LayerSetPosition
    CommunicatorClientResult LayerSetRenderOrder
    CommunicatorClientResult LayerSetSourceRectangle
    CommunicatorClientResult LayerSetType
    CommunicatorClientResult LayerSetVisibility

    CommunicatorClientResult LayergroupAddLayer
    CommunicatorClientResult LayergroupCreate
    CommunicatorClientResult LayergroupGetOpacity
    CommunicatorClientResult LayergroupGetVisibility
    CommunicatorClientResult LayergroupRemove
    CommunicatorClientResult LayergroupRemoveLayer
    CommunicatorClientResult LayergroupSetOpacity
    CommunicatorClientResult LayergroupSetVisibility

    CommunicatorClientResult SurfaceCreate
    CommunicatorClientResult SurfaceDump
    CommunicatorClientResult SurfaceGetDimension
    CommunicatorClientResult SurfaceGetOpacity
    CommunicatorClientResult SurfaceGetOrientation
    CommunicatorClientResult SurfaceGetPixelformat
    CommunicatorClientResult SurfaceGetPosition
    CommunicatorClientResult SurfaceGetVisibility
    CommunicatorClientResult SurfaceRemove
    CommunicatorClientResult SurfaceSetDestinationRectangle
    CommunicatorClientResult SurfaceSetDimension
    CommunicatorClientResult SurfaceSetNativeContent
    CommunicatorClientResult SurfaceSetOpacity
    CommunicatorClientResult SurfaceSetOrientation
    CommunicatorClientResult SurfaceSetPosition
    CommunicatorClientResult SurfaceSetShader
    CommunicatorClientResult SurfaceSetSourceRectangle
    CommunicatorClientResult SurfaceSetVisibility

    CommunicatorClientResult SurfacegroupAddSurface
    CommunicatorClientResult SurfacegroupCreate
    CommunicatorClientResult SurfacegroupGetOpacity
    CommunicatorClientResult SurfacegroupGetVisibility
    CommunicatorClientResult SurfacegroupRemove
    CommunicatorClientResult SurfacegroupRemoveSurface
    CommunicatorClientResult SurfacegroupSetOpacity
    CommunicatorClientResult SurfacegroupSetVisibility

    CommunicatorClientResult ShaderCreate
    CommunicatorClientResult ShaderDestroy
    CommunicatorClientResult ShaderSetUniforms
*/
};



#endif // _ICOMMUNICATORCLIENT_H_
