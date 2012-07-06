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

#ifndef _NULLWINDOWSYSTEM_H_
#define _NULLWINDOWSYSTEM_H_

#include "WindowSystems/BaseWindowSystem.h"
#include "Surface.h"
#include "Log.h"

class NullWindowSystem: public BaseWindowSystem
{
public:
    NullWindowSystem(Scene* pScene, int width, int height)
    : BaseWindowSystem(pScene, NULL)
    , windowWidth(width)
    , windowHeight(height)
    , resolutionWidth(width)
    , resolutionHeight(height)
    {
        LOG_DEBUG("NullWindowSystem", "creating BeagleWindowSystem");
    }

    bool start(int, int, const char*);

    void stop()
    {
    }


private:
    static void* EventLoop(void * ptr);

protected:
    static int windowWidth;
    static int windowHeight;
    static bool m_success;
    static bool m_initialized;

private:
    void Redraw();
    void printDebug(int posx, int posy);
    static bool debugMode;
    int resolutionWidth;
    int resolutionHeight;
};

#endif /* _NULLWINDOWSYSTEM_H_ */
