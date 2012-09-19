/**************************************************************************
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "WLContext.h"
#include "WaylandServerinfoClientProtocol.h"

#define WL_UNUSED(A) (A)=(A)

//////////////////////////////////////////////////////////////////////////////

static struct serverinfo_listener serverInfoListenerList = {
    WLContext::ServerInfoListener,
};

static struct wl_seat_listener seatListener = {
    WLContext::SeatHandleCapabilities,
};

//////////////////////////////////////////////////////////////////////////////

WLContext::WLContext()
: m_wlDisplay(NULL)
, m_wlCompositor(NULL)
, m_wlSeat(NULL)
, m_wlPointer(NULL)
, m_wlServerInfo(NULL)
, m_mask(0)
, m_connectionId(0)
, m_wlPointerListener(NULL)
{
}

WLContext::~WLContext()
{
    DestroyWLContext();
}

//////////////////////////////////////////////////////////////////////////////

void
WLContext::DisplayHandleGlobal(struct wl_display* display,
                               uint32_t id,
                               const char* interface,
                               uint32_t version,
                               void* data)
{
    WL_UNUSED(display);
    WL_UNUSED(version);

    WLContext* surface = static_cast<WLContext*>(data);
    assert(surface);

    do {
        if (!strcmp(interface, "wl_compositor")){
            surface->SetWLCompositor(
                (wl_compositor*)wl_display_bind(surface->GetWLDisplay(),
                                                id,
                                                &wl_compositor_interface));
            break;
        }

        if (!strcmp(interface, "serverinfo")){
            struct serverinfo* wlServerInfo = (struct serverinfo*)wl_display_bind(
                surface->GetWLDisplay(), id, &serverinfo_interface);
            serverinfo_add_listener(wlServerInfo, &serverInfoListenerList, data);
            serverinfo_get_connection_id(wlServerInfo);
            surface->SetWLServerInfo(wlServerInfo);
            break;
        }

        if (!strcmp(interface, "wl_seat")){
            struct wl_seat* wlSeat = (wl_seat*)wl_display_bind(
                surface->GetWLDisplay(), id, &wl_seat_interface);
            wl_seat_add_listener(wlSeat, &seatListener, data);
            surface->SetWLSeat(wlSeat);
        }
    } while (0);
}

int
WLContext::EventMaskUpdate(uint32_t mask, void* data)
{
    WLContext* surface = static_cast<WLContext*>(data);
    assert(surface);

    surface->SetEventMask(mask);
    return 0;
}

void
WLContext::ServerInfoListener(void* data,
                              struct serverinfo* serverInfo,
                              uint32_t clientHandle)
{
    WL_UNUSED(serverInfo);

    WLContext* surface = static_cast<WLContext*>(data);
    assert(surface);

    surface->SetConnectionId(clientHandle);
}

void
WLContext::SeatHandleCapabilities(void* data, struct wl_seat* seat, uint32_t caps)
{
    WL_UNUSED(seat);

    WLContext* context = static_cast<WLContext*>(data);
    assert(context);

    struct wl_seat* wlSeat = context->GetWLSeat();
    if (!wlSeat)
        return;

    struct wl_pointer* wlPointer = context->GetWLPointer();
    if ((caps & WL_SEAT_CAPABILITY_POINTER) && !wlPointer){
        wlPointer = wl_seat_get_pointer(wlSeat);
        wl_pointer_set_user_data(wlPointer, data);
        wl_pointer_add_listener(wlPointer, context->GetWLPointerListener(), data);
    } else
    if (!(caps & WL_SEAT_CAPABILITY_POINTER) && wlPointer){
        wl_pointer_destroy(wlPointer);
        wlPointer = NULL;
    }
    context->SetWLPointer(wlPointer);

    struct wl_keyboard* wlKeyboard = context->GetWLKeyboard();
    if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !wlKeyboard){
        wlKeyboard = wl_seat_get_keyboard(wlSeat);
        wl_keyboard_set_user_data(wlKeyboard, data);
        wl_keyboard_add_listener(wlKeyboard, context->GetWLKeyboardListener(), data);
    } else
    if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && wlKeyboard){
        wl_keyboard_destroy(wlKeyboard);
        wlKeyboard = NULL;
    }
    context->SetWLKeyboard(wlKeyboard);
}

//////////////////////////////////////////////////////////////////////////////

bool
WLContext::InitWLContext(const struct wl_pointer_listener* wlPointerListener,
                         const struct wl_keyboard_listener* wlKeyboardListener)
{
    m_wlPointerListener = const_cast<wl_pointer_listener*>(wlPointerListener);
    m_wlKeyboardListener = const_cast<wl_keyboard_listener*>(wlKeyboardListener);

    m_wlDisplay = wl_display_connect(NULL);

    wl_display_add_global_listener(m_wlDisplay, DisplayHandleGlobal, this);
    wl_display_get_fd(m_wlDisplay, EventMaskUpdate, this);
    wl_display_iterate(m_wlDisplay, WL_DISPLAY_READABLE);
    wl_display_roundtrip(m_wlDisplay);

    return true;
}

void
WLContext::DestroyWLContext()
{
    if (m_wlCompositor)
        wl_compositor_destroy(m_wlCompositor);
}
