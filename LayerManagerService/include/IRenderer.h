/***************************************************************************
*
* Copyright 2010,2011 BMW Car IT GmbH
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

#ifndef _IRENDERER_H_
#define _IRENDERER_H_

#include "LayerList.h"
#include "Log.h"
#include "LayerType.h"

/**
 * Abstract Base of all CompositingControllers, ie Renderers.
 */
class IRenderer {
public:
	virtual ~IRenderer(){};

	/**
	 * Start the actual renderingprocess (renderloop)
	 */
	virtual bool start(int width, int height, const char* displayName )=0;

	/**
	 * Stop renderingprocess
	 */
	virtual void stop()=0;

	/**
	 * Switch debug mode of this component on or off
	 * @param onoff Turn on debug mode (true) or off (false)
	 */
	virtual void setdebug(bool onoff)=0;

	virtual void doScreenShot(std::string fileToSave)=0;
	virtual void doScreenShotOfLayer(std::string fileToSave, const uint id)=0;
	virtual void doScreenShotOfSurface(std::string fileToSave, const uint id)=0;

	virtual uint getLayerTypeCapabilities(LayerType layertype)=0;

	virtual uint getNumberOfHardwareLayers(uint screenID)=0;

	virtual uint* getScreenResolution(uint screenID)=0;

	virtual uint* getScreenIDs(uint* length)=0;

};
#endif /* _IRENDERER_H_ */
