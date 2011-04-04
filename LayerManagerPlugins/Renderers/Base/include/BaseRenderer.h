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

#ifndef _BASERENDERER_H_
#define _BASERENDERER_H_

#include "LayerType.h"
#include "LayerList.h"
#include "IRenderer.h"
#include "WindowSystems/BaseWindowSystem.h"

class BaseRenderer : public IRenderer{
public:
	BaseRenderer(LayerList* layerlist);
	virtual ~BaseRenderer();

	bool start(int, int, const char*)=0;
	void stop()=0;
	void setdebug(bool onoff){debugMode = onoff;};
	virtual void doScreenShot(std::string fileToSave)=0;
	uint getLayerTypeCapabilities(LayerType layerType);

protected:
	LayerList* m_layerlist;

private:
	static bool debugMode;
};

#endif /* _BASERENDERER_H_*/
