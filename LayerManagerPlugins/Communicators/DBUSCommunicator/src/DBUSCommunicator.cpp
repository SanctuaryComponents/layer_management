/***************************************************************************
 *
 * Copyright 2010 BMW Car IT GmbH
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/
#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "DBUSCommunicator.h"
#include "Log.h"

typedef void (DBUSCommunicator::*CallBackMethod) (DBusConnection *connection, DBusMessage *message);

typedef struct {
  const char *name;
  const char *signature;
  const char *reply;
  CallBackMethod function;
} MethodTable;

static MethodTable manager_methods[] = {
    { "Debug",     		"b",  	"", 	&DBUSCommunicator::Debug },
    { "ScreenShot",		"s",  	"", 	&DBUSCommunicator::ScreenShot },
    { "ListAllLayerIDS", "",    "au",   &DBUSCommunicator::ListAllLayerIDS},
    { "ListAllSurfaceIDS", "",    "au",   &DBUSCommunicator::ListAllSurfaceIDS},
    { "ListAllLayerGroupIDS", "",    "au",   &DBUSCommunicator::ListAllLayerGroupIDS},
    { "ListAllSurfaceGroupIDS", "",    "au",   &DBUSCommunicator::ListAllSurfaceGroupIDS},
    { "ListSurfacesOfSurfacegroup", "u",    "au",   &DBUSCommunicator::ListSurfacesOfSurfacegroup},
    { "ListLayersOfLayergroup", "u",    "au",   &DBUSCommunicator::ListLayersOfLayergroup},
    { "ListSurfaceofLayer", "u",    "au",   &DBUSCommunicator::ListSurfaceofLayer},
    { "getPropertiesOfSurface", "u",    "duuuuuuuuyb",   &DBUSCommunicator::getPropertiesOfSurface},
    { "getPropertiesOfLayer", "u",    "duuuuuuuuyb",  &DBUSCommunicator::getPropertiesOfLayer },
    { "CreateSurface", "uuuu",    "u",   &DBUSCommunicator::CreateSurface},
    { "CreateSurfaceFromId", "uuuuu",    "u",   &DBUSCommunicator::CreateSurfaceFromId},
    { "RemoveSurface", "u",    "",  &DBUSCommunicator::RemoveSurface },
    { "CreateLayer", "",    "u",  &DBUSCommunicator::CreateLayer },
    { "CreateLayerFromId", "u", "u",   &DBUSCommunicator::CreateLayerFromId},
    { "RemoveLayer", "u",    "",  &DBUSCommunicator::RemoveLayer },
    { "AddSurfaceToSurfaceGroup", "uu",    "",  &DBUSCommunicator::AddSurfaceToSurfaceGroup },
    { "RemoveSurfaceFromSurfaceGroup", "uu",    "",  &DBUSCommunicator::RemoveSurfaceFromSurfaceGroup },
    { "AddLayerToLayerGroup", "uu",    "",   &DBUSCommunicator::AddLayerToLayerGroup},
    { "RemoveLayerFromLayerGroup",  "uu", "", &DBUSCommunicator::RemoveLayerFromLayerGroup },
    { "AddSurfaceToLayer", "uu",    "", &DBUSCommunicator::AddSurfaceToLayer  },
    { "RemoveSurfaceFromLayer", "uu",    "",   &DBUSCommunicator::RemoveSurfaceFromLayer},
    { "CreateSurfaceGroup", "",    "u",  &DBUSCommunicator::CreateSurfaceGroup },
    { "CreateSurfaceGroupFromId", "u",    "u",  &DBUSCommunicator::CreateSurfaceGroup },
    { "RemoveSurfaceGroup", "u",    "",   &DBUSCommunicator::RemoveSurfaceGroup},
    { "CreateLayerGroup", "",    "u",  &DBUSCommunicator::CreateLayerGroup },
    { "CreateLayerGroupFromId", "u",    "u",  &DBUSCommunicator::CreateLayerGroup },
    { "RemoveLayerGroup", "u",    "",  &DBUSCommunicator::RemoveLayerGroup },
    { "SetSurfaceSourceRegion", "uuuuu",    "",  &DBUSCommunicator::SetSurfaceSourceRegion },
    { "SetLayerSourceRegion", "uuuuu",    "",   &DBUSCommunicator::SetLayerSourceRegion},
    { "SetSurfaceDestinationRegion", "uuuuu",    "",   &DBUSCommunicator::SetSurfaceDestinationRegion},
    { "SetSurfacePosition", "uuu",    "",  &DBUSCommunicator::SetSurfacePosition },
    { "GetSurfacePosition", "u",    "uu",  &DBUSCommunicator::GetSurfacePosition },
    { "SetSurfaceDimension", "uuu",    "",   &DBUSCommunicator::SetSurfaceDimension},
    { "SetLayerDestinationRegion", "uuuuu",    "",   &DBUSCommunicator::SetLayerDestinationRegion},
    { "SetLayerPosition", "uuu",    "",   &DBUSCommunicator::SetLayerPosition},
    { "GetLayerPosition", "u",    "uu",   &DBUSCommunicator::GetLayerPosition},
    { "SetLayerDimension", "uuu",    "",   &DBUSCommunicator::SetLayerDimension},
    { "GetLayerDimension", "u",    "uu",   &DBUSCommunicator::GetLayerDimension},
    { "GetSurfaceDimension", "u",    "uu",   &DBUSCommunicator::GetSurfaceDimension},
    { "SetSurfaceOpacity", "ud",    "",   &DBUSCommunicator::SetSurfaceOpacity},
    { "SetLayerOpacity", "ud",    "",  &DBUSCommunicator::SetLayerOpacity },
    { "SetSurfacegroupOpacity", "ud",    "",  &DBUSCommunicator::SetSurfacegroupOpacity },
    { "SetLayergroupOpacity", "ud",    "",  &DBUSCommunicator::SetLayergroupOpacity },
    { "GetSurfaceOpacity", "u",    "d",  &DBUSCommunicator::GetSurfaceOpacity },
    { "GetLayerOpacity", "u",    "d", &DBUSCommunicator::GetLayerOpacity  },
    { "GetSurfacegroupOpacity", "u",    "d",  &DBUSCommunicator::GetSurfacegroupOpacity },
    { "GetLayergroupOpacity", "u",    "d",  &DBUSCommunicator::GetLayergroupOpacity },
    { "SetSurfaceOrientation", "uu",    "",  &DBUSCommunicator::SetSurfaceOrientation },
    { "SetLayerOrientation", "uu",    "",  &DBUSCommunicator::SetLayerOrientation },
    { "GetSurfacePixelformat", "u",    "u",  &DBUSCommunicator::GetSurfacePixelformat },
    { "SetSurfaceVisibility", "ub",    "",  &DBUSCommunicator::SetSurfaceVisibility },
    { "SetLayerVisibility", "ub",    "",  &DBUSCommunicator::SetLayerVisibility },
    { "GetSurfaceVisibility", "u",    "b", &DBUSCommunicator::GetSurfaceVisibility  },
    { "GetLayerVisibility", "u",    "b",   &DBUSCommunicator::GetLayerVisibility },
    { "SetSurfacegroupVisibility", "ub",    "",  &DBUSCommunicator::SetSurfacegroupVisibility },
    { "SetLayergroupVisibility", "ub",    "",  &DBUSCommunicator::SetLayergroupVisibility },
    { "SetRenderOrderOfLayers", "auu",    "",  &DBUSCommunicator::SetRenderOrderOfLayers },
    { "SetSurfaceRenderOrderWithinLayer", "uau",    "",  &DBUSCommunicator::SetSurfaceRenderOrderWithinLayer },
    { "GetLayerType", "u",    "u",&DBUSCommunicator::GetLayerType   },
    { "GetLayertypeCapabilities", "u",    "u",  &DBUSCommunicator::GetLayertypeCapabilities },
    { "GetLayerCapabilities", "u",    "u",  &DBUSCommunicator::GetLayerCapabilities },
    { "Exit", "",    "",  &DBUSCommunicator::Exit },
    { "CommitChanges", "",    "", &DBUSCommunicator::CommitChanges  },
    { "CreateShader", "ss",    "u",  &DBUSCommunicator::CreateShader },
    { "DestroyShader", "u",    "",   &DBUSCommunicator::DestroyShader},
    { "SetShader", "uu",    "",  &DBUSCommunicator::SetShader },
    { "SetUniforms", "uas",    "",  &DBUSCommunicator::SetUniforms },
    { "","","",NULL}
};

#include <string>

DBusMessageIter iter;
DBusMessage* currentMsg;
DBusMessage* reply;
dbus_uint32_t serial = 0;
DBusConnection* conn;

char* getStringFromMessage(){
  char* param;

  if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&iter)){
    LOG_ERROR("DBUSCommunicator", "Argument is not string!");
  }else{
    dbus_message_iter_get_basic(&iter, &param);
  }
  return param;
}

bool getBoolFromMessage(){
  dbus_bool_t boolparam;

  if (DBUS_TYPE_BOOLEAN != dbus_message_iter_get_arg_type(&iter)){
    LOG_ERROR("DBUSCommunicator", "Argument is not bool!");
  }else{
    dbus_message_iter_get_basic(&iter, &boolparam);
  }
  bool b;
  if (boolparam==0)
    b = false;
  else
    b = true;
  return b;
}

char getByteFromMessage(){
  char param;

  if (DBUS_TYPE_BYTE != dbus_message_iter_get_arg_type(&iter)){
    LOG_ERROR("DBUSCommunicator", "Argument is not byte!");
  }else{
    dbus_message_iter_get_basic(&iter, &param);
  }
  return param;
}

uint getUIntFromMessage(){
  uint param;

  if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&iter)){
    LOG_ERROR("DBUSCommunicator", "Argument is not uint32!");
  }else{
    dbus_message_iter_get_basic(&iter, &param);
  }
  return param;
}



double getDoubleFromMessage(){
  double param;

  if (DBUS_TYPE_DOUBLE != dbus_message_iter_get_arg_type(&iter)){
    LOG_ERROR("DBUSCommunicator","Argument is not double!");
  }else{
    dbus_message_iter_get_basic(&iter, &param);
  }
  return param;
}


void appendBool(bool toAppend){
  if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_BOOLEAN, &toAppend)) {
    LOG_ERROR("DBUSCommunicator", "Out Of Memory!");
    exit(1);
  }
}

void appendUint(uint toAppend){
  if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &toAppend)) {
    LOG_ERROR("DBUSCommunicator", "Out Of Memory!");
    exit(1);
  }
}

void appendDouble(double toAppend){
  if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &toAppend)) {
    LOG_ERROR("DBUSCommunicator", "Out Of Memory!");
    exit(1);
  }
}

void appendbyte(char toAppend){
  if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_BYTE, &toAppend)) {
    LOG_ERROR("DBUSCommunicator", "Out Of Memory!");
    exit(1);
  }
}

void initReceive(DBusMessage* msg){
  currentMsg = msg;
  if (!dbus_message_iter_init(currentMsg, &iter))
    LOG_ERROR("DBUSCommunicator", "Message has no arguments!");
}

void initReply(){
  // create a reply from the message
  reply = dbus_message_new_method_return(currentMsg);
  dbus_message_iter_init_append(reply, &iter);
}

void closeReply(){
  // send the reply && flush the connection
  if (!dbus_connection_send(conn, reply, &serial)) {
    LOG_ERROR("DBUSCommunicator", "Out Of Memory!");
    exit(1);
  }
  dbus_connection_flush(conn);

  // free the reply
  dbus_message_unref(reply);
}



void addIntrospectHeader(std::string& str){
  str.append("<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \n \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\"> \n");
}

void addIntrospectNode(std::string& str, std::string nodename){
  str.append("<node name=\"" + nodename  + "\">  \n");
}

void addIntrospectinterface(std::string& str, std::string interfacename){
  str.append("<interface name=\"" + interfacename + "\">  \n");
}

void addIntrospectMethod(std::string& str, std::string methodname){
  str.append("<method name=\"" + methodname + "\">  \n");
}

void addIntrospectArg(std::string& str, std::string argname,std::string direction,std::string type){
  str.append("<arg name=\"" + argname + "\" direction=\"" + direction + "\" type=\"" + type + "\"/>  \n");
}

void closeIntrospectMethod(std::string& str){
  str.append("</method>  \n");
}

void closeIntrospectInterface(std::string& str){
  str.append("</interface>  \n");
}

void closeIntrospectNode(std::string& str){
  str.append("</node>  \n");
}

std::string generateIntrospectionString(MethodTable* table){
  LOG_DEBUG("DBUSCommunicator", "Generation introspection data");
  std::string introspect;
  addIntrospectHeader(introspect);
  addIntrospectNode(introspect, "de.bmw.CompositingService");
  addIntrospectinterface(introspect, "org.freedesktop.DBus.Introspectable");
  addIntrospectMethod(introspect,"Introspect");
  addIntrospectArg(introspect,"data","out","s");
  closeIntrospectMethod(introspect);
  closeIntrospectInterface(introspect);

  addIntrospectinterface(introspect, "de.bmw.CompositingService");

  int i=0;
  while(strcmp(manager_methods[i].name,"")!=0){
    MethodTable entry = table[i];
    addIntrospectMethod(introspect,std::string(entry.name));
    std::string signatureIN = std::string(entry.signature);
    for (uint c=0;c<signatureIN.length();c++){
      switch(signatureIN.at(c)){
      case 'a': addIntrospectArg(introspect,"","in","a" + signatureIN.at(c+1));c++; break;
      default: addIntrospectArg(introspect,"","in","i"); break;
      }
    }

    std::string reply = std::string(entry.reply);
    for (uint c=0;c<reply.length();c++){
      switch(reply.at(c)){
      case 'a':addIntrospectArg(introspect,"","out","a" + reply.at(c+1)); c++; break;
      default:addIntrospectArg(introspect,"","out","i"); break;
      }
    }

    closeIntrospectMethod(introspect);
    i++;
  }

  closeIntrospectInterface(introspect);

  closeIntrospectNode(introspect);
  LOG_DEBUG("DBUSCommunicator", "returning introspection data");
  return introspect;
}

DBUSCommunicator* DBUSCommunicator::m_reference = NULL;

DBUSCommunicator::DBUSCommunicator(CommandExecutor* executor, ILayerList* ll) : BaseCommunicator(executor,ll){
}

DBUSCommunicator::~DBUSCommunicator(){

}

void DBUSCommunicator::setdebug(bool onoff){

}

bool DBUSCommunicator::start(){
  LOG_INFO("DBUSCommunicator", "Starting up dbus connector");
  DBusError err;
  int ret;
  // initialise the error
  dbus_error_init(&err);

  // connect to the bus and check for errors
  LOG_INFO("DBUSCommunicator","get DBUS Session");
  conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if (dbus_error_is_set(&err)) {
    LOG_ERROR("DBUSCommunicator","Connection error");
    dbus_error_free(&err);
  }
  if (NULL == conn) {
    LOG_ERROR("DBUSCommunicator","Connection is null");
    exit(1);
  }
  LOG_INFO("DBUSCommunicator","request dbus name");
  ret = dbus_bus_request_name(conn, "de.bmw.CompositingService", DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
  if (dbus_error_is_set(&err)) {
    LOG_ERROR("DBUSCommunicator", "Name Error "<< err.message);
    dbus_error_free(&err);
  }
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
    LOG_ERROR("DBUSCommunicator", "Not Primary Owner "<< ret);
    exit(1);
  }
  LOG_INFO("DBUSCommunicator","create thread");
  this->m_running = true;
  pthread_create(&m_currentThread, NULL, DBUSCommunicator::run, this);
  LOG_INFO("DBUSCommunicator", "Started dbus connector");
  return true;
}

void DBUSCommunicator::stop(){
  LOG_INFO("DBUSCommunicator","stopping");
  this->m_running = false;
  pthread_join(m_currentThread,NULL);
  DBusError err;
  dbus_error_init(&err);
  bool errorset = dbus_error_is_set(&err);
  if (errorset)
    LOG_ERROR("DBUSCommunicator","there was an dbus error");
  LOG_INFO("ask about owner name",0);
  dbus_bus_name_has_owner(conn,"de.bmw.CompositingService",&err);
  errorset = dbus_error_is_set(&err);
  if (errorset)
    LOG_ERROR("DBUSCommunicator","there was an dbus error");
  dbus_error_init(&err);
  dbus_bus_release_name(conn, "de.bmw.CompositingService",&err);
  LOG_INFO("DBUSCommunicator", "Stopped dbus connector");
}

void introspectCallback(DBusConnection* conn,DBusMessage* msg )
{
  DBusMessage* reply;
  DBusMessageIter args;
  dbus_uint32_t serial = 0;

  // create a reply from the message
  reply = dbus_message_new_method_return(msg);

  std::string introspect = generateIntrospectionString(manager_methods);


  const char* string = introspect.c_str();
  // add the arguments to the reply
  dbus_message_iter_init_append(reply, &args);
  if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &string)) {
    LOG_ERROR("DBUSCommunicator", "Out Of Memory!");
    exit(1);
  }

  // send the reply && flush the connection
  if (!dbus_connection_send(conn, reply, &serial)) {
    LOG_ERROR("DBUSCommunicator", "Out Of Memory!");
    exit(1);
  }
  dbus_connection_flush(conn);

  // free the reply
  dbus_message_unref(reply);
}

void DBUSCommunicator::Debug(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  bool param = getBoolFromMessage();

  m_reference->m_executor->execute(new DebugCommand(param));

  initReply();
  closeReply();
}

void DBUSCommunicator::ScreenShot(DBusConnection* conn,DBusMessage* msg )
{
	initReceive(msg);
	char* filename = getStringFromMessage();

	m_reference->m_executor->execute(new ScreenShotCommand(filename));

	initReply();
	closeReply();
}

void DBUSCommunicator::ListAllLayerIDS(DBusConnection* conn,DBusMessage* msg )
{
  currentMsg = msg;
  int* array = NULL;
  int length = 0;
  m_reference->m_layerlist->lockList();
  m_reference->m_layerlist->getLayerIDs(&length,&array);
  initReply();
  DBusMessageIter arrayIter;
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "u", &arrayIter);
  for ( int i = 0; i< length;i++)
    dbus_message_iter_append_basic(&arrayIter,DBUS_TYPE_UINT32,&array[i]);
  dbus_message_iter_close_container(&iter,&arrayIter);
  closeReply();
  m_reference->m_layerlist->unlockList();
}

void DBUSCommunicator::ListAllSurfaceIDS(DBusConnection* conn,DBusMessage* msg )
{
  currentMsg = msg;
  int* array = NULL;
  int length = 0;
  m_reference->m_layerlist->lockList();
  m_reference->m_layerlist->getSurfaceIDs(&length,&array);
  initReply();
  DBusMessageIter arrayIter;
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "u", &arrayIter);
  for ( int i = 0; i< length;i++)
    dbus_message_iter_append_basic(&arrayIter,DBUS_TYPE_UINT32,&array[i]);
  dbus_message_iter_close_container(&iter,&arrayIter);
  closeReply();
  m_reference->m_layerlist->unlockList();
}

void DBUSCommunicator::ListAllLayerGroupIDS(DBusConnection* conn,DBusMessage* msg )
{
  currentMsg = msg;
  int* array = NULL;
  int length = 0;
  m_reference->m_layerlist->lockList();
  m_reference->m_layerlist->getLayerGroupIDs(&length,&array);
  initReply();
  DBusMessageIter arrayIter;
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "u", &arrayIter);
  for ( int i = 0; i< length;i++)
    dbus_message_iter_append_basic(&arrayIter,DBUS_TYPE_UINT32,&array[i]);
  dbus_message_iter_close_container(&iter,&arrayIter);
  closeReply();
  m_reference->m_layerlist->unlockList();
}

void DBUSCommunicator::ListAllSurfaceGroupIDS(DBusConnection* conn,DBusMessage* msg )
{
  currentMsg = msg;
  int* array = NULL;
  int length = 0;
  m_reference->m_layerlist->lockList();
  m_reference->m_layerlist->getSurfaceGroupIDs(&length,&array);
  initReply();
  DBusMessageIter arrayIter;
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "u", &arrayIter);
  for ( int i = 0; i< length;i++)
    dbus_message_iter_append_basic(&arrayIter,DBUS_TYPE_UINT32,&array[i]);
  dbus_message_iter_close_container(&iter,&arrayIter);
  closeReply();
  m_reference->m_layerlist->unlockList();
}

void DBUSCommunicator::ListSurfacesOfSurfacegroup(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  m_reference->m_layerlist->lockList();
  SurfaceGroup* sg = m_reference->m_layerlist->getSurfaceGroup(id);
  std::list<Surface*> surfaces = sg->getList();
  initReply();
  DBusMessageIter arrayIter;
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "u", &arrayIter);
  for ( std::list<Surface*>::const_iterator it=surfaces.begin();it!=surfaces.end();it++){
    Surface* s = *it;
    int id = s->getID();
    dbus_message_iter_append_basic(&arrayIter,DBUS_TYPE_UINT32,&id);
  }
  dbus_message_iter_close_container(&iter,&arrayIter);
  closeReply();
  m_reference->m_layerlist->unlockList();
}

void DBUSCommunicator::ListLayersOfLayergroup(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  m_reference->m_layerlist->lockList();
  LayerGroup* sg = m_reference->m_layerlist->getLayerGroup(id);
  std::list<Layer*> layers = sg->getList();
  initReply();
  DBusMessageIter arrayIter;
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "u", &arrayIter);
  for ( std::list<Layer*>::const_iterator it=layers.begin();it!=layers.end();it++){
    Layer* l = *it;
    int id = l->getID();
    dbus_message_iter_append_basic(&arrayIter,DBUS_TYPE_UINT32,&id);
  }
  dbus_message_iter_close_container(&iter,&arrayIter);
  closeReply();
  m_reference->m_layerlist->unlockList();
}

void DBUSCommunicator::ListSurfaceofLayer(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  m_reference->m_layerlist->lockList();
  Layer* layer = m_reference->m_layerlist->getLayer(id);
  std::list<Surface*> surfaces = layer->getAllSurfaces();
  initReply();
  DBusMessageIter arrayIter;
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "u", &arrayIter);
  for ( std::list<Surface*>::const_iterator it=surfaces.begin();it!=surfaces.end();it++){
    Surface* s = *it;
    int id = s->getID();
    dbus_message_iter_append_basic(&arrayIter,DBUS_TYPE_UINT32,&id);
  }
  dbus_message_iter_close_container(&iter,&arrayIter);
  closeReply();
  m_reference->m_layerlist->unlockList();
}

void DBUSCommunicator::getPropertiesOfSurface(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();

  Surface* surface = m_reference->m_layerlist->getSurface(id);

  initReply();
  appendDouble(surface->getOpacity());
  Rectangle dest = surface->getDestinationRegion();
  Rectangle src = surface->getSourceRegion();
  appendUint(src.x);
  appendUint(src.y);
  appendUint(src.width);
  appendUint(src.height);
  appendUint(dest.x);
  appendUint(dest.y);
  appendUint(dest.width);
  appendUint(dest.height);
  OrientationType orientation = surface->getOrientation();
  appendbyte((char)orientation);
  appendBool(surface->getVisibility());
  closeReply();
}

void DBUSCommunicator::getPropertiesOfLayer(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();

  Layer* layer = m_reference->m_layerlist->getLayer(id);

  initReply();
  appendDouble(layer->getOpacity());
  Rectangle dest = layer->getDestinationRegion();
  Rectangle src = layer->getSourceRegion();
  appendUint(src.x);
  appendUint(src.y);
  appendUint(src.width);
  appendUint(src.height);
  appendUint(dest.x);
  appendUint(dest.y);
  appendUint(dest.width);
  appendUint(dest.height);
  OrientationType orientation = layer->getOrientation();
  appendbyte((char)orientation);
  appendBool(layer->getVisibility());
  closeReply();
}

void DBUSCommunicator::CreateSurface(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint handle = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint pixelformat = getUIntFromMessage();
  PixelFormat pf = (PixelFormat)pixelformat;

  dbus_message_iter_next(&iter);
  uint width = getUIntFromMessage();

  dbus_message_iter_next(&iter);
  uint height = getUIntFromMessage();
  int id = -1;
  m_reference->m_executor->execute(new CreateCommand(handle,TypeSurface,pf, width, height,&id));
  initReply();
  appendUint(id);
  closeReply();
}

void DBUSCommunicator::CreateSurfaceFromId(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  int id = 0;

  uint handle = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint pixelformat = getUIntFromMessage();
  PixelFormat pf = (PixelFormat)pixelformat;

  dbus_message_iter_next(&iter);
  uint width = getUIntFromMessage();

  dbus_message_iter_next(&iter);
  uint height = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  id = (int) getUIntFromMessage();

  m_reference->m_executor->execute(new CreateCommand(handle,TypeSurface,pf, width, height,&id));
  initReply();
  appendUint(id);
  closeReply();
}



void DBUSCommunicator::RemoveSurface(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint param = getUIntFromMessage();
  m_reference->m_executor->execute(new RemoveCommand(param,TypeSurface));
  initReply();
  closeReply();
}

void DBUSCommunicator::CreateLayer(DBusConnection* conn,DBusMessage* msg )
{
  currentMsg = msg;
  int id = -1;
  m_reference->m_executor->execute(new CreateCommand(-1,TypeLayer,PIXELFORMAT_R8,0,0,&id));
  initReply();
  appendUint(id);
  closeReply();
}

void DBUSCommunicator::CreateLayerFromId(DBusConnection* conn,DBusMessage* msg )
{
  currentMsg = msg;
  int id = -1;
  initReceive(msg);
  id = (int)getUIntFromMessage();
  m_reference->m_executor->execute(new CreateCommand(-1,TypeLayer,PIXELFORMAT_R8,0,0,&id));
  initReply();
  appendUint(id);
  closeReply();
}


void DBUSCommunicator::RemoveLayer(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint param = getUIntFromMessage();
  m_reference->m_executor->execute(new RemoveCommand(param,TypeLayer));
  initReply();
  closeReply();
}

void DBUSCommunicator::AddSurfaceToSurfaceGroup(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint surfaceid = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint surfacegroupid = getUIntFromMessage();
  m_reference->m_executor->execute(new SurfacegroupAddSurfaceCommand(surfacegroupid,surfaceid));
  initReply();
  closeReply();
}

void DBUSCommunicator::RemoveSurfaceFromSurfaceGroup(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint surfaceid = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint surfacegroupid = getUIntFromMessage();
  m_reference->m_executor->execute(new SurfacegroupRemoveSurfaceCommand(surfacegroupid,surfaceid));
  initReply();
  closeReply();
}

void DBUSCommunicator::AddLayerToLayerGroup(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint layerid = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint layergroupid = getUIntFromMessage();
  m_reference->m_executor->execute(new LayergroupAddLayerCommand(layergroupid,layerid));
  initReply();
  closeReply();
}

void DBUSCommunicator::RemoveLayerFromLayerGroup(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint layerid = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint layergroupid = getUIntFromMessage();
  m_reference->m_executor->execute(new LayergroupRemoveLayerCommand(layergroupid,layerid));
  initReply();
  closeReply();
}

void DBUSCommunicator::AddSurfaceToLayer(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint surfaceid = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint layer = getUIntFromMessage();
  m_reference->m_executor->execute(new LayerAddSurfaceCommand(layer,surfaceid));
  initReply();
  closeReply();
}

void DBUSCommunicator::RemoveSurfaceFromLayer(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint surfaceid = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint layerid = getUIntFromMessage();
  m_reference->m_executor->execute(new LayerRemoveSurfaceCommand(layerid,surfaceid));
  initReply();
  closeReply();
}

void DBUSCommunicator::CreateSurfaceGroup(DBusConnection* conn,DBusMessage* msg )
{
  currentMsg = msg;
  int newID = -1;
  m_reference->m_executor->execute(new CreateCommand(-1,TypeSurfaceGroup,PIXELFORMAT_R8,0,0,&newID));
  initReply();
  appendUint(newID);
  closeReply();
}

void DBUSCommunicator::CreateSurfaceGroupFromId(DBusConnection* conn,DBusMessage* msg )
{
  currentMsg = msg;
  int newID = -1;
  initReceive(msg);
  newID = (int)getUIntFromMessage();
  m_reference->m_executor->execute(new CreateCommand(-1,TypeSurfaceGroup,PIXELFORMAT_R8,0,0,&newID));
  initReply();
  appendUint(newID);
  closeReply();
}



void DBUSCommunicator::RemoveSurfaceGroup(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint param = getUIntFromMessage();
  m_reference->m_executor->execute(new RemoveCommand(param,TypeSurfaceGroup));
  initReply();
  closeReply();
}

void DBUSCommunicator::CreateLayerGroup(DBusConnection* conn,DBusMessage* msg )
{
  currentMsg = msg;
  int newID = -1;
  m_reference->m_executor->execute(new CreateCommand(-1,TypeLayerGroup,PIXELFORMAT_R8,0,0,&newID));
  initReply();
  appendUint(newID);
  closeReply();
}

void DBUSCommunicator::CreateLayerGroupFromId(DBusConnection* conn,DBusMessage* msg )
{
  currentMsg = msg;
  int newID = 0;
  initReceive(msg);
  newID = (int)getUIntFromMessage();
  m_reference->m_executor->execute(new CreateCommand(-1,TypeLayerGroup,PIXELFORMAT_R8,0,0,&newID));
  initReply();
  appendUint(newID);
  closeReply();
}


void DBUSCommunicator::RemoveLayerGroup(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint param = getUIntFromMessage();
  m_reference->m_executor->execute(new RemoveCommand(param,TypeLayerGroup));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetSurfaceSourceRegion(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint x = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint y = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint width = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint height = getUIntFromMessage();
  m_reference->m_executor->execute(new SetSourceRectangleCommand(id,TypeSurface,x,y,width,height));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetLayerSourceRegion(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint x = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint y = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint width = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint height = getUIntFromMessage();
  m_reference->m_executor->execute(new SetSourceRectangleCommand(id,TypeLayer,x,y,width,height));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetSurfaceDestinationRegion(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint x = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint y = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint width = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint height = getUIntFromMessage();
  m_reference->m_executor->execute(new SetDestinationRectangleCommand(id,TypeSurface,x,y,width,height));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetSurfacePosition(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint x = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint y = getUIntFromMessage();
  m_reference->m_executor->execute(new SetPositionCommand(id,TypeSurface,x,y));
  initReply();
  closeReply();
}

void DBUSCommunicator::GetSurfacePosition(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  uint x;
  uint y;
  m_reference->m_executor->execute(new GetPositionCommand(id,TypeSurface,&x,&y));
  initReply();
  appendUint(x);
  appendUint(y);
  closeReply();
}

void DBUSCommunicator::SetSurfaceDimension(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint width = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint height = getUIntFromMessage();
  m_reference->m_executor->execute(new SetDimensionCommand(id,TypeSurface,width,height));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetLayerDestinationRegion(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint x = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint y = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint width = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint height = getUIntFromMessage();
  m_reference->m_executor->execute(new SetDestinationRectangleCommand(id,TypeLayer,x,y,width,height));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetLayerPosition(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint x = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint y = getUIntFromMessage();
  m_reference->m_executor->execute(new SetPositionCommand(id,TypeLayer,x,y));
  initReply();
  closeReply();
}

void DBUSCommunicator::GetLayerPosition(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  uint x;
  uint y;
  m_reference->m_executor->execute(new GetPositionCommand(id,TypeLayer,&x,&y));
  initReply();
  appendUint(x);
  appendUint(y);
  closeReply();
}

void DBUSCommunicator::SetLayerDimension(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint width = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint height = getUIntFromMessage();
  m_reference->m_executor->execute(new SetDimensionCommand(id,TypeLayer,width,height));
  initReply();
  closeReply();
}

void DBUSCommunicator::GetLayerDimension(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  uint width;
  uint height;
  m_reference->m_executor->execute(new GetDimensionCommand(id,TypeLayer,&width,&height));
  initReply();
  appendUint(width);
  appendUint(height);
  closeReply();
}

void DBUSCommunicator::GetSurfaceDimension(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  uint width;
  uint height;
  m_reference->m_executor->execute(new GetDimensionCommand(id,TypeSurface,&width,&height));
  initReply();
  appendUint(width);
  appendUint(height);
  closeReply();
}

void DBUSCommunicator::SetSurfaceOpacity(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  double param = getDoubleFromMessage();
  m_reference->m_executor->execute(new SetOpacityCommand(id,TypeSurface,param));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetLayerOpacity(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  double param = getDoubleFromMessage();
  m_reference->m_executor->execute(new SetOpacityCommand(id,TypeLayer,param));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetSurfacegroupOpacity(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  double param = getDoubleFromMessage();
  m_reference->m_executor->execute(new SetOpacityCommand(id,TypeSurfaceGroup,param));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetLayergroupOpacity(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  double param = getDoubleFromMessage();
  m_reference->m_executor->execute(new SetOpacityCommand(id,TypeLayerGroup,param));
  initReply();
  closeReply();
}

void DBUSCommunicator::GetSurfaceOpacity(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  double param;
  m_reference->m_executor->execute(new GetOpacityCommand(id,TypeSurface,&param));
  initReply();
  appendDouble(param);
  closeReply();
}

void DBUSCommunicator::GetLayerOpacity(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  double param;
  m_reference->m_executor->execute(new GetOpacityCommand(id,TypeLayer,&param));
  initReply();
  appendDouble(param);
  closeReply();
}

void DBUSCommunicator::GetSurfacegroupOpacity(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  double param;
  m_reference->m_executor->execute(new GetOpacityCommand(id,TypeSurfaceGroup,&param));
  initReply();
  appendDouble(param);
  closeReply();
}

void DBUSCommunicator::GetLayergroupOpacity(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  double param;
  m_reference->m_executor->execute(new GetOpacityCommand(id,TypeLayerGroup,&param));
  initReply();
  appendDouble(param);
  closeReply();
}

void DBUSCommunicator::SetSurfaceOrientation(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint param = getUIntFromMessage();
  OrientationType o = (OrientationType) param;
  m_reference->m_executor->execute(new SetOrientationCommand(id,TypeSurface,o));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetLayerOrientation(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint param = getUIntFromMessage();
  OrientationType o = (OrientationType) param;
  m_reference->m_executor->execute(new SetOrientationCommand(id,TypeLayer,o));
  initReply();
  closeReply();
}

void DBUSCommunicator::GetSurfacePixelformat(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  PixelFormat param;
  m_reference->m_executor->execute(new GetPixelformatCommand(id,TypeSurface,&param));
  initReply();
  appendbyte(param);
  closeReply();
}

void DBUSCommunicator::SetSurfaceVisibility(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  const uint surfaceid = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  bool newVis= getBoolFromMessage();
  m_reference->m_executor->execute(new SetVisibilityCommand(surfaceid,TypeSurface,newVis));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetLayerVisibility(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint layerid = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  bool myparam = getBoolFromMessage();
  m_reference->m_executor->execute(new SetVisibilityCommand(layerid,TypeLayer,myparam));
  initReply();
  closeReply();
}

void DBUSCommunicator::GetSurfaceVisibility(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  bool param;
  m_reference->m_executor->execute(new GetVisibilityCommand(id,TypeSurface,&param));
  initReply();
  appendBool(param);
  closeReply();
}

void DBUSCommunicator::GetLayerVisibility(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  bool param;
  m_reference->m_executor->execute(new GetVisibilityCommand(id,TypeLayer,&param));
  initReply();
  appendBool(param);
  closeReply();
}

void DBUSCommunicator::SetSurfacegroupVisibility(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint groupid = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  bool myparam = getBoolFromMessage();
  m_reference->m_executor->execute(new SetVisibilityCommand(groupid,TypeSurfaceGroup,myparam));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetLayergroupVisibility(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint groupid = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  bool myparam = getBoolFromMessage();
  m_reference->m_executor->execute(new SetVisibilityCommand(groupid,TypeLayerGroup,myparam));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetRenderOrderOfLayers(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&iter))
    LOG_ERROR("DBUSCommunicator","Argument is not an array!");
  DBusMessageIter arrayIter;
  dbus_message_iter_recurse(&iter,&arrayIter);
  int* arr;
  int length;
  dbus_message_iter_get_fixed_array(&arrayIter,&arr,&length);

  m_reference->m_executor->execute(new SetLayerOrderCommand(arr,length));

  initReply();
  closeReply();
}

void DBUSCommunicator::SetSurfaceRenderOrderWithinLayer(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint layerid = getUIntFromMessage();
  dbus_message_iter_next(&iter);

  if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&iter))
    LOG_ERROR("DBUSCommunicator", "Argument is not an array!");
  DBusMessageIter arrayIter;
  dbus_message_iter_recurse(&iter,&arrayIter);
  int* arr;
  int length;
  dbus_message_iter_get_fixed_array(&arrayIter,&arr,&length);

  m_reference->m_executor->execute(new SetOrderWithinLayerCommand(layerid,arr,length));

  initReply();
  closeReply();
}

void DBUSCommunicator::GetLayerType(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  Layer* l = m_reference->m_layerlist->getLayer(id);
  initReply();
  appendUint(l->getLayerType());
  closeReply();
}

void DBUSCommunicator::GetLayertypeCapabilities(DBusConnection* conn,DBusMessage* msg )
{

  initReceive(msg);
  uint id = getUIntFromMessage();
  LayerType type = (LayerType)id;
  uint capabilities = m_reference->m_executor->getLayerTypeCapabilities(type);
  initReply();
  appendUint(capabilities);
  closeReply();
}

void DBUSCommunicator::GetLayerCapabilities(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  Layer* l = m_reference->m_layerlist->getLayer(id);
  initReply();
  appendUint(l->getCapabilities());
  closeReply();
}

void DBUSCommunicator::FadeIn(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  initReply();
  closeReply();
}

void DBUSCommunicator::SynchronizedFade(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  initReply();
  closeReply();
}

void DBUSCommunicator::FadeOut(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  initReply();
  closeReply();
}

void DBUSCommunicator::Exit(DBusConnection* conn,DBusMessage* msg )
{
  currentMsg = msg;
  m_reference->m_executor->execute(new ExitCommand());
  initReply();
  closeReply();
}

void DBUSCommunicator::CommitChanges(DBusConnection* conn,DBusMessage* msg )
{
  currentMsg = msg;
  m_reference->m_executor->execute(new CommitCommand());
  initReply();
  closeReply();
}

void DBUSCommunicator::CreateShader(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  char* vertname = getStringFromMessage();
  dbus_message_iter_next(&iter);
  char* fragname = getStringFromMessage();
  uint id;
  m_reference->m_executor->execute(new CreateShaderCommand(vertname,fragname,&id));
  initReply();
  appendUint(id);
  closeReply();
}

void DBUSCommunicator::DestroyShader(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint shaderid = getUIntFromMessage();
  m_reference->m_executor->execute(new DestroyShaderCommand(shaderid));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetShader(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint objectid = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  uint shaderid = getUIntFromMessage();
  m_reference->m_executor->execute(new SetShaderCommand(objectid,shaderid));
  initReply();
  closeReply();
}

void DBUSCommunicator::SetUniforms(DBusConnection* conn,DBusMessage* msg )
{
  initReceive(msg);
  uint id = getUIntFromMessage();
  dbus_message_iter_next(&iter);
  if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&iter))
    LOG_ERROR("DBUSCommunicator", "Argument is not an array!");
  std::vector<std::string> uniforms;
  DBusMessageIter arrayIter;

  dbus_message_iter_recurse(&iter,&arrayIter);
  bool hasNext = true;
  while(hasNext){
    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&arrayIter))
      LOG_ERROR("DBUSCommunicator", "Argument is not an string!");
    char* param;
    dbus_message_iter_get_basic(&arrayIter,&param);
    uniforms.push_back(std::string(param));
    if (dbus_message_iter_has_next(&arrayIter))
      dbus_message_iter_next(&arrayIter);
    else
      hasNext = false;
  }

  m_reference->m_executor->execute(new SetUniformsCommand(id,uniforms));
  initReply();
  closeReply();
}

void* DBUSCommunicator::run(void * arg){
  LOG_INFO("DBUSCommunicator","Running..");
  //	pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
  //	pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  m_reference = (DBUSCommunicator*)arg;
  while(m_reference->m_running){
    //		pthread_testcancel();
    dbus_connection_read_write(conn, 50);
    DBusMessage* msg = dbus_connection_pop_message(conn);
    if (NULL!=msg){
      const char *n = dbus_message_get_member (msg);

      bool found = false;
      int i=0;
      while(!found && strcmp(manager_methods[i].name,"")!=0){
        if (n && strcmp(manager_methods[i].name,n) == 0){
          MethodTable entry =  manager_methods[i];
          LOG_DEBUG("DBUSC","called methodname:" << entry.name);
          CallBackMethod m = entry.function;
          (m_reference->*m)(conn,msg);
          found = true;
        }
        i++;
      }
      if (dbus_message_is_method_call(msg, DBUS_INTERFACE_INTROSPECTABLE, "Introspect")){
        LOG_DEBUG("DBUSCommunicator", "Introspection called");
        introspectCallback(conn, msg);
        found = true;
      }
      if (!found){
        DBusMessage* reply = dbus_message_new_method_return(msg);
        uint serial = 0;
        // send the reply && flush the connection
        if (!dbus_connection_send(conn, reply, &serial)) {
          LOG_ERROR("DBUSCommunicator", "Out Of Memory!");
          exit(1);
        }
        dbus_connection_flush(conn);

        // free the reply
        dbus_message_unref(reply);
      }

    }
  }

  LOG_DEBUG("DBUSCommunicator","ending thread");
  return 0;
}

extern "C" DBUSCommunicator* createDBUSCommunicator(CommandExecutor* executor, LayerList* ll) {
  return new DBUSCommunicator(executor,ll);
}

extern "C" void destroyDBUSCommunicator(DBUSCommunicator* p) {
  delete p;
}


