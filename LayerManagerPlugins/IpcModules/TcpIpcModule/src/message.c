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
#include "IpcModule.h"
#include "socketShared.h"
#include <errno.h>
#include <stdio.h>
#include <sys/select.h>


//=============================================================================
// prototypes
//=============================================================================
t_ilm_bool acceptClientConnection();
int receiveFromSocket();


//=============================================================================
// message handling
//=============================================================================
t_ilm_bool createMessage(t_ilm_const_string name)
{
    LOG_ENTER_FUNCTION;

    memset(&gState.message, 0, sizeof(gState.message));

    gState.message.type = SOCKET_MESSAGE_TYPE_NORMAL;
    gState.readIndex = 0;
    gState.writeIndex = 0;

    return appendString(name);
}

t_ilm_bool sendMessage()
{
    LOG_ENTER_FUNCTION;
    int activesocket = 0;
    int sentBytes = 0;
    int retVal = 0;

    int headerSize = sizeof(gState.message) - sizeof(gState.message.data);
    gState.message.size = gState.writeIndex + headerSize;

    if (gState.socket.isClient)
    {
        activesocket = gState.socket.server;
    }
    else
    {
        activesocket = gState.senderSocket;
    }

    int sendSize = gState.message.size;

    do
    {
        retVal += send(activesocket,
                       &gState.message + sentBytes,
                       sendSize - sentBytes,
                       0);
        sentBytes += retVal;
    } while (retVal > 0 && sentBytes < sendSize);

    //printf("      --> TcpIpcModule: %d bytes sent to socket %d\n", sentBytes, activesocket);

    return (sentBytes == sendSize) ? ILM_TRUE : ILM_FALSE;
}

t_ilm_bool sendError(t_ilm_const_string desc)
{
    LOG_ENTER_FUNCTION;

    gState.message.type = SOCKET_MESSAGE_TYPE_ERROR;

    // reset content to error description
    gState.writeIndex = 0;
    appendString(desc);

    return sendMessage();
}

enum IpcMessageType receiveMessage(t_ilm_int timeoutInMs)
{
    LOG_ENTER_FUNCTION;
    enum IpcMessageType result = IpcMessageTypeNone;

    gState.readIndex = 0;
    gState.writeIndex = 0;

    fd_set readFds = gState.socket.monitoredSockets;

    struct timeval timeoutValue;
    timeoutValue.tv_sec = timeoutInMs / 1000;
    timeoutValue.tv_usec = (timeoutInMs % 1000) * 1000;

    int numberOfFdsReady = select(gState.socket.monitoredSocketMax + 1, &readFds, 0, 0, &timeoutValue);

    if (-1  == numberOfFdsReady)
    {
        printf("TcpIpcModule: select() failed\n");
    }
    else if (0 < numberOfFdsReady)
    {
        int socketNumber;
        for (socketNumber = 0; socketNumber <= gState.socket.monitoredSocketMax; ++socketNumber)
        {
            if (FD_ISSET(socketNumber, &readFds))
            {
                if (!gState.socket.isClient)
                {
                    if (gState.socket.server == socketNumber)
                    {
                        // New client connected
                        acceptClientConnection();
                        result = IpcMessageTypeNone;  // no data received
                        continue;
                    }

                    // receive data from socket
                    gState.senderSocket = socketNumber;
                    receiveFromSocket();

                    if(gState.message.size > 0)
                    {
                        // new message from client
                        getString(gState.messageName);
                        result = IpcMessageTypeCommand;
                        continue;
                    }

                    if(gState.message.size == 0)
                    {
                        // client disconnected
                        close(socketNumber);
                        FD_CLR(socketNumber, &gState.socket.monitoredSockets);
                        result = IpcMessageTypeDisconnect;
                        continue;
                    }

                    // error
                    const char* errorMsg = (char*)strerror(errno);
                    printf("      --> TcpIpcModule: Error receiving data from socket %d (%s)\n",
                           gState.senderSocket, errorMsg);
                    result = IpcMessageTypeError;
                }
                else
                {
                    // receive LayerManager response
                    gState.senderSocket = socketNumber;
                    receiveFromSocket(gState.senderSocket);
                    getString(gState.messageName);
                    result = IpcMessageTypeCommand;
                }
            }
        }
    }

