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

void ViewportTransform::applyLayerSource(const Rectangle& layerSource, Rectangle& surfaceSource, Rectangle& surfaceDestination)
{
    int cropamount = 0;
    float surfaceInverseScaleX = (float)surfaceSource.width / surfaceDestination.width;
    float surfaceInverseScaleY = (float)surfaceSource.height / surfaceDestination.height;

    // X

    // Crop from left
    cropamount = layerSource.x - surfaceDestination.x;
    if (cropamount > 0)
    {
        surfaceDestination.x = 0;
        surfaceDestination.width -= cropamount;

        // crop a proportional part of the source region
        surfaceSource.x += (float)cropamount * surfaceInverseScaleX;
        surfaceSource.width -= (float)cropamount * surfaceInverseScaleX;
    }
    else
    {
        surfaceDestination.x -= layerSource.x;
    }

    // Crop from right
    cropamount = surfaceDestination.x + surfaceDestination.width - layerSource.width;
    if (cropamount > 0)
    {
        surfaceDestination.width -= cropamount;

        // crop a proportional part of the source region
        surfaceSource.width -= (float)cropamount * surfaceInverseScaleX;
    }

    // Y

    // Crop from top
    cropamount = layerSource.y - surfaceDestination.y;
    if (cropamount > 0)
    {
        surfaceDestination.y = 0;
        surfaceDestination.height -= cropamount;

        // crop a proportional part of the source region
        surfaceSource.y += (float)cropamount * surfaceInverseScaleY;
        surfaceSource.height -= (float)cropamount * surfaceInverseScaleY;
    }
    else
    {
        surfaceDestination.y -= layerSource.y;
    }

    // Crop from bottom
    cropamount = surfaceDestination.y + surfaceDestination.height - layerSource.height;
    if (cropamount > 0)
    {
        surfaceDestination.height -= cropamount;

        // crop a proportional part of the source region
        surfaceSource.height -= (float)cropamount * surfaceInverseScaleY;
    }
}

void ViewportTransform::applyLayerDestination(const Rectangle& layerDestination, const Rectangle& layerSource, Rectangle& regionToScale)
{
    // scale position proportional to change in layer size
    regionToScale.x *= (float)layerDestination.width/layerSource.width;
    // scale width proportional to change in layer size
    regionToScale.width *= (float)layerDestination.width/layerSource.width;
    // after scaling, move surface because its position should be relative to moved layer
    regionToScale.x+=layerDestination.x;

    // scale position proportional to change in layer size
    regionToScale.y *= (float)layerDestination.height/layerSource.height;
    // scale width proportional to change in layer size
    regionToScale.height *= (float)layerDestination.height/layerSource.height;
    // after scaling, move surface because its position should be relative to moved layer
    regionToScale.y+=layerDestination.y;
}

void ViewportTransform::transformRectangleToTextureCoordinates(const Rectangle& rectangle, uint originalWidth, uint originalHeight, float* textureCoordinates)
{
    float originalWidthAsFloat = originalWidth;
    float originalHeightAsFloat = originalHeight;

    // move texture coordinate proportional to the cropped pixels
    float percentageCroppedFromLowerSide_U = rectangle.x/originalWidthAsFloat;
    textureCoordinates[0] = percentageCroppedFromLowerSide_U;

    // move texture coordinate proportional to the cropped pixels
    uint newRightSide = rectangle.x+rectangle.width;
    float percentageCroppedFromRightSide = (originalWidthAsFloat-newRightSide)/originalWidthAsFloat;
    textureCoordinates[1] = 1.0f-percentageCroppedFromRightSide ;

    // the same for Y
    // move texture coordinate proportional to the cropped pixels
    float percentageCroppedFromTopSideV = ((float)rectangle.y/originalHeightAsFloat);
    textureCoordinates[2] = percentageCroppedFromTopSideV;

    // move texture coordinate proportional to the cropped pixels
    int newBottomSide = rectangle.y+rectangle.height;
    float percentageCroppedFromBottomSide = (float)(originalHeightAsFloat-newBottomSide)/originalHeightAsFloat;
    textureCoordinates[3] = 1.0f-percentageCroppedFromBottomSide ;
}
