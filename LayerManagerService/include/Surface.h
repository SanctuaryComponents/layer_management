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

#ifndef _SURFACE_H_
#define _SURFACE_H_

#include <stdlib.h>
#include <pthread.h>

#include "GraphicalSurface.h"
#include "PlatformSurface.h"
#include "PixelFormat.h"
#include "InputManager.h"

/**
 * Represents a graphical surface generated by an application. Always contained in layers.
 */
class Surface: public GraphicalSurface
{

public:
    PixelFormat getPixelFormat() const
    {
        return m_pixformat;
    }

    void setPixelFormat(PixelFormat pf)
    {
        m_pixformat = pf;
    }

    unsigned int getContainingLayerId() const
    {
        return m_layerId;
    }

    void setContainingLayerId(unsigned int id)
    {
        m_layerId = id;
    }

    bool hasNativeContent() const
    {
        return m_hasNativeContent;
    }
    
    void removeNativeContent() 
    {
        m_hasNativeContent = false;
    }

    void setNativeContent(long nativeHandle)
    {
        if (!hasNativeContent())
        {
            m_nativeHandle = nativeHandle;
            m_hasNativeContent = true;
        }
    }

    long getNativeContent() const
    {
        return hasNativeContent() ? m_nativeHandle : -1;
    }

    /**
     * Indicate from which input devices the Surface can accept events.
     *
     * \param[in] devices Bit masks of supported devices
     * \param[in] accept Indicate if the surface should accept or reject input events from the specified list of devices
     */
    void updateInputEventAcceptanceFrom(InputDevice devices, bool accept)
    {
        if (accept)
        {
            pthread_mutex_lock(&m_inputAcceptMutex);
            m_acceptInputFrom = (InputDevice) (m_acceptInputFrom | devices);
            pthread_mutex_unlock(&m_inputAcceptMutex);
        }
        else
        {
            pthread_mutex_lock(&m_inputAcceptMutex);
            m_acceptInputFrom = (InputDevice) (m_acceptInputFrom  & ~devices);
            pthread_mutex_unlock(&m_inputAcceptMutex);
        }
    }


    /**
     * Indicate if the surface accepts input events from the specified list of devices.
     *
     * \param[in] devices Bit masks of devices
     * \return true if input events are accepted for ALL of the specified devices
     * \return false if input events are not accepted for at least ONE of the specified devices
     */
    bool isInputEventAcceptedFrom(InputDevice devices)
    {
        bool ret;

        pthread_mutex_lock(&m_inputAcceptMutex);
        ret = ((m_acceptInputFrom & devices) == devices);
        pthread_mutex_unlock(&m_inputAcceptMutex);

        return ret;
    }
    
    /**
     * Get the set of devices from which the surface can accept input events
     *
     * \return Bitmask of InputDevice
     */
    InputDevice getInputEventAcceptanceOnDevices() const
    {
        return m_acceptInputFrom;
    }
    
    /**
     * Platform specific Object containing surface information specific to a used platform.
     * This typically contains a native window handle/surface handle or information only used
     * by a specific renderer.unsigned
     */
    PlatformSurface *platform; // platform/rendering specific window attributes

    int frameCounter;
    int updateCounter;
    int drawCounter;

    Surface()
    : GraphicalSurface(TypeSurface)
    , platform(NULL)
    , frameCounter(0)
    , updateCounter(0)
    , drawCounter(0)
    , m_nativeHandle(0)
    , m_pixformat(PIXELFORMAT_UNKNOWN)
    , m_layerId(INVALID_ID)
    , m_hasNativeContent(false)
    , m_acceptInputFrom(INPUT_DEVICE_ALL)
    {
        pthread_mutex_init(&m_inputAcceptMutex, NULL);
    }

    Surface(int id)
    : GraphicalSurface(id, TypeSurface)
    , platform(NULL)
    , frameCounter(0)
    , updateCounter(0)
    , drawCounter(0)
    , m_nativeHandle(0)
    , m_pixformat(PIXELFORMAT_UNKNOWN)
    , m_layerId(INVALID_ID)
    , m_hasNativeContent(false)
    , m_acceptInputFrom(INPUT_DEVICE_ALL)
    {
        pthread_mutex_init(&m_inputAcceptMutex, NULL);
    }

    ~Surface()
    {
        pthread_mutex_destroy(&m_inputAcceptMutex);
    }
    

private:
    long m_nativeHandle;
    PixelFormat m_pixformat;
    unsigned int m_layerId;
    bool m_hasNativeContent;
    /** \brief Bitmask to represent from which kind of devices the surface can accept
      * input events. By default, all devices are accepted. */
    InputDevice m_acceptInputFrom;
    /** \brief m_acceptInputFrom can be accessed concurently. To make it thread safe, we need a mutex */
    pthread_mutex_t m_inputAcceptMutex;
};

#endif /* _SURFACE_H_ */

