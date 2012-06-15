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

#ifndef __SOCKETSHARED_H__
#define __SOCKETSHARED_H__

#include "socketConfiguration.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>  // struct hostent
#include <string.h>
#include <stdio.h>

//=============================================================================
// type definitions
//=============================================================================
struct SocketState
{
    t_ilm_bool isClient;
    int server;
    int client;
    struct sockaddr_in serverAddrIn;
    struct sockaddr_in clientAddrIn;
    fd_set monitoredSockets;
    int monitoredSocketMax;
};

struct SocketMessage
{
    int size;
    int type;
    char data[SOCKET_MAX_MESSAGE_SIZE];
};

struct State
{
    struct SocketState socket;
    struct SocketMessage message;
    unsigned int writeIndex;
    unsigned int readIndex;
    int senderSocket;
    char messageName[128];
};


//=============================================================================
// global variables
//=============================================================================
struct State gState;


//=============================================================================
// logging / debugging
//=============================================================================
//#define LOG_ENTER_FUNCTION printf("      --> TcpSocketIpcModule::%s()\n", __FUNCTION__)
#define LOG_ENTER_FUNCTION

void printMessage();


#endif // __SOCKETSHARED_H__
