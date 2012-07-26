/***************************************************************************
*
* Copyright 2010,2011 BMW Car IT GmbH
* Copyright (C) 2012 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
****************************************************************************/
// alpha value of the surfaces
uniform mediump float uAlphaVal;
// textureunit which is accessed
uniform mediump sampler2D uTexUnit;
// chromakey of the surfaces
uniform mediump vec3 uChromaKey;

// texture coordinates sended by the vertex shader
varying mediump vec2 vTexout;

void main()
{
	// correct Texture Coords;
    mediump vec4 tcolor = texture2D(uTexUnit, vTexout );
        // if both match, discarding
    if (tcolor.rgb == uChromaKey.rgb)
    {
        discard;
    }
    mediump vec4 color = tcolor * vec4(1.0,1.0,1.0,uAlphaVal);
    gl_FragColor = color;
}
