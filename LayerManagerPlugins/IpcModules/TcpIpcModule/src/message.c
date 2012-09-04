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
void receiveFromSocket(int socketNumber);


//=============================================================================
// message handling
//=============================================================================
t_ilm_bool createMessage(t_ilm_const_string name)
{
    memset(&gState.outgoingMessage, 0, sizeof(gState.outgoingMessage));

    gState.outgoingMessage.paket.type = SOCKET_MESSAGE_TYPE_NORMAL;
    gState.outgoingMessage.index = 0;

    return appendString(name);
}

t_ilm_bool destroyMessage()
{
    t_ilm_bool returnValue = ILM_TRUE;
    
    /* to be implemented if needed */
       
    return returnValue;
}

t_ilm_bool sendMessage()
{
    int activesocket = 0;
    int sentBytes = 0;
    int retVal = 0;

    int headerSize = sizeof(gState.outgoingMessage.paket) - sizeof(gState.outgoingMessage.paket.data);
    gState.outgoingMessage.paket.size = gState.outgoingMessage.index + headerSize;

    if (gState.isClient)
    {
        activesocket = gState.socket;
    }
    else
    {
        activesocket = gState.incomingMessage[gState.incomingQueueIndex].sender;
    }

    int sendSize = gState.outgoingMessage.paket.size;

    do
    {
        retVal += send(activesocket,
                       &gState.outgoingMessage.paket + sentBytes,
                       sendSize - sentBytes,
                       0);
        sentBytes += retVal;
    } while (retVal > 0 && sentBytes < sendSize);

    //printf("      --> TcpIpcModule: %d bytes sent to socket %d\n", sentBytes, activesocket);

    return (sentBytes == sendSize) ? ILM_TRUE : ILM_FALSE;
}

t_ilm_bool sendError(t_ilm_const_string desc)
{
    gState.outgoingMessage.paket.type = SOCKET_MESSAGE_TYPE_ERROR;

    // reset content to error description
    gState.outgoingMessage.index = 0;
    appendString(desc);

    return sendMessage();
}

enum IpcMessageType receiveMessage(t_ilm_int timeoutInMs)
{
    enum IpcMessageType result = IpcMessageTypeNone;

    // switch to next receive buffer
    gState.incomingQueueIndex = ++gState.incomingQueueIndex % SOCKET_MESSAGE_BUFFER_COUNT;

    struct SocketMessage* msg = &gState.incomingMessage[gState.incomingQueueIndex];

    msg->index = 0;

    fd_set readFds = gState.monitoredSockets;

    struct timeval timeoutValue;
    timeoutValue.tv_sec = timeoutInMs / 1000;
    timeoutValue.tv_usec = (timeoutInMs % 1000) * 1000;

    int numberOfFdsReady = select(gState.monitoredSocketMax + 1, &readFds, 0, 0, &timeoutValue);

    if (-1  == numberOfFdsReady)
    {
        printf("TcpIpcModule: select() failed\n");
    }
    else if (0 < numberOfFdsReady)
    {
        int socketNumber;
        for (socketNumber = 0; socketNumber <= gState.monitoredSocketMax; ++socketNumber)
        {
            if (FD_ISSET(socketNumber, &readFds))
            {
                if (!gState.isClient)
                {
                    if (gState.socket == socketNumber)
                    {
                        // New client connected
                        acceptClientConnection();
                        result = IpcMessageTypeNone;  // no data received
                        continue;
                    }

                    // receive data from socket
                    receiveFromSocket(socketNumber);

                    if(msg->paket.size > 0)
                    {
                        // new message from client
                        getString(msg->name);
                        result = IpcMessageTypeCommand;
                        continue;
                    }

                    if(msg->paket.size == 0)
                    {
                        // client disconnected
                        close(socketNumber);
                        FD_CLR(socketNumber, &gState.monitoredSockets);
                        result = IpcMessageTypeDisconnect;
                        continue;
                    }

                    // error
                    const char* errorMsg = (char*)strerror(errno);
                    printf("      --> TcpIpcModule: Error receiving data from socket %d (%s)\n",
                           msg->sender, errorMsg);
                    result = IpcMessageTypeError;
                }
                else
                {
                    // receive LayerManager response
                    receiveFromSocket(socketNumber);
                    getString(msg->name);
                    result = IpcMessageTypeCommand;
                }
            }
        }
    }

    return result;
}

t_ilm_const_string getMessageName()
{
    return gState.incomingMessage[gState.incomingQueueIndex].name;
}

t_ilm_bool isErrorMessage()
{
    return (SOCKET_MESSAGE_TYPE_ERROR == gState.incomingMessage[gState.incomingQueueIndex].paket.type);
}

t_ilm_const_string getSenderName()
{
    char buffer[16];
    sprintf(buffer, "socket %d", gState.incomingMessage[gState.incomingQueueIndex].sender);
    return strdup(buffer);
}

//=============================================================================
//private
//=============================================================================
t_ilm_bool acceptClientConnection()
{
    t_ilm_bool result = ILM_TRUE;
    unsigned int clientlen = sizeof(gState.clientAddrIn);

    int clientSocket = accept(gState.socket, (struct sockaddr *) &gState.clientAddrIn, &clientlen);

    if (clientSocket < 0)
    {
        printf("TcpIpcModule: accept() failed.\n");
        result = ILM_FALSE;
    }

    FD_SET(clientSocket, &gState.monitoredSockets);
    gState.monitoredSocketMax = (gState.monitoredSocketMax > clientSocket) ? gState.monitoredSocketMax : clientSocket;

    return result;
}

void receiveFromSocket(int socketNumber)
{
    int receivedBytes = 0;
    int retVal = 0;

    struct SocketMessage* msg = &gState.incomingMessage[gState.incomingQueueIndex];

    msg->sender = socketNumber;

    // receive header in first run (contains message size)
    msg->paket.size = sizeof(msg->paket) - sizeof(msg->paket.data);

    char* messageBuffer = (char*)&msg->paket;

    do
    {
        retVal = recv(msg->sender,
                      &messageBuffer[receivedBytes],
                      msg->paket.size - receivedBytes,
                      0);
        receivedBytes += retVal;
    } while ((retVal > 0) && (receivedBytes < msg->paket.size));

    if (0 == retVal)
    {
        // client disconnect
        msg->paket.size = 0;
    }
}

