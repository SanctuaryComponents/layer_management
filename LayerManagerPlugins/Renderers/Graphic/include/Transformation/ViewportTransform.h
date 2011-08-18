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
    static void applySurfaceSource(Surface *surface, float* u_left,float* u_right,float* v_left,float* v_right);
    
    static void applySurfaceDest(Surface *surface, Rectangle *newSurfacePos);
    
    static void applyLayerSource(Rectangle* newSurfacePos, Layer* layer, float* u_left, float* u_right, float* v_left, float* v_right);
    
    static void applyLayerDest(Rectangle* newSurfacePos, Layer* layer);
};


#endif /* _VIEWPORT_TRANSFORM_H_ */
