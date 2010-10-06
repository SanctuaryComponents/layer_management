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

#ifndef _BASECOMMUNICATOR_H_
#define _BASECOMMUNICATOR_H_

class BaseCommunicator;

#include <string>

class CommandExecutor;
class ILayerList;

/**
 * Abstract base for all communicator plugins. Subclasses construct command objects
 * objects from invidual values and give these to the executor object for processing.
 */
class BaseCommunicator {
public:

	/**
	 * Constructor
	 * @param executor Pointer to an object to send commands to
	 * @param layerlist Pointer to the layerlist currently in use
	 */
	BaseCommunicator(CommandExecutor * executor, ILayerList* layerlist) : m_executor(executor), m_layerlist(layerlist){};
	virtual ~BaseCommunicator(){};

	/**
	 * Start communication process, ie start specific listening process of communication method
	 */
	virtual bool start()=0;

	/**
	 * Stop communication. Stop sending command objects.
	 */
	virtual void stop()=0;

	/**
	 * Switch debug mode of this component on or off
	 * @param onoff Turn on debug mode (true) or off (false)
	 */
	virtual void setdebug(bool onoff)=0;

protected:
	// the object to send received commands to
	CommandExecutor* m_executor;
	ILayerList* m_layerlist;
};

#endif /* _BASECOMMUNICATOR_H_ */
