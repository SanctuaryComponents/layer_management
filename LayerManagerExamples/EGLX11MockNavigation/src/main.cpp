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

#include <iostream>
using std::cout;

int main (int argc, const char * argv[])
{
    float defaultFps = 15.0f;
    float defaultAnim = 1.5f;
    int defaultHouses = 15.0f;
    
    FloatArgument fps("fps", defaultFps, argc, argv);
    FloatArgument animSpeed("anim", defaultAnim, argc, argv);
    //IntArgument houses("houses", defaultHouses, argc, argv);
    BoolArgument help("help", false, argc, argv);
    
    if (help.get())
    {
        cout << "Usage: " << argv[0] << " [options]\n"
             << "possible options are:\n"
             << "  -help         show this help text\n"
             << "  -fps x        run with x frames per second (default " << defaultFps << ")\n"
             << "  -anim x       set animation speed (default " << defaultAnim << ")\n";
             //<< "  -houses x     generate x houses (default " << defaultHouses << ")\n";
    }
    else
    {
        MockNavi navi(fps.get(), animSpeed.get(), defaultHouses);
        navi.mainloop();
    }

    return 0;
}

