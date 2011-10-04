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
#include "Argument.h"
#include "MockNavi.h"
#include "LayerScene.h"

#include <iostream>
using std::cout;

#define DEFAULT_FPS      30.0
#define DEFAULT_ANIM     1.0
#define DEFAULT_LAYER    LAYER_EXAMPLE_GLES_APPLICATIONS
#define DEFAULT_SURFACE  SURFACE_EXAMPLE_EGLX11_APPLICATION
#define DEFAULT_WIDTH    800
#define DEFAULT_HEIGHT   480

int main (int argc, const char * argv[])
{
    FloatArgument fps("fps", DEFAULT_FPS, argc, argv);
    FloatArgument animSpeed("anim", DEFAULT_ANIM, argc, argv);
    IntArgument surfaceId("surface", DEFAULT_SURFACE, argc, argv);
    IntArgument layerId("layer", DEFAULT_LAYER, argc, argv);
    IntArgument width("width", DEFAULT_WIDTH, argc, argv);
    IntArgument height("height", DEFAULT_HEIGHT, argc, argv);
    BoolArgument help("help", false, argc, argv);
    
    if (help.get())
    {
        cout << "\nUsage: " << argv[0] << " [options]\n"
             << "possible options are:\n"
             << "  -help         show this help text\n"
             << "  -fps x        limit frames per second to x (default " << DEFAULT_FPS << ")\n"
             << "  -anim x       set animation speed (default " << DEFAULT_ANIM << ")\n"
             << "  -surface x    render to surface id x (default " << DEFAULT_SURFACE << ")\n"
             << "  -layer x      add surface to layer x (default " << DEFAULT_LAYER << ")\n"
             << "  -width x      set surface width to x (default " << DEFAULT_WIDTH << ")\n"
             << "  -height x     set surface height to x (default " << DEFAULT_HEIGHT << ")\n\n";
    }
    else
    {
        SurfaceConfiguration config;
        config.layerId = layerId.get();
        config.surfaceId = surfaceId.get();
        config.surfaceWidth = width.get();
        config.surfaceHeight = height.get();

        MockNavi navi(fps.get(), animSpeed.get(), &config);
        navi.mainloop();
    }

    return 0;
}

