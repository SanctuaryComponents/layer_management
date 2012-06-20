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

#ifndef _VIEWPORT_TRANSFORM_H_
#define _VIEWPORT_TRANSFORM_H_

#include "Layer.h"
#include "Surface.h"

class ViewportTransform
{
public:

    /*
     * Returns true if surface is completely cropped by the given layer source region
     */
    static bool isFullyCropped(const Rectangle& surfaceDestination, const Rectangle& layerSource);

    /*
     * Apply Source View of Layer to the given surface source and destination regions, ie cropping surface parts to layer source view
     */
    static void applyLayerSource(const FloatRectangle& layerSource, FloatRectangle& surfaceSource, FloatRectangle& surfaceDestination);

    /*
     * Apply Destination View of Layer to the given surface destination region, ie scale and movement relative to scaling and position of layer
     */
    static void applyLayerDestination(const FloatRectangle& layerDestination, const FloatRectangle& layerSource, FloatRectangle& regionToScale);

    /*
     * Calculate Texture Coordinates as relation of the given rectangle to original values.
     * Example: x position of 10 with an original width of 40 will yield a texture coordinate of 10/40=0.25f.
     * This function expects textureCoordinates to be an allocated float array of size 4, in which the texture coordinates will be returned.
     */
    static void transformRectangleToTextureCoordinates(const FloatRectangle& rectangle, const float originalWidth, const float originalHeight, float* textureCoordinates);
};

#endif /* _VIEWPORT_TRANSFORM_H_ */
