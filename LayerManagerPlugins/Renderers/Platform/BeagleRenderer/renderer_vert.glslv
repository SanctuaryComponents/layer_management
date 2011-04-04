/***************************************************************************
*
* Copyright 2010,2011 BMW Car IT GmbH
*
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

// Vertex position. Will be in range 0 to 1
attribute highp vec2 aPosition;
attribute highp vec2 aTexCoords;

// normalized window position
// (0, 0 is upper left,  1, 1 is lower right)
uniform mediump float uX;
uniform mediump float uY;

// normalized window size
// 1.0 will cover the entire viewport
uniform mediump float uWidth;
uniform mediump float uHeight;

// texrange describes the width and height of texture area which should be rendered
uniform mediump vec2 uTexRange;
// texrange describes the x and y position of the texture area which should be rendered
uniform mediump vec2 uTexOffset;

// texture coordinates
varying mediump vec2 vTexout;

void main()
{
	// Get the size correct. Multiply by 2 because -1 to +1 is mapped to 
	// the viewport
	gl_Position.xy = 2.0 * aPosition.xy * vec2( uWidth, uHeight);  			// rechte seite des fensters: volle breite sonst 0
gl_Position.xy += vec2(uX * 2.0 - 1.0+0.5, 1.0 - ((uY + uHeight)* 2.0)); 
	gl_Position.zw = vec2(0.0, 1.0);
	vTexout = vec2(uTexOffset.x + uTexRange.x * aTexCoords.x,1.0-(uTexOffset.y + aTexCoords.y*uTexRange.y+(1.0-uTexRange.y)));
}
