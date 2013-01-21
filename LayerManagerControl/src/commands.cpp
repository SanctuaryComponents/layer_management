/***************************************************************************
 *
 * Copyright 2012 BMW Car IT GmbH
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either inputess or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/
#include "ilm_client.h"
#include "LMControl.h"
#include "Expression.h"
#include "ExpressionInterpreter.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <signal.h> // signal
#include <unistd.h> // alarm

using namespace std;


#define COMMAND(text) COMMAND2(__COUNTER__,text)

#define COMMAND2(x,y) COMMAND3(x,y)

#define COMMAND3(funcNumber, text) \
    void func_ ## funcNumber(Expression* input); \
    static const bool reg_ ## funcNumber = \
        ExpressionInterpreter::addExpression(func_ ## funcNumber, text); \
    void func_ ## funcNumber(Expression* input)



//=============================================================================
COMMAND("help")
//=============================================================================
{
    (void)input;
    cout << "help: supported commands:\n\n";
    ExpressionInterpreter::printExpressionList();
    cout << "\n";
}

//=============================================================================
COMMAND("tree")
//=============================================================================
{
    (void)input;
    cout << "help: supported commands:\n\n";
    ExpressionInterpreter::printExpressionTree();
    cout << "\n";
}

//=============================================================================
COMMAND("get scene|screens|layers|surfaces")
//=============================================================================
{
    if (input->contains("scene"))
    {
        printScene();
    }
    else if (input->contains("screens"))
    {
        (void)input;
        unsigned int count = 0;
        unsigned int* array = NULL;
        ilm_getScreenIDs(&count, &array);
        printArray("Screen", array, count);
    }
    else if (input->contains("layers"))
    {
        (void)input;
        int count = 0;
        unsigned int* array = NULL;
        ilm_getLayerIDs(&count, &array);
        printArray("Layer", array, count);
    }
    else if (input->contains("surfaces"))
    {
        (void)input;
        int count = 0;
        unsigned int* array = NULL;
        ilm_getSurfaceIDs(&count, &array);
        printArray("Surface", array, count);
    }
}

//=============================================================================
COMMAND("get screen|layer|surface <id>")
//=============================================================================
{
    if (input->contains("screen"))
    {
        printScreenProperties(input->getUint("id"));
    }
    else if (input->contains("layer"))
    {
        printLayerProperties(input->getUint("id"));
    }
    else if (input->contains("surface"))
    {
        printSurfaceProperties(input->getUint("id"));
    }
}

//=============================================================================
COMMAND("dump screen|layer|surface <id> to <file>")
//=============================================================================
{
    if (input->contains("screen"))
    {
        ilm_takeScreenshot(input->getUint("id"),
                           input->getString("file").c_str());
    }
    else if (input->contains("layer"))
    {
        ilm_takeLayerScreenshot(input->getString("file").c_str(),
                                input->getUint("id"));
    }
    else if (input->contains("surface"))
    {
        ilm_takeSurfaceScreenshot(input->getString("file").c_str(),
                                  input->getUint("id"));
    }
}

//=============================================================================
COMMAND("set layer|surface <id> source region <x> <y> <w> <h>")
//=============================================================================
{
    if (input->contains("layer"))
    {
        ilm_layerSetSourceRectangle(input->getUint("id"),
                                    input->getUint("x"),
                                    input->getUint("y"),
                                    input->getUint("w"),
                                    input->getUint("h"));
        ilm_commitChanges();
    }
    else if (input->contains("surface"))
    {
        ilm_surfaceSetSourceRectangle(input->getUint("id"),
                                      input->getUint("x"),
                                      input->getUint("y"),
                                      input->getUint("w"),
                                      input->getUint("h"));
            ilm_commitChanges();
    }
}

//=============================================================================
COMMAND("set layer|surface <id> destination region <x> <y> <w> <h>")
//=============================================================================
{
    if (input->contains("layer"))
    {
        ilm_layerSetDestinationRectangle(input->getUint("id"),
                                        input->getUint("x"),
                                        input->getUint("y"),
                                        input->getUint("w"),
                                        input->getUint("h"));
        ilm_commitChanges();
    }
    else if (input->contains("surface"))
    {
        ilm_surfaceSetDestinationRectangle(input->getUint("id"),
                                           input->getUint("x"),
                                           input->getUint("y"),
                                           input->getUint("w"),
                                           input->getUint("h"));
        ilm_commitChanges();
    }
}

//=============================================================================
COMMAND("set layer|surface <id> opacity <opacity>")
//=============================================================================
{
    if (input->contains("layer"))
    {
        ilm_layerSetOpacity(input->getUint("id"),
                input->getDouble("opacity"));
        ilm_commitChanges();
    }
    else if (input->contains("surface"))
    {
        ilm_surfaceSetOpacity(input->getUint("id"),
                                  input->getDouble("opacity"));
            ilm_commitChanges();
    }
}

//=============================================================================
COMMAND("set layer|surface <id> visibility <visibility>")
//=============================================================================
{
    if (input->contains("layer"))
    {
        ilm_layerSetVisibility(input->getUint("id"),
                               input->getBool("visibility"));
        ilm_commitChanges();
    }
    else if (input->contains("surface"))
    {
        ilm_surfaceSetVisibility(input->getUint("id"),
                                 input->getBool("visibility"));
        ilm_commitChanges();
    }
}

//=============================================================================
COMMAND("set layer|surface <id> orientation <orientation>")
//=============================================================================
{
    if (input->contains("layer"))
    {
        ilm_layerSetOrientation(input->getUint("id"),
                    (ilmOrientation)input->getInt("orientation"));
        ilm_commitChanges();
    }
    else if (input->contains("surface"))
    {
        ilm_surfaceSetOrientation(input->getUint("id"),
                      (ilmOrientation)input->getInt("orientation"));
        ilm_commitChanges();
    }

}

//=============================================================================
COMMAND("set screen|layer <id> render order [<idarray>]")
//=============================================================================
{
    if (input->contains("screen"))
    {
        if (input->contains("idarray"))
        {
            unsigned int count = 0;
            unsigned int* array = NULL;
            unsigned int screenid = input->getUint("id");
            input->getUintArray("idarray", &array, &count);
            ilm_displaySetRenderOrder(screenid, array, count);
            ilm_commitChanges();
        }
        else
        {
            unsigned int screenid = input->getUint("id");
            ilm_displaySetRenderOrder(screenid, NULL, 0);
            ilm_commitChanges();
        }
    }
    else if (input->contains("layer"))
    {
        if (input->contains("idarray"))
        {
            unsigned int count = 0;
            unsigned int* array = NULL;
            unsigned int layerid = input->getUint("id");
            input->getUintArray("idarray", &array, &count);
            ilm_layerSetRenderOrder(layerid, array, count);
            ilm_commitChanges();
        }
        else
        {
            unsigned int layerid = input->getUint("id");
            ilm_layerSetRenderOrder(layerid, NULL, 0);
            ilm_commitChanges();
        }
    }
}

//=============================================================================
COMMAND("set layer|surface <id> width <width>")
//=============================================================================
{
    if (input->contains("layer"))
    {
        unsigned int dimension[2];
        unsigned int layerid = input->getUint("id");
        ilm_layerGetDimension(layerid, dimension);
        dimension[0] = input->getUint("width");
        ilm_layerSetDimension(layerid, dimension);
        ilm_commitChanges();
    }
    else if (input->contains("surface"))
    {
        unsigned int dimension[2];
        unsigned int surfaceid = input->getUint("id");
        ilm_surfaceGetDimension(surfaceid, dimension);
        dimension[0] = input->getUint("width");
        ilm_surfaceSetDimension(surfaceid, dimension);
        ilm_commitChanges();
    }
}

//=============================================================================
COMMAND("set layer|surface <id> height <height>")
//=============================================================================
{
    if (input->contains("layer"))
    {
        unsigned int dimension[2];
        unsigned int layerid = input->getUint("id");
        ilm_layerGetDimension(layerid, dimension);
        dimension[1] = input->getUint("height");
        ilm_layerSetDimension(layerid, dimension);
        ilm_commitChanges();
    }
    else if (input->contains("surface"))
    {
        unsigned int dimension[2];
        unsigned int surfaceid = input->getUint("id");
        ilm_surfaceGetDimension(surfaceid, dimension);
        dimension[1] = input->getUint("height");
        ilm_surfaceSetDimension(surfaceid, dimension);
        ilm_commitChanges();
    }
}

//=============================================================================
COMMAND("set layer|surface <id> position <x> <y>")
//=============================================================================
{
    if (input->contains("layer"))
    {
        unsigned int dimension[2];
        unsigned int layerid = input->getUint("id");
        dimension[0] = input->getUint("x");
        dimension[1] = input->getUint("y");
        ilm_layerSetPosition(layerid, dimension);
        ilm_commitChanges();
    }
    else if (input->contains("surface"))
    {
        unsigned int dimension[2];
        unsigned int surfaceid = input->getUint("id");
        dimension[0] = input->getUint("x");
        dimension[1] = input->getUint("y");
        ilm_surfaceSetPosition(surfaceid, dimension);
        ilm_commitChanges();
    }
}

//=============================================================================
COMMAND("create layer <layerid> [<width> <height>]")
//=============================================================================
{
    if (input->contains("width") && input->contains("height"))
    {
        unsigned int layerid = input->getUint("layerid");
        unsigned int width = input->getUint("width");
        unsigned int height = input->getUint("height");
        ilm_layerCreateWithDimension(&layerid, width, height);
    }
    else
    {
        unsigned int layerid = input->getUint("layerid");
        ilm_layerCreate(&layerid);
    }
}

//=============================================================================
COMMAND("create surface <surfaceid> <nativehandle> <width> <height> <pixelformat>")
//=============================================================================
{
    unsigned int surfaceid = input->getUint("surfaceid");
    unsigned int nativeHandle = input->getUint("nativehandle");
    unsigned int width = input->getUint("width");
    unsigned int height = input->getUint("height");
    e_ilmPixelFormat pixelformat = (e_ilmPixelFormat)input->getUint("pixelformat");
    ilm_surfaceCreate(nativeHandle, width, height, pixelformat, &surfaceid);
}

//=============================================================================
COMMAND("destroy layer|surface <id>")
//=============================================================================
{
    if (input->contains("layer"))
    {
        unsigned int layerid = input->getUint("id");
        ilm_layerRemove(layerid);
    }
    else if (input->contains("surface"))
    {
        unsigned int surfaceid = input->getUint("id");
        ilm_surfaceRemove(surfaceid);
    }
}

//=============================================================================
COMMAND("get communicator performance")
//=============================================================================
{
    (void) input; //suppress warning: unused parameter
    getCommunicatorPerformance();
}

//=============================================================================
COMMAND("set surface <surfaceid> keyboard focus")
//=============================================================================
{
    t_ilm_surface surface = input->getUint("surfaceid");

    setSurfaceKeyboardFocus(surface);
}

//=============================================================================
COMMAND("get keyboard focus")
//=============================================================================
{
    (void) input; //suppress warning: unused parameter
    getKeyboardFocus();
}

//=============================================================================
COMMAND("set surface <surfaceid> accept <acceptance> input events from devices <kbd:pointer:touch>")
//=============================================================================
{
    t_ilm_surface surfaceId = input->getUint("surfaceid");
    t_ilm_bool acceptance = input->getBool("acceptance");
    string kbdPointerTouch = input->getString("kbd:pointer:touch");

    setSurfaceAcceptsInput(surfaceId, kbdPointerTouch, acceptance);
}

//=============================================================================
COMMAND("set layer|surface <id> chromakey <red> <green> <blue>")
//=============================================================================
{
    if (input->contains("layer"))
    {
        t_ilm_layer layer = input->getUint("id");
        t_ilm_int color[3] =
        {
            input->getInt("red"),
            input->getInt("green"),
            input->getInt("blue")
        };

        ilm_layerSetChromaKey(layer, color);
        ilm_commitChanges();
    }
    else if (input->contains("surface"))
    {
        t_ilm_surface surface = input->getUint("id");
        t_ilm_int color[3] =
        {
            input->getInt("red"),
            input->getInt("green"),
            input->getInt("blue")
        };
        ilm_surfaceSetChromaKey(surface, color);
        ilm_commitChanges();
    }
}

//=============================================================================
COMMAND("set surface <surfaceid> chromakey disabled")
//=============================================================================
{
    t_ilm_surface surface = input->getUint("surfaceid");
    ilm_surfaceSetChromaKey(surface, NULL);
    ilm_commitChanges();
}

//=============================================================================
COMMAND("test notification layer <layerid>")
//=============================================================================
{
    unsigned int layerid = input->getUint("layerid");

    testNotificationLayer(layerid);
}

//=============================================================================
COMMAND("watch layer|surface <idarray>")
//=============================================================================
{
    if (input->contains("layer"))
    {
        unsigned int* layerids = NULL;
        unsigned int layeridCount;
        input->getUintArray("idarray", &layerids, &layeridCount);

        watchLayer(layerids, layeridCount);
    }
    else if (input->contains("surface"))
    {
        unsigned int* surfaceids = NULL;
        unsigned int surfaceidCount;
        input->getUintArray("idarray", &surfaceids, &surfaceidCount);

        watchSurface(surfaceids, surfaceidCount);
    }
}

//=============================================================================
COMMAND("set optimization <id> mode <mode>")
//=============================================================================
{
    t_ilm_uint id = input->getUint("id");
    t_ilm_uint mode = input->getUint("mode");
    setOptimization(id, mode);
}

//=============================================================================
COMMAND("get optimization <id>")
//=============================================================================
{
    t_ilm_uint id = input->getUint("id");
    getOptimization(id);
}

//=============================================================================
COMMAND("analyze surface <surfaceid>")
//=============================================================================
{
    t_ilm_surface targetSurfaceId = (t_ilm_uint) input->getUint("surfaceid");
    analyzeSurface(targetSurfaceId);
}

//=============================================================================
COMMAND("scatter [all]")
//=============================================================================
{
    if (input->contains("all"))
    {
        scatterAll();
    }
    else
    {
        scatter();
    }
}

//=============================================================================
COMMAND("demo [<animation_mode=2>]")
//=============================================================================
{
    t_ilm_uint mode = (t_ilm_uint) input->getUint("animation_mode");
    demo(mode);
}

//=============================================================================
COMMAND("export scene to <filename>")
//=============================================================================
{
    string filename = (string) input->getString("filename");
    exportSceneToFile(filename);
}

//=============================================================================
COMMAND("export xtext to <filename> <grammar> <url>")
//=============================================================================
{
    string filename = (string) input->getString("filename");
    string grammar = (string) input->getString("grammar");
    string url = (string) input->getString("url");
    exportXtext(filename, grammar, url);
}

//=============================================================================
COMMAND("import scene from <filename>")
//=============================================================================
{
    string filename = (string) input->getString("filename");
    importSceneFromFile(filename);
}
