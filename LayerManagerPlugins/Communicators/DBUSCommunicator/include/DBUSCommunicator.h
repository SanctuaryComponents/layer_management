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
#ifndef _DBUSCOMMUNICATOR_H_
#define _DBUSCOMMUNICATOR_H_

#include "BaseCommunicator.h"
#include "CommandExecutor.h"
#include "Log.h"
#include <dbus/dbus.h>

class DBUSCommunicator: public BaseCommunicator{
public:
	DBUSCommunicator(CommandExecutor* executor, ILayerList* ll);
	virtual ~DBUSCommunicator();
	virtual bool start();
	virtual void stop();
	virtual void setdebug(bool onoff);

	void Debug(DBusConnection* conn,DBusMessage* msg );
	void ListAllLayerIDS(DBusConnection* conn,DBusMessage* msg );
	void ListAllSurfaceIDS(DBusConnection* conn,DBusMessage* msg );
	void ListAllLayerGroupIDS(DBusConnection* conn,DBusMessage* msg );
	void ListAllSurfaceGroupIDS(DBusConnection* conn,DBusMessage* msg );
	void ListSurfacesOfSurfacegroup(DBusConnection* conn,DBusMessage* msg );
	void ListLayersOfLayergroup(DBusConnection* conn,DBusMessage* msg );
	void ListSurfaceofLayer(DBusConnection* conn,DBusMessage* msg );
	void getPropertiesOfSurface(DBusConnection* conn,DBusMessage* msg );
	void getPropertiesOfLayer(DBusConnection* conn,DBusMessage* msg );
	void CreateSurface(DBusConnection* conn,DBusMessage* msg );
	void RemoveSurface(DBusConnection* conn,DBusMessage* msg );
	void CreateLayer(DBusConnection* conn,DBusMessage* msg );
	void RemoveLayer(DBusConnection* conn,DBusMessage* msg );
	void AddSurfaceToSurfaceGroup(DBusConnection* conn,DBusMessage* msg );
	void RemoveSurfaceFromSurfaceGroup(DBusConnection* conn,DBusMessage* msg );
	void AddLayerToLayerGroup(DBusConnection* conn,DBusMessage* msg );
	void RemoveLayerFromLayerGroup(DBusConnection* conn,DBusMessage* msg );
	void AddSurfaceToLayer(DBusConnection* conn,DBusMessage* msg );
	void RemoveSurfaceFromLayer(DBusConnection* conn,DBusMessage* msg );
	void CreateSurfaceGroup(DBusConnection* conn,DBusMessage* msg );
	void RemoveSurfaceGroup(DBusConnection* conn,DBusMessage* msg );
	void CreateLayerGroup(DBusConnection* conn,DBusMessage* msg );
	void RemoveLayerGroup(DBusConnection* conn,DBusMessage* msg );
	void SetSurfaceSourceRegion(DBusConnection* conn,DBusMessage* msg );
	void SetLayerSourceRegion(DBusConnection* conn,DBusMessage* msg );
	void SetSurfaceDestinationRegion(DBusConnection* conn,DBusMessage* msg );
	void SetSurfacePosition(DBusConnection* conn,DBusMessage* msg );
	void GetSurfacePosition(DBusConnection* conn,DBusMessage* msg );
	void SetSurfaceDimension(DBusConnection* conn,DBusMessage* msg );
	void SetLayerDestinationRegion(DBusConnection* conn,DBusMessage* msg );
	void SetLayerPosition(DBusConnection* conn,DBusMessage* msg );
	void GetLayerPosition(DBusConnection* conn,DBusMessage* msg );
	void SetLayerDimension(DBusConnection* conn,DBusMessage* msg );
	void GetLayerDimension(DBusConnection* conn,DBusMessage* msg );
	void GetSurfaceDimension(DBusConnection* conn,DBusMessage* msg );
	void SetSurfaceOpacity(DBusConnection* conn,DBusMessage* msg );
	void SetLayerOpacity(DBusConnection* conn,DBusMessage* msg );
	void SetSurfacegroupOpacity(DBusConnection* conn,DBusMessage* msg );
	void SetLayergroupOpacity(DBusConnection* conn,DBusMessage* msg );
	void GetSurfaceOpacity(DBusConnection* conn,DBusMessage* msg );
	void GetLayerOpacity(DBusConnection* conn,DBusMessage* msg );
	void GetSurfacegroupOpacity(DBusConnection* conn,DBusMessage* msg );
	void GetLayergroupOpacity(DBusConnection* conn,DBusMessage* msg );
	void SetSurfaceOrientation(DBusConnection* conn,DBusMessage* msg );
	void SetLayerOrientation(DBusConnection* conn,DBusMessage* msg );
	void GetSurfacePixelformat(DBusConnection* conn,DBusMessage* msg );
	void SetSurfaceVisibility(DBusConnection* conn,DBusMessage* msg );
	void SetLayerVisibility(DBusConnection* conn,DBusMessage* msg );
	void GetSurfaceVisibility(DBusConnection* conn,DBusMessage* msg );
	void GetLayerVisibility(DBusConnection* conn,DBusMessage* msg );
	void SetSurfacegroupVisibility(DBusConnection* conn,DBusMessage* msg );
	void SetLayergroupVisibility(DBusConnection* conn,DBusMessage* msg );
	void SetRenderOrderOfLayers(DBusConnection* conn,DBusMessage* msg );
	void SetSurfaceRenderOrderWithinLayer(DBusConnection* conn,DBusMessage* msg );
	void GetLayerType(DBusConnection* conn,DBusMessage* msg );
	void GetLayertypeCapabilities(DBusConnection* conn,DBusMessage* msg );
	void GetLayerCapabilities(DBusConnection* conn,DBusMessage* msg );
	void FadeIn(DBusConnection* conn,DBusMessage* msg );
	void SynchronizedFade(DBusConnection* conn,DBusMessage* msg );
	void FadeOut(DBusConnection* conn,DBusMessage* msg );
	void Exit(DBusConnection* conn,DBusMessage* msg );
	void CommitChanges(DBusConnection* conn,DBusMessage* msg );
	void CreateShader(DBusConnection* conn,DBusMessage* msg );
	void DestroyShader(DBusConnection* conn,DBusMessage* msg );
	void SetShader(DBusConnection* conn,DBusMessage* msg );
	void SetUniforms(DBusConnection* conn,DBusMessage* msg );
private:
	static void* run(void * threadid);
	pthread_t m_currentThread;
	static DBUSCommunicator* m_reference;
	bool m_running;
};

#endif /* _DBUSCOMMUNICATOR_H_ */
