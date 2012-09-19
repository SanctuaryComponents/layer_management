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
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 ****************************************************************************/

#ifndef _WAYLANDINPUTEVENT_H_
#define _WAYLANDINPUTEVENT_H_
#include <wayland-server.h>
#include "Log.h"
#include "config.h"
#include "WindowSystems/WaylandInputDevice.h"

//////////////////////////////////////////////////////////////////////////////
extern "C"
{
};

class WaylandBaseWindowSystem;

//////////////////////////////////////////////////////////////////////////////
class WaylandInputEvent
{
// Properties
protected:
    WaylandBaseWindowSystem *m_windowSystem;
    WaylandInputDevice      *m_inputDevice;
    struct wl_event_source  *m_wlEventSource;
    int m_fd;

    struct xkbInfo {
        struct xkb_keymap *keymap;
        int                keymapFd;
        size_t             keymapSize;
        char              *keymapArea;
    } m_xkbInfo;

// Methods
public:
    WaylandInputEvent(WaylandBaseWindowSystem *windowSystem);
    virtual ~WaylandInputEvent();

    virtual void setupInputEvent();

    WaylandInputDevice& inputDevice() const;
    WaylandBaseWindowSystem& windowSystem() const;

protected:
    int createAnonymousFile(off_t size);

private:
    void initInputEvent();
};

inline WaylandInputDevice& WaylandInputEvent::inputDevice() const { return *m_inputDevice; }
inline WaylandBaseWindowSystem& WaylandInputEvent::windowSystem() const { return *m_windowSystem; }

#endif /* _WAYLANDINPUTEVENT_H_ */
