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

#ifndef _ORIENTATIONTYPE_H
#define _ORIENTATIONTYPE_H

#include "ilm_types.h"

/**
 * Enumeration of possible orientations. Orientation in this context means Rotation.
 * Surfaces and Layers can be rotated for composition. Values mean clockwise rotation.
 */
enum OrientationType
{
    Zero = ILM_ZERO,
    Ninety = ILM_NINETY,
    OneEighty = ILM_ONEHUNDREDEIGHTY,
    TwoSeventy = ILM_TWOHUNDREDSEVENTY
};

#endif /* _ORIENTATIONTYPE_H */
