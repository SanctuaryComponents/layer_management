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
#include "DBUSIntrospection.h"
#include "DBUSCommunicator.h"
#include "DBUSConfiguration.h"

#include <string.h>
#include <sstream>
using std::stringstream;


DBUSIntrospection::DBUSIntrospection(MethodTable* methodTable)
: m_methodTable(methodTable)
{
    generateString();
}

void DBUSIntrospection::generateString()
{
    LOG_DEBUG("DBUSCommunicator", "Generating introspection data");

    addHeader();
    openNode(DBUS_SERVICE_PREFIX);
    openInterface("org.freedesktop.DBus.Introspectable");
    openMethod("Introspect");
    addArgument("data", "out", "s");
    closeMethod();
    closeInterface();
    openInterface(DBUS_SERVICE_PREFIX);

    int index = 0;

    while (strcmp(m_methodTable[index].name, "") != 0)
    {
        MethodTable entry = m_methodTable[index];
        addEntry(entry);
        ++index;
    }

    closeInterface();

    closeNode();
    //LOG_DEBUG("DBUSCommunicator", "generated introspection data");
}

void DBUSIntrospection::addHeader(void)
{
    m_introspectionString << "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS"
        << "Object Introspection 1.0//EN\"\n \"http://www.freedesktop.org/standards/"
        << "dbus/1.0/introspect.dtd\"> \n";
}

void DBUSIntrospection::openNode(string nodename)
{
    m_introspectionString << "<node name=\"" << nodename << "\">  \n";
}

void DBUSIntrospection::openInterface(string interfacename)
{
    m_introspectionString << "<interface name=\"" << interfacename << "\">  \n";
}

void DBUSIntrospection::openMethod(string methodname)
{
    m_introspectionString << "<method name=\"" << methodname << "\">  \n";
}

void DBUSIntrospection::addArgument(string argname, string direction, string type)
{
    m_introspectionString << "<arg name=\"" << argname << "\" direction=\""
        << direction << "\" type=\"" << type << "\"/>  \n";
}

void DBUSIntrospection::closeMethod(void)
{
    m_introspectionString << "</method>  \n";
}

void DBUSIntrospection::closeInterface(void)
{
    m_introspectionString << "</interface>  \n";
}

void DBUSIntrospection::closeNode(void)
{
    m_introspectionString << "</node>  \n";
}


void DBUSIntrospection::addEntry(MethodTable entry)
{
    string methodName = entry.name;
    string parameterArray = entry.signature;
    string returnValueArray = string(entry.reply);

    openMethod(methodName);

    for(uint parameterIndex = 0; parameterIndex < parameterArray.length(); ++parameterIndex)
    {
        switch (parameterArray.at(parameterIndex))
        {
            case 'a':
                parameterIndex++;
                addArgument("", "in", "a" + parameterArray.at(parameterIndex));
                break;
            default:
                addArgument("", "in", "i");
                break;
        }
    }

    for(uint returnValueIndex = 0; returnValueIndex < returnValueArray.length(); ++returnValueIndex)
    {
        switch (returnValueArray.at(returnValueIndex))
        {
            case 'a':
                returnValueIndex++;
                addArgument("", "out", "a" + returnValueArray.at(returnValueIndex));
                break;
            default:
                addArgument("", "out", "i");
                break;
        }
    }

    closeMethod();
}

void DBUSIntrospection::process(DBusConnection* conn, DBusMessage* msg)
{
    DBusMessage * reply;
    DBusMessageIter args;
    dbus_uint32_t serial = 0;

    // create a reply from the message
    reply = dbus_message_new_method_return(msg);

    string introspect = m_introspectionString.str();
    const char* string = introspect.c_str();

    // add the arguments to the reply
    dbus_message_iter_init_append(reply, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &string))
    {
        LOG_ERROR("DBUSCommunicator", "Out Of Memory!");
        exit(1);
    }

    // send the reply && flush the connection
    if (!dbus_connection_send(conn, reply, &serial))
    {
        LOG_ERROR("DBUSCommunicator", "Out Of Memory!");
        exit(1);
    }
    dbus_connection_flush(conn);

    // free the reply
    dbus_message_unref(reply);
}
