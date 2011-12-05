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

#ifndef _SURFACESETNATIVECONTENTCOMMAND_H_
#define _SURFACESETNATIVECONTENTCOMMAND_H_

#include "BaseCommandSynchronous.h"
#include "PixelFormat.h"
#include "IScene.h"

class SurfaceSetNativeContentCommand : public BaseCommandSynchronous
{
public:
       SurfaceSetNativeContentCommand(unsigned int surfaceId, unsigned int handle, PixelFormat pixelformat, uint OriginalWidth, uint OriginalHeight);
       virtual ~SurfaceSetNativeContentCommand() {}
    virtual ExecutionResult execute(ICommandExecutor* executor);
    virtual const std::string getString();

private:
    uint m_surfaceId;
    const unsigned int m_nativeHandle;
    const PixelFormat m_pixelformat;
    uint m_originalWidth;
    uint m_originalHeight;

    // for unit testing
    //template <typename nativeHandle_type, typename pixelformat_type, typename OriginalWidth_type, typename OriginalHeight_type> friend class SurfaceSetRenderBufferCommandEqMatcherP4;
};


#endif /* _SURFACESETNATIVECONTENTCOMMAND_H_ */
