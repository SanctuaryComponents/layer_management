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
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "WindowSystems/WaylandInputEvent.h"
#include "WindowSystems/WaylandBaseWindowSystem.h"

#define HAVE_MKOSTEMP 1

/////////////////////////////////////////////////////////////////////////////

static int
setCloexecOrClose(int fd)
{
    long flags;
    if (fd == -1)
        return -1;

    flags = fcntl(fd, F_GETFD);
    if (flags == -1)
        goto err;

    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)
        goto err;

    return fd;
err:
    close(fd);
    return -1;
}

static int
createTmpFileCloexec(char *tmpname)
{
    int fd;
#ifdef HAVE_MKOSTEMP
    fd = mkostemp(tmpname, O_CLOEXEC);
    if (fd >= 0){
        unlink(tmpname);
    }
#else
    fd = mkstemp(tmpname);
    if (fd >= 0){
        fd = setCloexecOrClose(fd);
        unlink(tmpname);
    }
#endif
    return fd;
}

/////////////////////////////////////////////////////////////////////////////

WaylandInputEvent::WaylandInputEvent(WaylandBaseWindowSystem *windowSystem)
: m_windowSystem(windowSystem)
, m_inputDevice(NULL)
, m_wlEventSource(NULL)
, m_fd(0)
{
    initInputEvent();
}

WaylandInputEvent::~WaylandInputEvent()
{
    if (!m_wlEventSource)
        wl_event_source_remove(m_wlEventSource);
}

void
WaylandInputEvent::initInputEvent()
{
    LOG_DEBUG("WaylandInputEvent", "initInputEvent IN");
    m_inputDevice = new WaylandInputDevice(m_windowSystem->getNativeDisplayHandle());
    if (!m_inputDevice){
        LOG_ERROR("WaylandInputEvent", "Failed to create WaylandInputDevice");
        return;
    }
    LOG_DEBUG("WaylandInputEvent", "initInputEvent OUT");
}

void
WaylandInputEvent::setupInputEvent()
{
    LOG_ERROR("WaylandInputEvent", "Called setupInputEvent()");
}

int
WaylandInputEvent::createAnonymousFile(off_t size)
{
    static const char temp[] = "/weston-shared-XXXXXX";
    const char *path;
    char *name;
    int fd;

    path = getenv("XDG_RUNTIME_DIR");
    if (!path){
        return -1;
    }

    name = (char*)malloc(strlen(path) + sizeof(temp));
    if (!name){
        return -1;
    }

    strcpy(name, path);
    strcat(name, temp);
    fd = createTmpFileCloexec(name);

    free(name);
    if (fd < 0){
        return -1;
    }

    if (ftruncate(fd, size) < 0){
        close(fd);
        return -1;
    }

    return fd;
}
