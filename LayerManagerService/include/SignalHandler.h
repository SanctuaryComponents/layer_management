/***************************************************************************
 * Copyright 2012 BMW Car IT GmbH
 * Copyright (C) 2012 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
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

#ifndef __SIGNALHADLER_H__
#define __SIGNALHADLER_H__

#include "ilm_types.h"

class SignalHandler
{
public:
    SignalHandler();
    ~SignalHandler();
    t_ilm_bool shutdownSignalReceived();
};

#endif // __SIGNALHADLER_H__
