/***************************************************************************
 *
 * Copyright 2010, 2011 BMW Car IT GmbH  
 * Copyright (C) 2011 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
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

#ifndef _WAYLANDCOPYGLES_H_
#define _WAYLANDCOPYGLES_H_

#include "TextureBinders/ITextureBinder.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

class WaylandCopyGLES: public ITextureBinder
{
public:
    virtual PlatformSurface* createPlatformSurface(Surface* surface);

    WaylandCopyGLES(EGLDisplay eglDisplay, struct wl_display* waylandDisplay);
    ~WaylandCopyGLES();
    bool bindSurfaceTexture(Surface* surface);
    bool unbindSurfaceTexture(Surface* surface);
    void createClientBuffer(Surface* surface);
    void destroyClientBuffer(Surface* surface);

private:
    EGLDisplay m_eglDisplay;
    struct wl_display* m_wlDisplay;
};

#endif /* _WAYLANDCOPYGLES_H_ */
