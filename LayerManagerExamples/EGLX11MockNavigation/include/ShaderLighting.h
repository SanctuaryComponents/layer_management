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
#ifndef SHADERLIGHTING_H_
#define SHADERLIGHTING_H_

#include "ShaderBase.h"

class ShaderLighting: public ShaderBase {
public:
	ShaderLighting(IlmMatrix* projectionMatrix);
	virtual ~ShaderLighting();

	virtual void use(vec3f* position, vec4f* color);

private:
	vec3f* m_pPosition;
	vec4f* m_pColor;
	int m_uniformModelMatrix;
	int m_uniformColor;
};

#endif /* SHADERLIGHTING_H_ */
