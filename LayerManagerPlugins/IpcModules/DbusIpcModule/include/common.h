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
#ifndef __COMMON_H__
#define __COMMON_H__

#include "ilm_types.h"
#include <dbus/dbus.h>

//=============================================================================
// logging
//=============================================================================
//#define LOG_ENTER_FUNCTION printf("      --> DBUSIpcModule::%s()\n", __FUNCTION__)
#define LOG_ENTER_FUNCTION


//=============================================================================
// data structures
//=============================================================================
typedef struct
{
    t_ilm_const_string name;
    t_ilm_const_string sender;
    DBusMessage* pMessage;
    DBusMessageIter iter;
    DBusPendingCall* pPending;
} dbusmessage;

typedef struct
{
    DBusConnection* connection;
    DBusBusType type;
    DBusError error;
    t_ilm_bool initialized;
    t_ilm_bool isClient;
} dbus;

//=============================================================================
// global variables
//=============================================================================
dbusmessage* gpCurrentMessage;
dbus* gpDbusState;

//=============================================================================
// prototypes
//=============================================================================
void printTypeName(int type);
void unregisterMessageFunc(DBusConnection* conn, void *user_data);

//=============================================================================
// callback functions
//=============================================================================
void callbackUnregister(DBusConnection *connection, void *user_data);
DBusHandlerResult callbackMessage(DBusConnection *connection, DBusMessage *message, void *user_data);


#endif // __COMMON_H__
