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

void ViewportTransform::applyLayerSource(const Rectangle& layerSource, Rectangle& surfaceSource, Rectangle& surfaceDestination)
{
    Rectangle regionBeforeTransformation = surfaceDestination;

    // X

    // crop surface from lower side if it is outside the cropregion of the layer
    if (regionBeforeTransformation.x < layerSource.x)
    { // partly outside lower end
        // crop a part of the destination region
        surfaceDestination.width -= layerSource.x-regionBeforeTransformation.x;

        if ( surfaceDestination.x < layerSource.x )
            surfaceDestination.x = 0;
        else
            surfaceDestination.x -= layerSource.x;

        // crop a proportional part of the source region
        surfaceSource.x += (float)(layerSource.x-regionBeforeTransformation.x)/regionBeforeTransformation.width * surfaceSource.width;
        surfaceSource.width -= (float)(layerSource.x-regionBeforeTransformation.x)/regionBeforeTransformation.width * surfaceSource.width;
    }
    // crop surface from higher side if some part of surface lies beyond the cropregion of the layer
    if(regionBeforeTransformation.x+regionBeforeTransformation.width > layerSource.x+layerSource.width)
    { // partially outside higher end
        // crop a part of the surfaces geometry that extended beyond layer cropregion
        surfaceDestination.width -= regionBeforeTransformation.x+regionBeforeTransformation.width-layerSource.x-layerSource.width;

        if (surfaceDestination.x < layerSource.x )
            surfaceDestination.x = 0;
        else
            surfaceDestination.x -= layerSource.x;

        surfaceSource.width -= regionBeforeTransformation.x+regionBeforeTransformation.width-layerSource.x-layerSource.width;
    }
    // if surface is completely within the cropregion then just move it by the amount cropped from the lower end of the layer
    if (regionBeforeTransformation.x >= layerSource.x && regionBeforeTransformation.x+regionBeforeTransformation.width <= layerSource.x+layerSource.width)
    {
        if (surfaceDestination.x < layerSource.x )
            surfaceDestination.x = 0;
        else
            surfaceDestination.x -= layerSource.x;
    }

    // y

    // crop surface from lower side if it is outside the cropregion of the layer
    if (regionBeforeTransformation.y < layerSource.y)
    { // partly outside lower end
        // crop a part of the destination region
        surfaceDestination.height -= layerSource.y-regionBeforeTransformation.y;

        if ( surfaceDestination.y < layerSource.y )
            surfaceDestination.y = 0;
        else
            surfaceDestination.y -= layerSource.y;

        // crop a proportional part of the source region
        surfaceSource.y += (float)(layerSource.y-regionBeforeTransformation.y)/regionBeforeTransformation.height * surfaceSource.height;
        surfaceSource.height -= (float)(layerSource.y-regionBeforeTransformation.y)/regionBeforeTransformation.height * surfaceSource.height;
    }
    // crop surface from higher side if some part of surface lies beyond the cropregion of the layer
    if(regionBeforeTransformation.y+regionBeforeTransformation.height > layerSource.y+layerSource.height)
    { // partially outside higher end
        // crop a part of the surfaces geometry that eytended beyond layer cropregion
        surfaceDestination.height -= regionBeforeTransformation.y+regionBeforeTransformation.height-layerSource.y-layerSource.height;

        if (surfaceDestination.y < layerSource.y )
            surfaceDestination.y = 0;
        else
            surfaceDestination.y -= layerSource.y;

        surfaceSource.height -= regionBeforeTransformation.y+regionBeforeTransformation.height-layerSource.y-layerSource.height;
    }
    // if surface is completely within the cropregion then just move it by the amount cropped from the lower end of the layer
    if (regionBeforeTransformation.y >= layerSource.y && regionBeforeTransformation.y+regionBeforeTransformation.height <= layerSource.y+layerSource.height)
    {
        if (surfaceDestination.y < layerSource.y )
            surfaceDestination.y = 0;
        else
            surfaceDestination.y -= layerSource.y;
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
