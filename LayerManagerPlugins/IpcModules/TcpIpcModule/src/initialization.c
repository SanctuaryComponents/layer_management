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
#include <stdio.h>
#include <stdlib.h>  // getenv
#include <string.h>  // memset


t_ilm_bool init(t_ilm_bool isClient)
{
    LOG_ENTER_FUNCTION;
    t_ilm_bool result = ILM_TRUE;

    struct SocketState* sock = &gState.socket;

    sock->isClient = isClient;

    sock->server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock->server < 0)
    {
        printf("TcpIpcModule: socket()...failed\n");
        result = ILM_FALSE;
    }

    const char* portString = getenv(ENV_TCP_PORT);
    int port = SOCKET_TCP_PORT;
    if (portString)
    {
        port = atoi(portString);
    }

    sock->serverAddrIn.sin_family = AF_INET;
    sock->serverAddrIn.sin_port = htons(port);
    memset(&(sock->serverAddrIn.sin_zero), '\0', 8);

    if (sock->isClient)  // Client
    {
        const char* hostname = getenv(ENV_TCP_HOST);
        if (!hostname)
        {
            hostname = SOCKET_TCP_HOST;
        }

        struct hostent* server;
        server = gethostbyname(hostname);
        if (!server)
        {
            printf("TcpIpcModule: error: could not resolve host '%s'.\n", hostname);
            result = ILM_FALSE;
        }
        else
        {
            memcpy(&sock->serverAddrIn.sin_addr.s_addr, server->h_addr, server->h_length);
        }

        if (0 != connect(sock->server,
                         (struct sockaddr *) &sock->serverAddrIn,
                         sizeof(sock->serverAddrIn)))
        {
            result = ILM_FALSE;
        }

        printf("TcpIpcModule: connection to %s:%d %s.\n",
                hostname, port,
                (ILM_TRUE == result) ? "established" : "failed");

        FD_SET(sock->server, &sock->monitoredSockets);
        sock->monitoredSocketMax = sock->server;
    }
    else  // LayerManagerService
    {
        int on = 1;
        setsockopt(sock->server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        sock->serverAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

        if (0 > bind(sock->server,
                    (struct sockaddr *) &sock->serverAddrIn,
                     sizeof(sock->serverAddrIn)))
        {
            printf("TcpIpcModule: bind()...failed\n");
            result = ILM_FALSE;
        }

        if (listen(sock->server, SOCKET_MAX_PENDING_CONNECTIONS) < 0)
        {
            printf("TcpIpcModule: listen()...failed\n");
            result = ILM_FALSE;
        }

        FD_SET(sock->server, &sock->monitoredSockets);
        sock->monitoredSocketMax = sock->server;

        printf("TcpIpcModule: listening to TCP port: %d\n", port);
    }

    return result;
}

t_ilm_bool destroy()
{
    LOG_ENTER_FUNCTION;
    int socketNumber;
    for (socketNumber = 0; socketNumber <= gState.socket.monitoredSocketMax; ++socketNumber)
    {
        if (FD_ISSET(socketNumber, &gState.socket.monitoredSockets))
        {
            printf("TcpIpcModule: Closing socket %d\n", socketNumber);
            close(socketNumber);
        }
    }
    return ILM_TRUE;
}

