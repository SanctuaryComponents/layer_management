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

#ifndef X11GLXRENDERER_H_
#define X11GLXRENDERER_H_

#include "LayerList.h"
#include "BaseRenderer.h"
#include "GraphicSystems/BaseGraphicSystem.h"

class X11GLXRenderer : public BaseRenderer {
public:
	X11GLXRenderer(LayerList* layerlist);
	virtual ~X11GLXRenderer();
	void doScreenShot(std::string fileToSave);
private:
	BaseGraphicSystem* graphicSys;
};

#endif /* X11GLXRENDERER_H_ */