    return result;
}

t_ilm_const_string getMessageName()
{
    LOG_ENTER_FUNCTION;
    return gState.messageName;
}

t_ilm_bool isErrorMessage()
{
    LOG_ENTER_FUNCTION;
    return (SOCKET_MESSAGE_TYPE_ERROR == gState.message.type);
}

t_ilm_const_string getSenderName()
{
    LOG_ENTER_FUNCTION;
    char buffer[16];
    sprintf(buffer, "socket %d", gState.senderSocket);
    return strdup(buffer);
}

//=============================================================================
//private
//=============================================================================
t_ilm_bool acceptClientConnection()
{
    LOG_ENTER_FUNCTION;

    t_ilm_bool result = ILM_TRUE;
    unsigned int clientlen = sizeof(gState.socket.clientAddrIn);

    gState.socket.client = accept(gState.socket.server, (struct sockaddr *) &gState.socket.clientAddrIn, &clientlen);

    if (gState.socket.client < 0)
    {
        printf("TcpIpcModule: accept() failed.\n");
        result = ILM_FALSE;
    }

    FD_SET(gState.socket.client, &gState.socket.monitoredSockets);
    gState.socket.monitoredSocketMax = (gState.socket.monitoredSocketMax > gState.socket.client) ? gState.socket.monitoredSocketMax : gState.socket.client;

    return result;
}

int receiveFromSocket()
{
    LOG_ENTER_FUNCTION;
    int receivedBytes = 0;
    int retVal = 0;

    // receive header in first run (contains message size)
    gState.message.size = sizeof(gState.message) - sizeof(gState.message.data);

    char* messageBuffer = (char*)&gState.message;

    do
    {
        retVal = recv(gState.senderSocket,
                      &messageBuffer[receivedBytes],
                      gState.message.size - receivedBytes,
                      0);
        receivedBytes += retVal;
    } while ((retVal > 0) && (receivedBytes < gState.message.size));

    if (0 == retVal)
    {
        // client disconnect
        gState.message.size = 0;
    }
}


//=============================================================================
// logging / debugging
//=============================================================================
void printMessage()
{
    LOG_ENTER_FUNCTION;

    printf("          --> Message type: %s\n", (gState.message.type == SOCKET_MESSAGE_TYPE_NORMAL) ? "normal" : "error");
    printf("          --> Message size: %d\n", gState.message.size);

    int i = 0;

    char headersize = sizeof(gState.message) - sizeof(gState.message.data);

    // iterate over message data
    while (i < gState.message.size - headersize)
    {
        // read type information
        char type  = gState.message.data[i++];
        char datasize = gState.message.data[i++];

        // set pointer to value data
        char* data = &gState.message.data[i];

        // print data
        switch (type)
        {
        case SOCKET_MESSAGE_TYPE_ARRAY:
            printf("          --> Array, %d entries:\n", datasize);
            datasize = 0;
            break;

        case SOCKET_MESSAGE_TYPE_STRING:
            printf("          --> String: '%s' (%d bytes)\n", data, datasize);
            break;

        case SOCKET_MESSAGE_TYPE_UINT:
            printf("          --> UInt: %u (%d bytes)\n", *(t_ilm_uint*)data, datasize);
            break;

        case SOCKET_MESSAGE_TYPE_INT:
            printf("          --> Int: %d (%d bytes)\n", *(t_ilm_int*)data, datasize);
            break;

        case SOCKET_MESSAGE_TYPE_DOUBLE:
            printf("          --> Double: %lf (%d bytes)\n", *(t_ilm_float*)data, datasize);
            break;

        case SOCKET_MESSAGE_TYPE_BOOL:
            printf("          --> Bool: %d (%d bytes)\n", *data, datasize);
            break;

        default:
            printf("          --> Unknown type (%s), %d bytes\n", (char*)&type, datasize);
            break;
        }

        // skip already processed data
        i += datasize;
    }
}

