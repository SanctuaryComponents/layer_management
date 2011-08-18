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

void ViewportTransform::applySurfaceSource(Surface *surface, float* u_left,float* u_right,float* v_left,float* v_right) 
{
    // move texture coordinate proportional to the cropped pixels
    const Rectangle& surfaceSource = surface->getSourceRegion();
    float percentageCroppedFromLowerSide_U = ((float)surfaceSource.x/surface->OriginalSourceWidth);
    float textureRangeU = u_right-u_left;
    *u_left += percentageCroppedFromLowerSide_U * textureRangeU;

    // move texture coordinate proportional to the cropped pixels
    int newRightSide = surfaceSource.x+surfaceSource.width;    
    float percentageCroppedFromRightSide = (float)(surface->OriginalSourceWidth-newRightSide)/surface->OriginalSourceWidth;
    *u_right -= percentageCroppedFromRightSide * textureRangeU;

    // the same for Y
    // move texture coordinate proportional to the cropped pixels
    float percentageCroppedFromTopSideV = ((float)surfaceSource.y/surface->OriginalSourceHeight);
    float textureRangeV = v_left-v_right;
    *v_left += percentageCroppedFromTopSideV * textureRangeV;

    // move texture coordinate proportional to the cropped pixels
    int newBottomSide = surfaceSource.y+surfaceSource.height;
    float percentageCroppedFromBottomSide = (float)(surface->OriginalSourceHeight-newBottomSide)/surface->OriginalSourceHeight;
    *v_right -= percentageCroppedFromBottomSide * textureRangeV;

}
    
void ViewportTransform::applySurfaceDest(Surface *surface, Rectangle *newSurfacePos) 
{
    const Rectangle& surfaceDest = surface->getDestinationRegion();
    (*newSurfacePos).x = surfaceDest.x;
    (*newSurfacePos).width = surfaceDest.width;

    (*newSurfacePos).y = surfaceDest.y;
    (*newSurfacePos).height = surfaceDest.height;
}
    
void ViewportTransform::applyLayerSource(Rectangle* newSurfacePos, Layer* layer, float* u_left, float* u_right, float* v_left, float* v_right) 
{
    Rectangle surfacePosOrig;
    const Rectangle& layerSource = layer->getSourceRegion();
    surfacePosOrig.x = newSurfacePos->x;
    surfacePosOrig.width = newSurfacePos->width;
    surfacePosOrig.y = newSurfacePos->y;
    surfacePosOrig.height = newSurfacePos->height;
    float u_leftOrig = *u_left;
    float u_rightOrig = *u_right;
    float v_leftOrig = *v_left;
    float v_rightOrig = *v_right;

    // X

    // crop surface from lower side if it is outside the cropregion of the layer
    if (surfacePosOrig.x < layerSource.x)
    { // partly outside lower end
        // crop a proportional part of the texture
        *u_left += (float)(layerSource.x-surfacePosOrig.x)/surfacePosOrig.width * (u_rightOrig-u_leftOrig);
        // crop a part of the geometry
        (*newSurfacePos).width -= layerSource.x-surfacePosOrig.x;

        if ( newSurfacePos->x < layerSource.x )
            (*newSurfacePos).x = 0;
        else
            (*newSurfacePos).x -= layerSource.x;
    }
    // crop surface from higher side if some part of surface lies beyond the cropregion of the layer
    if(surfacePosOrig.x+surfacePosOrig.width > layerSource.x+layerSource.width)
    { // partially outside higher end
        // crop a part of the texture
        *u_right -= (float)(surfacePosOrig.x+surfacePosOrig.width-layerSource.x-layerSource.width)/surfacePosOrig.width * (u_rightOrig-u_leftOrig);
        // crop a part of the surfaces geometry that extended beyond layer cropregion
        (*newSurfacePos).width -= surfacePosOrig.x+surfacePosOrig.width-layerSource.x-layerSource.width;
        if (newSurfacePos->x < layerSource.x )
            (*newSurfacePos).x = 0;
        else
            (*newSurfacePos).x -= layerSource.x;
    }
    // if surface is completely within the cropregion then just move it by the amount cropped from the lower end of the layer
    if (surfacePosOrig.x >= layerSource.x && surfacePosOrig.x+surfacePosOrig.width <= layerSource.x+layerSource.width)
    {
        if (newSurfacePos->x < layerSource.x )
            (*newSurfacePos).x = 0;
        else
            (*newSurfacePos).x -= layerSource.x;
    }

    //Y
    // crop surface from top side if it is outside the cropregion of the layer
    if (surfacePosOrig.y < layerSource.y)
    { // partly outside lower end
        // crop a proportional part of the texture
        *v_left += (float)(layerSource.y-surfacePosOrig.y)/surfacePosOrig.height * (v_rightOrig-v_leftOrig);
        // crop a part of the geometry
        (*newSurfacePos).height -= layerSource.y-surfacePosOrig.y;

        if ( newSurfacePos->y < layerSource.y )
            (*newSurfacePos).y = 0;
        else
            (*newSurfacePos).y -= layerSource.y;
    }
    // crop surface from higher side if some part of surface lies beyond the cropregion of the layer
    if(surfacePosOrig.y+surfacePosOrig.height > layerSource.y+layerSource.height)
    { // partially outside higher end
        // crop a part of the texture
        *v_right -= (float)(surfacePosOrig.y+surfacePosOrig.height-layerSource.y-layerSource.height)/surfacePosOrig.height * (v_rightOrig-v_leftOrig);
        // crop a part of the surfaces geometry that extended beyond layer cropregion
        (*newSurfacePos).height -= surfacePosOrig.y+surfacePosOrig.height-layerSource.y-layerSource.height;
        if (newSurfacePos->y < layerSource.y )
            (*newSurfacePos).y = 0;
        else
            (*newSurfacePos).y -= layerSource.y;
    }
    // if surface is completely within the cropregion then just move it by the amount cropped from the lower end of the layer
    if (surfacePosOrig.y >= layerSource.y && surfacePosOrig.y+surfacePosOrig.height <= layerSource.y+layerSource.height)
    {
        if (newSurfacePos->y < layerSource.y )
            (*newSurfacePos).y = 0;
        else
            (*newSurfacePos).y -= layerSource.y;
    }

}
    
void ViewportTransform::applyLayerDest(Rectangle* newSurfacePos, Layer* layer) 
{
        const Rectangle& layerDest = layer->getDestinationRegion();
        
		// scale position proportional to change in layer size
		(*newSurfacePos).x *= (float)layerDest.width/layer->OriginalSourceWidth;
		// scale width proportional to change in layer size
		(*newSurfacePos).width *= (float)layerDest.width/layer->OriginalSourceWidth;
		// after scaling, move surface because its position should be relative to moved layer
		(*newSurfacePos).x+=layerDest.x;

		// scale position proportional to change in layer size
		(*newSurfacePos).y *= (float)layerDest.height/layer->OriginalSourceHeight;
		// scale width proportional to change in layer size
		(*newSurfacePos).height *= (float)layerDest.height/layer->OriginalSourceHeight;
		// after scaling, move surface because its position should be relative to moved layer
		(*newSurfacePos).y+=layerDest.y;    
    
}
