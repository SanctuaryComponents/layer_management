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

#ifndef _LAYER_H_
#define _LAYER_H_

#include "Surface.h"
#include "LayerType.h"
#include <list>
/*
 * A graphical Layerobject which contains Surfaces.
 */
class Layer : public GraphicalSurface{

public:

	LayerType getLayerType(){	return layerType; 	};
	void setLayerType(LayerType lt){	layerType = lt; }

	void setLayerCapabilities(uint newCapabilities){ capabilities = newCapabilities; };
	uint getCapabilities(){	return capabilities;	};

	void addSurface(Surface* s)	{	surfaces.push_back(s);	};
	void removeSurface(Surface* s){	surfaces.remove(s);		};

	std::list<Surface*>& getAllSurfaces(){return surfaces;}

	std::list<Surface*> surfaces;
private:
	LayerType layerType;
	uint capabilities;
	Layer() : GraphicalSurface(TypeLayer), layerType(Software_2D), capabilities(0){};
	friend class LayerList;

};


#endif /* _LAYER_H_ */
