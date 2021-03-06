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
#define DEFAULT_POS_X    0
#define DEFAULT_POS_Y    0
#define DEFAULT_OPACITY  1.0
#define DEFAULT_NOSKY    false
#define DEFAULT_HELP     false

int main(int argc, const char * argv[])
{
    FloatArgument fps("fps", DEFAULT_FPS, argc, argv);
    FloatArgument animSpeed("anim", DEFAULT_ANIM, argc, argv);
    UnsignedIntArgument surfaceId("surface", DEFAULT_SURFACE, argc, argv);
    UnsignedIntArgument layerId("layer", DEFAULT_LAYER, argc, argv);
    IntArgument width("width", DEFAULT_WIDTH, argc, argv);
    IntArgument height("height", DEFAULT_HEIGHT, argc, argv);
    IntArgument posx("posx", DEFAULT_POS_X, argc, argv);
    IntArgument posy("posy", DEFAULT_POS_Y, argc, argv);
    FloatArgument opacity("opacity", DEFAULT_OPACITY, argc, argv);
    BoolArgument nosky("nosky", DEFAULT_NOSKY, argc, argv);
    BoolArgument help("help", DEFAULT_HELP, argc, argv);

    if (help.get())
    {
        cout << "\nUsage: " << argv[0] << " [options]\n"
             << "possible options are:\n"
             << "  -help         show this help text (default " << DEFAULT_HELP << ")\n"
             << "  -fps x        limit frames per second to x (default " << DEFAULT_FPS << ")\n"
             << "  -anim x       set animation speed (default " << DEFAULT_ANIM << ")\n"
             << "  -nosky        do not render sky, background transparent (default " << DEFAULT_NOSKY << ")\n"
             << "  -surface x    render to surface id x (default " << DEFAULT_SURFACE << ")\n"
             << "  -layer x      add surface to layer x (default " << DEFAULT_LAYER << ")\n"
             << "  -width x      set surface width to x (default " << DEFAULT_WIDTH << ")\n"
             << "  -height x     set surface height to x (default " << DEFAULT_HEIGHT << ")\n"
             << "  -posx x       set surface x position on layer to x (default " << DEFAULT_POS_X << ")\n"
             << "  -posy x       set surface y position on layer to x (default " << DEFAULT_POS_Y << ")\n"
             << "  -opacity x    set opacity of surface to x (default " << DEFAULT_OPACITY << ")\n\n";
    }
    else
    {
        SurfaceConfiguration config;
        config.layerId = layerId.get();
        config.surfaceId = surfaceId.get();
        config.surfaceWidth = width.get();
        config.surfaceHeight = height.get();
        config.surfacePosX = posx.get();
        config.surfacePosY = posy.get();
        config.opacity = opacity.get();
        config.nosky = nosky.get();

        MockNavi navi(fps.get(), animSpeed.get(), &config);
        navi.mainloop();
    }

    return 0;
}

