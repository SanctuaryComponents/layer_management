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

#include "Transformation/ViewportTransform.h"

bool ViewportTransform::isFullyCropped(const Rectangle& surfaceDestination, const Rectangle& layerSource)
{
    // is surface completely to the right of layer source region?
    if (surfaceDestination.x >= layerSource.x + layerSource.width )
        return true;
    // is surface completely beneath layer source region?
    if (surfaceDestination.y >= layerSource.y + layerSource.height )
        return true;
    // is surface completely to the left of layer source region?
    if (surfaceDestination.x + surfaceDestination.width <= layerSource.x)
        return true;
    // is surface completely above layer source region?
    if (surfaceDestination.y + surfaceDestination.height <= layerSource.y)
        return true;
    return false;
}

void ViewportTransform::applyLayerSource(const FloatRectangle& layerSource, FloatRectangle& surfaceSource, FloatRectangle& surfaceDestination)
{
    float cropamount = 0;
    float surfaceInverseScaleX = surfaceSource.width / surfaceDestination.width;
    float surfaceInverseScaleY = surfaceSource.height / surfaceDestination.height;

    // X

    // Crop from left
    cropamount = layerSource.x - surfaceDestination.x;
    if (cropamount > 0.0f)
    {
        surfaceDestination.x = 0.0f;
        surfaceDestination.width -= cropamount;

        // crop a proportional part of the source region
        surfaceSource.x += cropamount * surfaceInverseScaleX;
        surfaceSource.width -= cropamount * surfaceInverseScaleX;
    }
    else
    {
        surfaceDestination.x -= layerSource.x;
    }

    // Crop from right
    cropamount = surfaceDestination.x + surfaceDestination.width - layerSource.width;
    if (cropamount > 0.0f)
    {
        surfaceDestination.width -= cropamount;

        // crop a proportional part of the source region
        surfaceSource.width -= cropamount * surfaceInverseScaleX;
    }

    // Y

    // Crop from top
    cropamount = layerSource.y - surfaceDestination.y;
    if (cropamount > 0.0f)
    {
        surfaceDestination.y = 0.0f;
        surfaceDestination.height -= cropamount;

        // crop a proportional part of the source region
        surfaceSource.y += cropamount * surfaceInverseScaleY;
        surfaceSource.height -= cropamount * surfaceInverseScaleY;
    }
    else
    {
        surfaceDestination.y -= layerSource.y;
    }

    // Crop from bottom
    cropamount = surfaceDestination.y + surfaceDestination.height - layerSource.height;
    if (cropamount > 0.0f)
    {
        surfaceDestination.height -= cropamount;

        // crop a proportional part of the source region
        surfaceSource.height -= cropamount * surfaceInverseScaleY;
    }
}

void ViewportTransform::applyLayerDestination(const FloatRectangle& layerDestination, const FloatRectangle& layerSource, FloatRectangle& regionToScale)
{
    float scaleX = layerDestination.width / layerSource.width;
    float scaleY = layerDestination.height / layerSource.height;

    // scale position proportional to change in float*layer size
    regionToScale.x *= scaleX;
    // scale width proportional to change in layer size
    regionToScale.width *= scaleX;
    // after scaling, move surface because its position should be relative to moved layer
    regionToScale.x += layerDestination.x;

    // scale position proportional to change in layer size
    regionToScale.y *= scaleY;
    // scale width proportional to change in layer size
    regionToScale.height *= scaleY;
    // after scaling, move surface because its position should be relative to moved layer
    regionToScale.y += layerDestination.y;
}

void ViewportTransform::transformRectangleToTextureCoordinates(const FloatRectangle& rectangle, const float originalWidth, const float originalHeight, float* textureCoordinates)
{
    // move texture coordinate proportional to the cropped pixels
    float percentageCroppedFromLeftSide = rectangle.x / originalWidth;
    textureCoordinates[0] = percentageCroppedFromLeftSide;

    // move texture coordinate proportional to the cropped pixels
    uint newRightSide = rectangle.x + rectangle.width;
    float percentageCroppedFromRightSide = (originalWidth - newRightSide) / originalWidth;
    textureCoordinates[1] = 1.0f - percentageCroppedFromRightSide;

    // the same for Y
    // move texture coordinate proportional to the cropped pixels
    float percentageCroppedFromTopSide = rectangle.y / originalHeight;
    textureCoordinates[2] = percentageCroppedFromTopSide;

    // move texture coordinate proportional to the cropped pixels
    int newBottomSide = rectangle.y + rectangle.height;
    float percentageCroppedFromBottomSide = (originalHeight - newBottomSide) / originalHeight;
    textureCoordinates[3] = 1.0f - percentageCroppedFromBottomSide;
}
