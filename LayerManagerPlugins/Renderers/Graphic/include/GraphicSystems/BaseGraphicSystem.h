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

#ifndef _BASEGRAPHICSYSTEM_H_
#define _BASEGRAPHICSYSTEM_H_

#include "TextureBinders/ITextureBinder.h"
#include "PlatformSurface.h"
#include "Surface.h"
#include "Layer.h"

class BaseGraphicSystem {
public:
	virtual ~BaseGraphicSystem(){};
	virtual bool init(void* display, void* WindowID,int WindowHeight, int WindowWidth)=0;
	virtual void drawSurface(Layer*,Surface*)=0;
	virtual void clearBackground()=0;
	virtual void swapBuffers()=0;
	virtual void doScreenShot(std::string fileToSave)=0;
	ITextureBinder* m_binder;

};

#endif /* _BASEGRAPHICSYSTEM_H_ */
