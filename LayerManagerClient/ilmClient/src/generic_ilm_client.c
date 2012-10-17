/**************************************************************************
 *
 * Copyright 2012 BMW Car IT GmbH
 * Copyright (C) 2012 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
 * Copyright (C) 2012 Bayerische Motorenwerke Aktiengesellschaft
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
#include "ilm_client.h"
#include "ilm_types.h"
#include "IpcModuleLoader.h"
#include "ObjectType.h"
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <mqueue.h>
#include <fcntl.h>


//=============================================================================
// global settings
//=============================================================================
const int gReceiveTimeout  = -1;  // in ms, negative value for infinite
const int gResponseTimeout = 500; // in ms

// must be same as GraphicalObject::INVALID_ID, but this is defined in C++
// and can not be used here
#define INVALID_ID 0xFFFFFFFF

// queue names for incoming notifications and messages
#define NOTIFICATION_QUEUE_NAME "/ilmClient%dNotification"
#define INCOMING_QUEUE_NAME     "/ilmClient%dIncoming"

//=============================================================================
// global vars
//=============================================================================
extern char *__progname;

struct IpcModule gIpcModule;

pthread_t gReceiveThread;
pthread_mutex_t gNewMessageAvailable;
pthread_mutex_t gCurrentMessageReleased;
pthread_t gNotificationThread;

mqd_t incomingMqRead;
mqd_t incomingMqWrite;

mqd_t notificationMqRead;
mqd_t notificationMqWrite;

//=============================================================================
// notification management
//=============================================================================
#define MAX_CALLBACK_COUNT 64
struct
{
    t_ilm_uint id;
    layerNotificationFunc callback;
} static gLayerNotificationCallbacks[MAX_CALLBACK_COUNT];

struct
{
    t_ilm_uint id;
    surfaceNotificationFunc callback;
} static gSurfaceNotificationCallbacks[MAX_CALLBACK_COUNT];

void initNotificationCallbacks()
{
    int i = 0;
    for (i = 0; i < MAX_CALLBACK_COUNT; ++i)
    {
        gLayerNotificationCallbacks[i].id = INVALID_ID;
        gLayerNotificationCallbacks[i].callback = NULL;
        gSurfaceNotificationCallbacks[i].id = INVALID_ID;
        gSurfaceNotificationCallbacks[i].callback = NULL;
    }
}

layerNotificationFunc getLayerNotificationCallback(t_ilm_layer layer)
{
    int i = 0;
    for (i = 0; i < MAX_CALLBACK_COUNT; ++i)
    {
        if (gLayerNotificationCallbacks[i].id == layer)
        {
            return gLayerNotificationCallbacks[i].callback;
        }
    }
    return NULL;
}

surfaceNotificationFunc getSurfaceNotificationCallback(t_ilm_surface surface)
{
    int i = 0;
    for (i = 0; i < MAX_CALLBACK_COUNT; ++i)
    {
        if (gSurfaceNotificationCallbacks[i].id == surface)
        {
            return gSurfaceNotificationCallbacks[i].callback;
        }
    }
    return NULL;
}

t_ilm_bool findLayerCallback(t_ilm_layer layer)
{
    int i = 0;

    // try to overwrite existing entry for layer id
    for (i = 0; i < MAX_CALLBACK_COUNT; ++i)
    {
        if (gLayerNotificationCallbacks[i].id == layer)
        {
            return ILM_TRUE;
        }
    }
    return ILM_FALSE;
}

t_ilm_bool addLayerCallback(t_ilm_layer layer, layerNotificationFunc func)
{
    int i = 0;

    if (findLayerCallback(layer))
    {
        return ILM_FALSE;
    }

    // find free slot and store callback
    for (i = 0; i < MAX_CALLBACK_COUNT; ++i)
    {
        if (gLayerNotificationCallbacks[i].id == INVALID_ID)
        {
            gLayerNotificationCallbacks[i].id = layer;
            gLayerNotificationCallbacks[i].callback = func;
            return ILM_TRUE;
        }
    }
    printf("DbusIpcModule: addLayerCallback() failed. no free slots.");
    return ILM_FALSE;
}

t_ilm_bool findSurfaceCallback(t_ilm_surface surface)
{
    int i = 0;

    // try to overwrite existing entry for layer id
    for (i = 0; i < MAX_CALLBACK_COUNT; ++i)
    {
        if (gSurfaceNotificationCallbacks[i].id == surface)
        {
            return ILM_TRUE;
        }
    }
    return ILM_FALSE;
}

t_ilm_bool addSurfaceCallback(t_ilm_surface surface, surfaceNotificationFunc func)
{
    int i = 0;

    if (findSurfaceCallback(surface))
    {
        return ILM_FALSE;
    }

    // find free slot and store callback
    for (i = 0; i < MAX_CALLBACK_COUNT; ++i)
    {
        if (gSurfaceNotificationCallbacks[i].id == INVALID_ID)
        {
            gSurfaceNotificationCallbacks[i].id = surface;
            gSurfaceNotificationCallbacks[i].callback = func;
            return ILM_TRUE;
        }
    }
    printf("DbusIpcModule: addSurfaceCallback() failed. no free slots.");
    return ILM_FALSE;
}

void removeLayerCallback(t_ilm_layer layer)
{
    int i = 0;
    for (i = 0; i < MAX_CALLBACK_COUNT; ++i)
    {
        if (gLayerNotificationCallbacks[i].id == layer)
        {
            gLayerNotificationCallbacks[i].id = INVALID_ID;
            gLayerNotificationCallbacks[i].callback = NULL;
            return;
        }
    }
}

void removeSurfaceCallback(t_ilm_surface layer)
{
    int i = 0;
    for (i = 0; i < MAX_CALLBACK_COUNT; ++i)
    {
        if (gSurfaceNotificationCallbacks[i].id == layer)
        {
            gSurfaceNotificationCallbacks[i].id = INVALID_ID;
            gSurfaceNotificationCallbacks[i].callback = NULL;
            return;
        }
    }
}

//=============================================================================
// handling of internal notification thread for dispatching notifications
// Note: notification callbacks may be blocked by client, but receive thread
// must not be blocked
//=============================================================================
void* notificationThreadLoop(void* param)
{
    t_ilm_message notification;

    while (-1 != mq_receive(notificationMqRead, (char*)&notification, sizeof(notification), NULL))
    {
        t_ilm_const_string name = gIpcModule.getMessageName(notification);

        // this depends on message name, but it is fast
        if ('L' == name[15])
        {
            t_ilm_uint id;
            t_ilm_uint mask;
            struct ilmLayerProperties properties;

            gIpcModule.getUint(notification, &id);
            gIpcModule.getUint(notification, &mask);
            gIpcModule.getDouble(notification, &properties.opacity);
            gIpcModule.getUint(notification, &properties.sourceX);
            gIpcModule.getUint(notification, &properties.sourceY);
            gIpcModule.getUint(notification, &properties.sourceWidth);
            gIpcModule.getUint(notification, &properties.sourceHeight);
            gIpcModule.getUint(notification, &properties.origSourceWidth);
            gIpcModule.getUint(notification, &properties.origSourceHeight);
            gIpcModule.getUint(notification, &properties.destX);
            gIpcModule.getUint(notification, &properties.destY);
            gIpcModule.getUint(notification, &properties.destWidth);
            gIpcModule.getUint(notification, &properties.destHeight);
            gIpcModule.getUint(notification, &properties.orientation);
            gIpcModule.getBool(notification, &properties.visibility);
            gIpcModule.getUint(notification, &properties.type);
            gIpcModule.getBool(notification, &properties.chromaKeyEnabled);
            gIpcModule.getUint(notification, &properties.chromaKeyRed);
            gIpcModule.getUint(notification, &properties.chromaKeyGreen);
            gIpcModule.getUint(notification, &properties.chromaKeyBlue);
            gIpcModule.getInt(notification, &properties.creatorPid);

            layerNotificationFunc func = getLayerNotificationCallback(id);
            if (func)
            {
                (*func)(id, &properties, mask);
            }
            else
            {
                fprintf(stderr, "notification for layer %d received, but no callback set\n", id);
            }
        }

        if ('S' == name[15])
        {
            t_ilm_uint id;
            t_ilm_uint mask;
            struct ilmSurfaceProperties properties;

            gIpcModule.getUint(notification, &id);
            gIpcModule.getUint(notification, &mask);
            gIpcModule.getDouble(notification, &properties.opacity);
            gIpcModule.getUint(notification, &properties.sourceX);
            gIpcModule.getUint(notification, &properties.sourceY);
            gIpcModule.getUint(notification, &properties.sourceWidth);
            gIpcModule.getUint(notification, &properties.sourceHeight);
            gIpcModule.getUint(notification, &properties.origSourceWidth);
            gIpcModule.getUint(notification, &properties.origSourceHeight);
            gIpcModule.getUint(notification, &properties.destX);
            gIpcModule.getUint(notification, &properties.destY);
            gIpcModule.getUint(notification, &properties.destWidth);
            gIpcModule.getUint(notification, &properties.destHeight);
            gIpcModule.getUint(notification, &properties.orientation);
            gIpcModule.getBool(notification, &properties.visibility);
            gIpcModule.getUint(notification, &properties.frameCounter);
            gIpcModule.getUint(notification, &properties.drawCounter);
            gIpcModule.getUint(notification, &properties.updateCounter);
            gIpcModule.getUint(notification, &properties.pixelformat);
            gIpcModule.getUint(notification, &properties.nativeSurface);
            gIpcModule.getUint(notification, &properties.inputDevicesAcceptance);
            gIpcModule.getBool(notification, &properties.chromaKeyEnabled);
            gIpcModule.getUint(notification, &properties.chromaKeyRed);
            gIpcModule.getUint(notification, &properties.chromaKeyGreen);
            gIpcModule.getUint(notification, &properties.chromaKeyBlue);
            gIpcModule.getInt(notification, &properties.creatorPid);

            surfaceNotificationFunc func = getSurfaceNotificationCallback(id);
            if (func)
            {
                (*func)(id, &properties, mask);
            }
            else
            {
                fprintf(stderr, "notification for surface %d received, but no callback set\n", id);
            }
        }
    }
    gIpcModule.destroyMessage(notification);
    return NULL;
}

//=============================================================================
// handling of internal receive thread for event handling
//=============================================================================
void* receiveThreadLoop(void* param)
{
    t_ilm_bool running = ILM_TRUE;
    t_ilm_bool waitForMessageRelease = ILM_FALSE;

    while (running)
    {
        t_ilm_message message = gIpcModule.receive(gReceiveTimeout);
        t_ilm_message_type messageType = gIpcModule.getMessageType(message);
        switch (messageType)
        {
            case IpcMessageTypeNotification:
                if (-1 == mq_send(notificationMqWrite, (char*)&message, sizeof(message), 0))
                {
                    if (EAGAIN == errno)
                    {
                        printf("Notification queue full, dropped notification %s\n", gIpcModule.getMessageName(message));
                    }
                }
                break;

            case IpcMessageTypeCommand:
            case IpcMessageTypeError:
                if (-1 == mq_send(incomingMqWrite, (char*)&message, sizeof(message), 0))
                {
                    if (EAGAIN == errno)
                    {
                        printf("Incoming queue full, dropped message %s\n", gIpcModule.getMessageName(message));
                    }
                }
                break;

            case IpcMessageTypeNone:
                break;

            default:
                printf("ilmClient: discarded unexpected message (type: %d)\n", (int)messageType);
                gIpcModule.destroyMessage(message);
                break;
        }
    }

    return NULL;
}

void calculateTimeout(struct timeval* currentTime, int giventimeout, struct timespec* timeout)
{
    // nanoseconds is old value in nanoseconds + the given milliseconds as nanoseconds
    t_ilm_ulong newNanoSeconds = currentTime->tv_usec * 1000 + giventimeout * (1000 * 1000);

    // only use non full seconds, otherwise overflow!
    timeout->tv_nsec = newNanoSeconds % (1000000000);

    // new seconds are old seconds + full seconds from new nanoseconds part
    timeout->tv_sec  = currentTime->tv_sec + (newNanoSeconds / 1000000000 );
}

t_ilm_message waitForResponse(int timeoutInMs)
{
    t_ilm_message message = 0;

    if (-1 == mq_receive(incomingMqRead, (char*)&message, sizeof(t_ilm_message), NULL))
    {
       fprintf(stderr,"waitForResponse: mq_receive failed, errno = %d\n", errno);
    }
    return message;
}

//=============================================================================
// implementation
//=============================================================================
ilmErrorTypes ilm_init()
{
    ilmErrorTypes result = ILM_FAILED;

    initNotificationCallbacks();

    if (loadIpcModule(&gIpcModule))
    {
        int pid = getpid();

        if (gIpcModule.initClientMode())
        {
            result = ILM_SUCCESS;

            struct mq_attr mqAttr =
            {
                .mq_maxmsg = 4,
                .mq_msgsize = sizeof(t_ilm_message),
                .mq_flags = 0, //O_NONBLOCK,
                .mq_curmsgs = 0
            };

            char mqName[30];
            snprintf(mqName, sizeof(mqName), NOTIFICATION_QUEUE_NAME, getpid());

            notificationMqWrite = mq_open(mqName, O_WRONLY | O_CREAT, 0600, &mqAttr);
            notificationMqRead = mq_open(mqName, O_RDONLY);
            mq_unlink(mqName); // is destroyed on closed filedescriptor

            snprintf(mqName, sizeof(mqName), INCOMING_QUEUE_NAME, getpid());
            incomingMqWrite = mq_open(mqName, O_WRONLY | O_CREAT, 0600, &mqAttr);
            incomingMqRead = mq_open(mqName, O_RDONLY);
            mq_unlink(mqName); // is destroyed on closed filedescriptor

            if ((mqd_t)-1 == notificationMqRead || (mqd_t)-1 == notificationMqWrite)
            {
                printf("mq_open failed, errno = %d\n", errno);
                return result;
            }

            pthread_mutex_init(&gNewMessageAvailable, NULL);
            pthread_mutex_init(&gCurrentMessageReleased, NULL);

            pthread_mutex_lock(&gNewMessageAvailable);
            pthread_mutex_lock(&gCurrentMessageReleased);

            pthread_attr_t notificationThreadAttributes;
            pthread_attr_init(&notificationThreadAttributes);
            pthread_attr_setdetachstate(&notificationThreadAttributes,
                                        PTHREAD_CREATE_JOINABLE);

            if (0 != pthread_create(&gReceiveThread,
                                    &notificationThreadAttributes,
                                    receiveThreadLoop,
                                    NULL))
            {
                result = ILM_FAILED;
                printf("Failed to start internal receive thread.");
            }

            if (0 != pthread_create(&gNotificationThread,
                                    &notificationThreadAttributes,
                                    notificationThreadLoop,
                                    NULL))
            {
                result = ILM_FAILED;
                printf("Failed to start internal notification thread.");
            }
        }
        else
        {
            printf("Failed to initialize Client Ipc Module");
        }

        t_ilm_message command = gIpcModule.createMessage("ServiceConnect");
        if (command
                && gIpcModule.appendUint(command, pid)
                && gIpcModule.appendString(command, __progname)
                && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response)
            {
                result = ILM_SUCCESS;
            }
            else
            {
                result = ILM_FAILED;
                printf("Failed to connect to LayerManagerService.");
            }
            gIpcModule.destroyMessage(response);
        }
        gIpcModule.destroyMessage(command);
    }

    return result;
}

ilmErrorTypes ilm_destroy()
{
    ilmErrorTypes result = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("ServiceDisconnect");
    if (command
        && gIpcModule.appendUint(command, getpid())
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response)
        {
            result = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);

    // cancel worker threads
    void* threadReturnValue = NULL;

    pthread_cancel(gReceiveThread);
    pthread_cancel(gNotificationThread);

    pthread_join(gReceiveThread, &threadReturnValue);
    pthread_join(gNotificationThread, &threadReturnValue);

    pthread_mutex_unlock(&gNewMessageAvailable);
    pthread_mutex_unlock(&gCurrentMessageReleased);

    pthread_mutex_destroy(&gNewMessageAvailable);
    pthread_mutex_destroy(&gCurrentMessageReleased);

    gIpcModule.destroy();

    mq_close(notificationMqRead);
    mq_close(notificationMqWrite);

    mq_close(incomingMqRead);
    mq_close(incomingMqWrite);

    return ILM_SUCCESS;
}

ilmErrorTypes ilm_getPropertiesOfSurface(t_ilm_uint surfaceID, struct ilmSurfaceProperties* pSurfaceProperties)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetPropertiesOfSurface");
    if (pSurfaceProperties
        && command
        && gIpcModule.appendUint(command, surfaceID)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getDouble(response, &pSurfaceProperties->opacity)
            && gIpcModule.getUint(response, &pSurfaceProperties->sourceX)
            && gIpcModule.getUint(response, &pSurfaceProperties->sourceY)
            && gIpcModule.getUint(response, &pSurfaceProperties->sourceWidth)
            && gIpcModule.getUint(response, &pSurfaceProperties->sourceHeight)
            && gIpcModule.getUint(response, &pSurfaceProperties->origSourceWidth)
            && gIpcModule.getUint(response, &pSurfaceProperties->origSourceHeight)
            && gIpcModule.getUint(response, &pSurfaceProperties->destX)
            && gIpcModule.getUint(response, &pSurfaceProperties->destY)
            && gIpcModule.getUint(response, &pSurfaceProperties->destWidth)
            && gIpcModule.getUint(response, &pSurfaceProperties->destHeight)
            && gIpcModule.getUint(response, &pSurfaceProperties->orientation)
            && gIpcModule.getBool(response, &pSurfaceProperties->visibility)
            && gIpcModule.getUint(response, &pSurfaceProperties->frameCounter)
            && gIpcModule.getUint(response, &pSurfaceProperties->drawCounter)
            && gIpcModule.getUint(response, &pSurfaceProperties->updateCounter)
            && gIpcModule.getUint(response, &pSurfaceProperties->pixelformat)
            && gIpcModule.getUint(response, &pSurfaceProperties->nativeSurface)
            && gIpcModule.getUint(response, &pSurfaceProperties->inputDevicesAcceptance)
            && gIpcModule.getBool(response, &pSurfaceProperties->chromaKeyEnabled)
            && gIpcModule.getUint(response, &pSurfaceProperties->chromaKeyRed)
            && gIpcModule.getUint(response, &pSurfaceProperties->chromaKeyGreen)
            && gIpcModule.getUint(response, &pSurfaceProperties->chromaKeyBlue)
            && gIpcModule.getInt(response, &pSurfaceProperties->creatorPid))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_getPropertiesOfLayer(t_ilm_uint layerID, struct ilmLayerProperties* pLayerProperties)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetPropertiesOfLayer");
    if (pLayerProperties
        && command
        && gIpcModule.appendUint(command, layerID)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getDouble(response, &pLayerProperties->opacity)
            && gIpcModule.getUint(response, &pLayerProperties->sourceX)
            && gIpcModule.getUint(response, &pLayerProperties->sourceY)
            && gIpcModule.getUint(response, &pLayerProperties->sourceWidth)
            && gIpcModule.getUint(response, &pLayerProperties->sourceHeight)
            && gIpcModule.getUint(response, &pLayerProperties->origSourceWidth)
            && gIpcModule.getUint(response, &pLayerProperties->origSourceHeight)
            && gIpcModule.getUint(response, &pLayerProperties->destX)
            && gIpcModule.getUint(response, &pLayerProperties->destY)
            && gIpcModule.getUint(response, &pLayerProperties->destWidth)
            && gIpcModule.getUint(response, &pLayerProperties->destHeight)
            && gIpcModule.getUint(response, &pLayerProperties->orientation)
            && gIpcModule.getBool(response, &pLayerProperties->visibility)
            && gIpcModule.getUint(response, &pLayerProperties->type)
            && gIpcModule.getBool(response, &pLayerProperties->chromaKeyEnabled)
            && gIpcModule.getUint(response, &pLayerProperties->chromaKeyRed)
            && gIpcModule.getUint(response, &pLayerProperties->chromaKeyGreen)
            && gIpcModule.getUint(response, &pLayerProperties->chromaKeyBlue)
            && gIpcModule.getInt(response, &pLayerProperties->creatorPid))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_getNumberOfHardwareLayers(t_ilm_uint screenID, t_ilm_uint* pNumberOfHardwareLayers)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetNumberOfHardwareLayers");
    if (pNumberOfHardwareLayers
        && command
        && gIpcModule.appendUint(command, screenID)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUint(response, pNumberOfHardwareLayers))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_getScreenResolution(t_ilm_uint screenID, t_ilm_uint* pWidth, t_ilm_uint* pHeight)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetScreenResolution");
    if (pWidth && pHeight
        && command
        && gIpcModule.appendUint(command, screenID)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUint(response, pWidth)
            && gIpcModule.getUint(response, pHeight))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_getLayerIDs(t_ilm_int* pLength, t_ilm_layer** ppArray)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("ListAllLayerIDS");
    if (pLength && ppArray
        && command
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUintArray(response, ppArray, pLength))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_getLayerIDsOnScreen(t_ilm_uint screenId, t_ilm_int* pLength, t_ilm_layer** ppArray)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("ListAllLayerIDsOnScreen");
    if (pLength && ppArray
        && command
        && gIpcModule.appendUint(command, screenId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUintArray(response, ppArray, pLength))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_getSurfaceIDs(t_ilm_int* pLength, t_ilm_surface** ppArray)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("ListAllSurfaceIDS");
    if (pLength && ppArray
        && command
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUintArray(response, ppArray, pLength))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_getLayerGroupIDs(t_ilm_int* pLength, t_ilm_layergroup** ppArray)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("ListAllLayerGroupIDS");
    if (pLength && ppArray
        && command
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUintArray(response, ppArray, pLength))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_getSurfaceGroupIDs(t_ilm_int* pLength, t_ilm_surfacegroup** ppArray)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("ListAllSurfaceGroupIDS");
    if (pLength && ppArray
        && command
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUintArray(response, ppArray, pLength))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_getSurfaceIDsOnLayer(t_ilm_layer layer, t_ilm_int* pLength, t_ilm_surface** ppArray)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("ListSurfaceofLayer");
    if (pLength && ppArray
        && command
        && gIpcModule.appendUint(command, layer)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUintArray(response, ppArray, pLength))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerCreate(t_ilm_layer* pLayerId)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLayerId && (INVALID_ID != *pLayerId))
    {
        t_ilm_message command = gIpcModule.createMessage("CreateLayerFromId");
        if (command
            && gIpcModule.appendUint(command, *pLayerId)
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
                && gIpcModule.getUint(response, pLayerId))
            {
                returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
        gIpcModule.destroyMessage(command);
    }
    else
    {
        t_ilm_message command = gIpcModule.createMessage("CreateLayer");
        if (command
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
                && gIpcModule.getUint(response, pLayerId))
            {
                returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
        gIpcModule.destroyMessage(command);
    }
    return returnValue;
}

ilmErrorTypes ilm_layerCreateWithDimension(t_ilm_layer* pLayerId, t_ilm_uint width, t_ilm_uint height)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLayerId && (INVALID_ID != *pLayerId))
    {
        t_ilm_message command = gIpcModule.createMessage("CreateLayerFromIdWithDimension");
        if (command
            && gIpcModule.appendUint(command, *pLayerId)
            && gIpcModule.appendUint(command, width)
            && gIpcModule.appendUint(command, height)
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
                && gIpcModule.getUint(response, pLayerId))
            {
                returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
        gIpcModule.destroyMessage(command);
    }
    else
    {
        t_ilm_message command = gIpcModule.createMessage("CreateLayerWithDimension");
        if (command
            && gIpcModule.appendUint(command, width)
            && gIpcModule.appendUint(command, height)
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
                && gIpcModule.getUint(response, pLayerId))
            {
                returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
        gIpcModule.destroyMessage(command);
    }
    return returnValue;
}

ilmErrorTypes ilm_layerRemove(t_ilm_layer layerId)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("RemoveLayer");
    if (command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerAddSurface(t_ilm_layer layerId, t_ilm_surface surfaceId)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("AddSurfaceToLayer");
    if (command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerRemoveSurface(t_ilm_layer layerId, t_ilm_surface surfaceId)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("RemoveSurfaceFromLayer");
    if (command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerGetType(t_ilm_layer layerId, ilmLayerType* pLayerType)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetLayerType");
    if (pLayerType
        && command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUint(response, pLayerType))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerSetVisibility(t_ilm_layer layerId, t_ilm_bool newVisibility)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetLayerVisibility");
    if (command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.appendBool(command, newVisibility)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerGetVisibility(t_ilm_layer layerId, t_ilm_bool *pVisibility)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetLayerVisibility");
    if (pVisibility
        && command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getBool(response, pVisibility))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerSetOpacity(t_ilm_layer layerId, t_ilm_float opacity)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetLayerOpacity");
    if (command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.appendDouble(command, opacity)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerGetOpacity(t_ilm_layer layerId, t_ilm_float *pOpacity)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetLayerOpacity");
    if (pOpacity
        && command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getDouble(response, pOpacity))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerSetSourceRectangle(t_ilm_layer layerId, t_ilm_uint x, t_ilm_uint y, t_ilm_uint width, t_ilm_uint height)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetLayerSourceRegion");
    if (command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.appendUint(command, x)
        && gIpcModule.appendUint(command, y)
        && gIpcModule.appendUint(command, width)
        && gIpcModule.appendUint(command, height)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerSetDestinationRectangle(t_ilm_layer layerId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetLayerDestinationRegion");
    if (command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.appendUint(command, x)
        && gIpcModule.appendUint(command, y)
        && gIpcModule.appendUint(command, width)
        && gIpcModule.appendUint(command, height)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerGetDimension(t_ilm_layer layerId, t_ilm_uint *pDimension)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetLayerDimension");
    if (pDimension
        && command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUint(response, &pDimension[0])
            && gIpcModule.getUint(response, &pDimension[1]))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerSetDimension(t_ilm_layer layerId, t_ilm_uint *pDimension)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetLayerDimension");
    if (pDimension
        && command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.appendUint(command, pDimension[0])
        && gIpcModule.appendUint(command, pDimension[1])
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerGetPosition(t_ilm_layer layerId, t_ilm_uint *pPosition)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetLayerPosition");
    if (pPosition
        && command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUint(response, &pPosition[0])
            && gIpcModule.getUint(response, &pPosition[1]))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerSetPosition(t_ilm_layer layerId, t_ilm_uint *pPosition)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetLayerPosition");
    if (pPosition
        && command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.appendUint(command, pPosition[0])
        && gIpcModule.appendUint(command, pPosition[1])
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerSetOrientation(t_ilm_layer layerId, ilmOrientation orientation)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetLayerOrientation");
    if (command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.appendUint(command, orientation)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerGetOrientation(t_ilm_layer layerId, ilmOrientation *pOrientation)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetLayerOrientation");
    if (pOrientation
        && command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUint(response, pOrientation))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerSetChromaKey(t_ilm_layer layerId, t_ilm_int* pColor)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetLayerChromaKey");
    if (command
        && gIpcModule.appendUint(command, layerId))
    {
        t_ilm_bool comResult = ILM_TRUE;

        // Checking pColor has a content, otherwise chromakey is disabled
        if (pColor)
        {
            const t_ilm_uint number = 3;
            comResult = gIpcModule.appendUintArray(command, (t_ilm_uint *)pColor, number);
        }
        if (comResult
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
            {
               returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerSetRenderOrder(t_ilm_layer layerId, t_ilm_layer *pSurfaceId, t_ilm_int number)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetSurfaceRenderOrderWithinLayer");
    if (pSurfaceId
        && command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.appendUintArray(command, pSurfaceId, number)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerGetCapabilities(t_ilm_layer layerId, t_ilm_layercapabilities *pCapabilities)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetLayerCapabilities");
    if (pCapabilities
        && command
        && gIpcModule.appendUint(command, layerId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUint(response, pCapabilities))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerTypeGetCapabilities(ilmLayerType layerType, t_ilm_layercapabilities *pCapabilities)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetLayertypeCapabilities");
    if (pCapabilities
        && command
        && gIpcModule.appendUint(command, layerType)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUint(response, pCapabilities))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layergroupCreate(t_ilm_layergroup *pLayergroup)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pLayergroup && (INVALID_ID != *pLayergroup))
    {
        t_ilm_message command = gIpcModule.createMessage("CreateLayerGroupFromId");
        if (pLayergroup
            && command
            && gIpcModule.appendUint(command, *pLayergroup)
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
                && gIpcModule.getUint(response, pLayergroup))
            {
                returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
        gIpcModule.destroyMessage(command);
    }
    else
    {
        t_ilm_message command = gIpcModule.createMessage("CreateLayerGroup");
        if (pLayergroup
            && command
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
                && gIpcModule.getUint(response, pLayergroup))
            {
                returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
        gIpcModule.destroyMessage(command);
    }
    return returnValue;
}

ilmErrorTypes ilm_layergroupRemove(t_ilm_layergroup group)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("RemoveLayerGroup");
    if (command
        && gIpcModule.appendUint(command, group)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layergroupAddLayer(t_ilm_layergroup group, t_ilm_layer layer)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("AddLayerToLayerGroup");
    if (command
        && gIpcModule.appendUint(command, layer)
        && gIpcModule.appendUint(command, group)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layergroupRemoveLayer(t_ilm_layergroup group, t_ilm_layer layer)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("RemoveLayerFromLayerGroup");
    if (command
        && gIpcModule.appendUint(command, layer)
        && gIpcModule.appendUint(command, group)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layergroupSetVisibility(t_ilm_layergroup group, t_ilm_bool newVisibility)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetLayergroupVisibility");
    if (command
        && gIpcModule.appendUint(command, group)
        && gIpcModule.appendBool(command, newVisibility)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layergroupSetOpacity(t_ilm_layergroup group, t_ilm_float opacity)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetLayergroupOpacity");
    if (command
        && gIpcModule.appendUint(command, group)
        && gIpcModule.appendDouble(command, opacity)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceCreate(t_ilm_nativehandle nativehandle, t_ilm_int width, t_ilm_int height, ilmPixelFormat pixelFormat, t_ilm_surface* pSurfaceId)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pSurfaceId && (INVALID_ID != *pSurfaceId))
    {
        t_ilm_message command = gIpcModule.createMessage("CreateSurfaceFromId");
        if (command
            && gIpcModule.appendUint(command, nativehandle)
            && gIpcModule.appendUint(command, width)
            && gIpcModule.appendUint(command, height)
            && gIpcModule.appendUint(command, pixelFormat)
            && gIpcModule.appendUint(command, *pSurfaceId)
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
                && gIpcModule.getUint(response, pSurfaceId))
            {
                returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
        gIpcModule.destroyMessage(command);
    }
    else
    {
        t_ilm_message command = gIpcModule.createMessage("CreateSurface");
        if (command
            && gIpcModule.appendUint(command, nativehandle)
            && gIpcModule.appendUint(command, width)
            && gIpcModule.appendUint(command, height)
            && gIpcModule.appendUint(command, pixelFormat)
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
                && gIpcModule.getUint(response, pSurfaceId))
            {
                returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
        gIpcModule.destroyMessage(command);
    }
    return returnValue;
}

ilmErrorTypes ilm_surfaceInitialize(t_ilm_surface *pSurfaceId)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pSurfaceId && (INVALID_ID != *pSurfaceId))
    {
        t_ilm_message command = gIpcModule.createMessage("InitializeSurfaceFromId");
        if (command
            && gIpcModule.appendUint(command, *pSurfaceId)
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
                && gIpcModule.getUint(response, pSurfaceId))
            {
                returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
        gIpcModule.destroyMessage(command);
    }
    else
    {
        t_ilm_message command = gIpcModule.createMessage("InitializeSurface");
        if (command
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
                && gIpcModule.getUint(response, pSurfaceId))
            {
                returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
        gIpcModule.destroyMessage(command);
    }
    return returnValue;
}

ilmErrorTypes ilm_surfaceSetNativeContent(t_ilm_nativehandle nativehandle, t_ilm_int width, t_ilm_int height, ilmPixelFormat pixelFormat, t_ilm_surface surfaceId)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetSurfaceNativeContent");
    if (command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.appendUint(command, nativehandle)
        && gIpcModule.appendUint(command, width)
        && gIpcModule.appendUint(command, height)
        && gIpcModule.appendUint(command, pixelFormat)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceRemoveNativeContent(t_ilm_surface surfaceId)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("RemoveSurfaceNativeContent");
    if (command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceRemove(t_ilm_surface surfaceId)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("RemoveSurface");
    if (command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceSetVisibility(t_ilm_surface surfaceId, t_ilm_bool newVisibility)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetSurfaceVisibility");
    if (command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.appendBool(command, newVisibility)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceGetVisibility(t_ilm_surface surfaceId, t_ilm_bool *pVisibility)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetSurfaceVisibility");
    if (pVisibility
        && command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getBool(response, pVisibility))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceSetOpacity(t_ilm_surface surfaceId, t_ilm_float opacity)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetSurfaceOpacity");
    if (command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.appendDouble(command, opacity)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceGetOpacity(t_ilm_surface surfaceId, t_ilm_float *pOpacity)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetSurfaceOpacity");
    if (pOpacity
        && command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getDouble(response, pOpacity))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceSetSourceRectangle(t_ilm_surface surfaceId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetSurfaceSourceRegion");
    if (command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.appendUint(command, x)
        && gIpcModule.appendUint(command, y)
        && gIpcModule.appendUint(command, width)
        && gIpcModule.appendUint(command, height)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceSetDestinationRectangle(t_ilm_surface surfaceId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetSurfaceDestinationRegion");
    if (command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.appendUint(command, x)
        && gIpcModule.appendUint(command, y)
        && gIpcModule.appendUint(command, width)
        && gIpcModule.appendUint(command, height)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceGetDimension(t_ilm_surface surfaceId, t_ilm_uint *pDimension)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetSurfaceDimension");
    if (pDimension
        && command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUint(response, &pDimension[0])
            && gIpcModule.getUint(response, &pDimension[1]))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceSetDimension(t_ilm_surface surfaceId, t_ilm_uint *pDimension)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetSurfaceDimension");
    if (pDimension
        && command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.appendUint(command, pDimension[0])
        && gIpcModule.appendUint(command, pDimension[1])
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceGetPosition(t_ilm_surface surfaceId, t_ilm_uint *pPosition)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetSurfacePosition");
    if (pPosition
        && command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUint(response, &pPosition[0])
            && gIpcModule.getUint(response, &pPosition[1]))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceSetPosition(t_ilm_surface surfaceId, t_ilm_uint *pPosition)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetSurfacePosition");
    if (pPosition
        && command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.appendUint(command, pPosition[0])
        && gIpcModule.appendUint(command, pPosition[1])
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceSetOrientation(t_ilm_surface surfaceId, ilmOrientation orientation)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetSurfaceOrientation");
    if (command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.appendUint(command, orientation)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceGetOrientation(t_ilm_surface surfaceId, ilmOrientation *pOrientation)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetSurfaceOrientation");
    if (pOrientation
        && command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUint(response, pOrientation))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceGetPixelformat(t_ilm_layer surfaceId, ilmPixelFormat *pPixelformat)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetSurfacePixelformat");
    if (pPixelformat
        && command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUint(response, pPixelformat))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceSetChromaKey(t_ilm_surface surfaceId, t_ilm_int* pColor)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetSurfaceChromaKey");
    if (command
        && gIpcModule.appendUint(command, surfaceId))
    {
        t_ilm_bool comResult = ILM_TRUE;

        // Checking pColor has a content, otherwise chromakey is disabled
        if (pColor)
        {
            const t_ilm_uint number = 3;
            comResult = gIpcModule.appendUintArray(command, (t_ilm_uint *)pColor, number);
        }
        if (comResult
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
            {
               returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceInvalidateRectangle(t_ilm_surface surfaceId)
{
    ilmErrorTypes error = ILM_FAILED;

    // TODO: Implement this on both server and client

    return error;
}

ilmErrorTypes ilm_surfacegroupCreate(t_ilm_surfacegroup *pSurfacegroup)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    if (pSurfacegroup && (INVALID_ID != *pSurfacegroup))
    {
        t_ilm_message command = gIpcModule.createMessage("CreateSurfaceGroupFromId");
        if (command
            && gIpcModule.appendUint(command, *pSurfacegroup)
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
                && gIpcModule.getUint(response, pSurfacegroup))
            {
                returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
        gIpcModule.destroyMessage(command);
    }
    else
    {
        t_ilm_message command = gIpcModule.createMessage("CreateSurfaceGroup");
        if (pSurfacegroup
            && command
            && gIpcModule.sendToService(command))
        {
            t_ilm_message response = waitForResponse(gResponseTimeout);
            if (response
                && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
                && gIpcModule.getUint(response, pSurfacegroup))
            {
                returnValue = ILM_SUCCESS;
            }
            gIpcModule.destroyMessage(response);
        }
        gIpcModule.destroyMessage(command);
    }
    return returnValue;
}

ilmErrorTypes ilm_surfacegroupRemove(t_ilm_surfacegroup group)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("RemoveSurfaceGroup");
    if (command
        && gIpcModule.appendUint(command, group)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfacegroupAddSurface(t_ilm_surfacegroup group, t_ilm_surface surface)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("AddSurfaceToSurfaceGroup");
    if (command
        && gIpcModule.appendUint(command, surface)
        && gIpcModule.appendUint(command, group)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfacegroupRemoveSurface(t_ilm_surfacegroup group, t_ilm_surface surface)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("RemoveSurfaceFromSurfaceGroup");
    if (command
        && gIpcModule.appendUint(command, surface)
        && gIpcModule.appendUint(command, group)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfacegroupSetVisibility(t_ilm_surfacegroup group, t_ilm_bool newVisibility)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetSurfacegroupVisibility");
    if (command
        && gIpcModule.appendUint(command, group)
        && gIpcModule.appendBool(command, newVisibility)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfacegroupSetOpacity(t_ilm_surfacegroup group, t_ilm_float opacity)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetSurfacegroupOpacity");
    if (command
        && gIpcModule.appendUint(command, group)
        && gIpcModule.appendDouble(command, opacity)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_displaySetRenderOrder(t_ilm_display display, t_ilm_layer *pLayerId, const t_ilm_uint number)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetRenderOrderOfLayers");
    if (pLayerId
        && command
        && gIpcModule.appendUintArray(command, pLayerId, number)
        && gIpcModule.appendUint(command, display)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_getScreenIDs(t_ilm_uint* pNumberOfIDs, t_ilm_uint** ppIDs)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetScreenIDs");
    if (pNumberOfIDs && ppIDs
        && command
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUintArray(response, ppIDs, (t_ilm_int *)pNumberOfIDs))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_takeScreenshot(t_ilm_uint screen, t_ilm_const_string filename)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("ScreenShot");
    if (command
        && gIpcModule.appendUint(command, screen)
        && gIpcModule.appendString(command, filename)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_takeLayerScreenshot(t_ilm_const_string filename, t_ilm_layer layerid)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("ScreenShotOfLayer");
    if (command
        && gIpcModule.appendString(command, filename)
        && gIpcModule.appendUint(command, layerid)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_takeSurfaceScreenshot(t_ilm_const_string filename, t_ilm_surface surfaceid)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("ScreenShotOfSurface");
    if (command
        && gIpcModule.appendString(command, filename)
        && gIpcModule.appendUint(command, surfaceid)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_SetKeyboardFocusOn(t_ilm_surface surfaceId)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("SetKeyboardFocusOn");
    if (command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_GetKeyboardFocusSurfaceId(t_ilm_surface* pSurfaceId)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("GetKeyboardFocusSurfaceId");
    if (command
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand
            && gIpcModule.getUint(response, pSurfaceId))
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_UpdateInputEventAcceptanceOn(t_ilm_surface surfaceId, ilmInputDevice devices, t_ilm_bool acceptance)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("UpdateInputEventAcceptanceOn");
    if (command
        && gIpcModule.appendUint(command, surfaceId)
        && gIpcModule.appendUint(command, devices)
        && gIpcModule.appendBool(command, acceptance)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_commitChanges()
{
    ilmErrorTypes returnValue = ILM_FAILED;

    t_ilm_message command = gIpcModule.createMessage("CommitChanges");
    if (command
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerAddNotification(t_ilm_layer layer, layerNotificationFunc callback)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    if (findLayerCallback(layer))
    {
        return ILM_ERROR_INVALID_ARGUMENTS;
    }

    t_ilm_message command = gIpcModule.createMessage("LayerAddNotification");
    if (command
        && gIpcModule.appendUint(command, layer)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            addLayerCallback(layer, callback);
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_layerRemoveNotification(t_ilm_layer layer)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    if (!findLayerCallback(layer))
    {
        return ILM_ERROR_INVALID_ARGUMENTS;
    }

    t_ilm_message command = gIpcModule.createMessage("LayerRemoveNotification");
    if (command
        && gIpcModule.appendUint(command, layer)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            removeLayerCallback(layer);
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceAddNotification(t_ilm_surface surface, surfaceNotificationFunc callback)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    if (findSurfaceCallback(surface))
    {
        return ILM_ERROR_INVALID_ARGUMENTS;
    }

    t_ilm_message command = gIpcModule.createMessage("SurfaceAddNotification");
    if (command
        && gIpcModule.appendUint(command, surface)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            addSurfaceCallback(surface, callback);
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}

ilmErrorTypes ilm_surfaceRemoveNotification(t_ilm_surface surface)
{
    ilmErrorTypes returnValue = ILM_FAILED;

    if (!findSurfaceCallback(surface))
    {
        return ILM_ERROR_INVALID_ARGUMENTS;
    }

    t_ilm_message command = gIpcModule.createMessage("SurfaceRemoveNotification");
    if (command
        && gIpcModule.appendUint(command, surface)
        && gIpcModule.sendToService(command))
    {
        t_ilm_message response = waitForResponse(gResponseTimeout);
        if (response
            && gIpcModule.getMessageType(response) == IpcMessageTypeCommand)
        {
            removeSurfaceCallback(surface);
            returnValue = ILM_SUCCESS;
        }
        gIpcModule.destroyMessage(response);
    }
    gIpcModule.destroyMessage(command);
    return returnValue;
}
