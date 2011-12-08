
#include "DBUSMessageHandler.h"
#include "DBUSConfiguration.h"
#include <stdlib.h>
#include <string.h>
#include "Log.h"

DBUSMessageHandler::DBUSMessageHandler()
: m_MessageIter()
, m_pReply(0)
, m_serial(0)
, m_pConnection(0)
{
    dbus_error_init(&m_err);

    // connect to the bus and check for errors
    char* useSessionBus = getenv("LM_USE_SESSION_BUS");
    if ( NULL != useSessionBus && strcmp(useSessionBus,"enable") == 0 )
    {
        LOG_INFO("DBUSMessageHandler", "Using Session Bus");
        m_pConnection = dbus_bus_get(DBUS_BUS_SESSION, &m_err);
    } 
    else 
    {
        LOG_INFO("DBUSMessageHandler", "Using System Bus");
        m_pConnection = dbus_bus_get(DBUS_BUS_SYSTEM, &m_err);
    }    

    if (dbus_error_is_set(&m_err))
    {
        LOG_ERROR("DBUSMessageHandler","Connection error");
        dbus_error_free(&m_err);
    }
    
    if (NULL == m_pConnection)
    {
        LOG_ERROR("DBUSMessageHandler","Connection is null");
        exit(1);
    }
    
    int ret = dbus_bus_request_name(m_pConnection, DBUS_SERVICE_PREFIX, DBUS_NAME_FLAG_REPLACE_EXISTING, &m_err);
    
    if (dbus_error_is_set(&m_err))
    {
        LOG_ERROR("DBUSMessageHandler", "Name Error "<< m_err.message);
        dbus_error_free(&m_err);
    }
    
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret)
    {
        LOG_ERROR("DBUSMessageHandler", "Not Primary Owner "<< ret);
        exit(1);
    }
}

DBUSMessageHandler::~DBUSMessageHandler()
{
    DBusError err;
    dbus_error_init(&err);
    bool errorset = dbus_error_is_set(&err);
    if (errorset)
    {
        LOG_ERROR("DBUSMessageHandler","there was an dbus error");
    }
    dbus_connection_unregister_object_path(m_pConnection,DBUS_SERVICE_OBJECT_PATH);
    LOG_INFO("DBUSMessageHandler","Ask about owner name");
    dbus_bus_name_has_owner(m_pConnection, DBUS_SERVICE_PREFIX, &err);
    errorset = dbus_error_is_set(&err);
    if (errorset)
    {
        LOG_ERROR("DBUSMessageHandler","there was an dbus error");
    }
    dbus_error_init(&err);
    dbus_bus_release_name(m_pConnection, DBUS_SERVICE_PREFIX, &err);
}

bool DBUSMessageHandler::registerMessageFilter(  DBusHandleMessageFunction fMessageFunc,
                                                 void* comInstance )
{
    bool result = true;
    if ( !dbus_connection_add_filter ( m_pConnection , fMessageFunc, comInstance, NULL) ) 
    {
        result = false;
    }
    return result;
}


bool DBUSMessageHandler::registerPathFunction(  DBusObjectPathMessageFunction fMessageFunc,
                                                DBusObjectPathUnregisterFunction fUnregisterFunc, 
                                                void* comInstance)
{
    bool result = true;
    m_objectPathVTable.unregister_function = fUnregisterFunc;
    m_objectPathVTable.message_function = fMessageFunc;        
    
    if (!dbus_connection_register_object_path(m_pConnection,DBUS_SERVICE_OBJECT_PATH,&m_objectPathVTable, comInstance) )
    {
        result = false;
    }    
    return result;
}

void DBUSMessageHandler::initReceive(DBusMessage* msg)
{
    LOG_DEBUG("DBUSMessageHandler","Message " << dbus_message_get_member(msg) << " was sent by " << dbus_message_get_sender(msg) );
    if (!dbus_message_iter_init(msg, &m_MessageIter))
    {
        LOG_ERROR("DBUSMessageHandler", "Message has no arguments!");
    }
}

void DBUSMessageHandler::initReply(DBusMessage* msg)
{
    // create a reply from the message
    m_pReply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(m_pReply, &m_MessageIter);
}

void DBUSMessageHandler::closeReply()
{
    // send the reply && flush the connection
    if (!dbus_connection_send(m_pConnection, m_pReply, &m_serial))
    {
        LOG_ERROR("DBUSMessageHandler", "Out Of Memory!");
        exit(1);
    }
    LOG_DEBUG("DBUSMessageHandler", "sending reply");
    dbus_connection_flush(m_pConnection);

    // free the reply
    dbus_message_unref(m_pReply);
    m_pReply = NULL;
}

void DBUSMessageHandler::ReplyError(DBusMessage* msg, const char* errorname, const char* errorMsg)
{
    m_pReply = dbus_message_new_error(msg, errorname, errorMsg);
    // send the reply && flush the connection
    if (!dbus_connection_send(m_pConnection, m_pReply, &m_serial))
    {
        LOG_ERROR("DBUSMessageHandler", "Out Of Memory!");
        exit(1);
    }
    LOG_INFO("DBUSMessageHandler", "sending reply with error");
    dbus_connection_flush(m_pConnection);

    // free the reply
    dbus_message_unref(m_pReply);
    m_pReply = NULL;
}

char* DBUSMessageHandler::getString()
{
    char* param;

    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&m_MessageIter))
    {
        LOG_ERROR("DBUSMessageHandler", "Argument is not string!");
    }
    else
    {
        dbus_message_iter_get_basic(&m_MessageIter, &param);
        dbus_message_iter_next(&m_MessageIter);
    }
    return param;
}

dbus_bool_t DBUSMessageHandler::getBool()
{
    dbus_bool_t boolparam;

    if (DBUS_TYPE_BOOLEAN != dbus_message_iter_get_arg_type(&m_MessageIter))
    {
        LOG_ERROR("DBUSMessageHandler", "Argument is not bool!");
    }
    else
    {
        dbus_message_iter_get_basic(&m_MessageIter, &boolparam);
        dbus_message_iter_next(&m_MessageIter);
    }
    return boolparam;
}

char DBUSMessageHandler::getByte()
{
    char param;

    if (DBUS_TYPE_BYTE != dbus_message_iter_get_arg_type(&m_MessageIter))
    {
        LOG_ERROR("DBUSMessageHandler", "Argument is not byte!");
    }
    else
    {
        dbus_message_iter_get_basic(&m_MessageIter, &param);
        dbus_message_iter_next(&m_MessageIter);
    }
    return param;
}

dbus_uint32_t DBUSMessageHandler::getUInt()
{
    dbus_uint32_t param;

    if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&m_MessageIter))
    {
        LOG_ERROR("DBUSMessageHandler", "Argument is not uint32!");
    }
    else
    {
        dbus_message_iter_get_basic(&m_MessageIter, &param);
        dbus_message_iter_next(&m_MessageIter);
    }
    return param;
}

double DBUSMessageHandler::getDouble()
{
    double param;

    if (DBUS_TYPE_DOUBLE != dbus_message_iter_get_arg_type(&m_MessageIter))
    {
        LOG_ERROR("DBUSMessageHandler","Argument is not double!");
    }
    else
    {
        dbus_message_iter_get_basic(&m_MessageIter, &param);
        dbus_message_iter_next(&m_MessageIter);
    }
    return param;
}

void DBUSMessageHandler::getArrayOfUInt(int* pLength, unsigned int** ppArray)
{
    if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&m_MessageIter))
    {
        LOG_ERROR("DBUSMessageHandler","Argument is not an array!");
        return;
    }

    DBusMessageIter arrayIter;
    dbus_message_iter_recurse(&m_MessageIter, &arrayIter);

    uint* localArray;
    dbus_message_iter_get_fixed_array(&arrayIter, &localArray, pLength);

    *ppArray = new uint[*pLength];
    for (int i = 0; i < *pLength; i++)
    {
        (*ppArray)[i] = localArray[i];
    }
}

void DBUSMessageHandler::getArrayOfString(std::vector<std::string>* stringVector)
{
    if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&m_MessageIter))
    {
        LOG_ERROR("DBUSMessageHandler", "Argument is not an array!");
        return;
    }

    DBusMessageIter arrayIter;
    dbus_message_iter_recurse(&m_MessageIter, &arrayIter);
    bool hasNext = true;
    while (hasNext)
    {
        if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&arrayIter))
        {
            LOG_ERROR("DBUSMessageHandler", "Argument is not an string!");
        }
        char* param;
        dbus_message_iter_get_basic(&arrayIter, &param);

        stringVector->push_back(std::string(param));

        if (dbus_message_iter_has_next(&arrayIter))
        {
            dbus_message_iter_next(&arrayIter);
        }
        else
        {
            hasNext = false;
        }
    }
}

void DBUSMessageHandler::appendBool(dbus_bool_t toAppend)
{
    if (!dbus_message_iter_append_basic(&m_MessageIter, DBUS_TYPE_BOOLEAN, &toAppend))
    {
        LOG_ERROR("DBUSMessageHandler", "Out Of Memory!");
        exit(1);
    }
}

void DBUSMessageHandler::appendUInt(dbus_uint32_t toAppend)
{
    if (!dbus_message_iter_append_basic(&m_MessageIter, DBUS_TYPE_UINT32, &toAppend))
    {
        LOG_ERROR("DBUSMessageHandler", "Out Of Memory!");
        exit(1);
    }
}

void DBUSMessageHandler::appendDouble(double toAppend)
{
    if (!dbus_message_iter_append_basic(&m_MessageIter, DBUS_TYPE_DOUBLE, &toAppend))
    {
        LOG_ERROR("DBUSMessageHandler", "Out Of Memory!");
        exit(1);
    }
}

void DBUSMessageHandler::appendByte(char toAppend)
{
    if (!dbus_message_iter_append_basic(&m_MessageIter, DBUS_TYPE_BYTE, &toAppend))
    {
        LOG_ERROR("DBUSMessageHandler", "Out Of Memory!");
        exit(1);
    }
}

void DBUSMessageHandler::appendArrayOfUInt(unsigned int length, unsigned int *IDs)
{
    DBusMessageIter arrayIter;
    dbus_message_iter_open_container(&m_MessageIter, DBUS_TYPE_ARRAY, "u", &arrayIter);
    for(unsigned int i = 0; i < length; i++)
    {
        dbus_message_iter_append_basic(&arrayIter, DBUS_TYPE_UINT32, &IDs[i]);
    }
    dbus_message_iter_close_container(&m_MessageIter, &arrayIter);
}
