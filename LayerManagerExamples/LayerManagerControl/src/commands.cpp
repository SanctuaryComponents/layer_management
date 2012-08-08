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
#include "Expression.h"
#include "ExpressionInterpreter.h"
#include <iostream>
#include <cstring>
#include <signal.h> // signal

using namespace std;

//=============================================================================
// common helper functions
//=============================================================================

#define COMMAND(text) COMMAND2(__COUNTER__,text)

#define COMMAND2(x,y) COMMAND3(x,y)

#define COMMAND3(funcNumber, text) \
    void func_ ## funcNumber(Expression* input); \
    static const bool reg_ ## funcNumber = \
        ExpressionInterpreter::addExpression(func_ ## funcNumber, text); \
    void func_ ## funcNumber(Expression* input)

void printArray(const char* text, unsigned int* array, int count);
void printScreenProperties(unsigned int screenid, const char* prefix = "");
void printLayerProperties(unsigned int layerid, const char* prefix = "");
void printSurfaceProperties(unsigned int surfaceid, const char* prefix = "");

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
COMMAND("get screens")
//=============================================================================
{
    (void)input;
    unsigned int count = 0;
    unsigned int* array = NULL;
    ilm_getScreenIDs(&count, &array);
    printArray("Screen", array, count);
}

//=============================================================================
COMMAND("get layers")
//=============================================================================
{
    (void)input;
    int count = 0;
    unsigned int* array = NULL;
    ilm_getLayerIDs(&count, &array);
    printArray("Layer", array, count);
}

//=============================================================================
COMMAND("get surfaces")
//=============================================================================
{
    (void)input;
    int count = 0;
    unsigned int* array = NULL;
    ilm_getSurfaceIDs(&count, &array);
    printArray("Surface", array, count);
}

//=============================================================================
COMMAND("get screen <screenid>")
//=============================================================================
{
    printScreenProperties(input->getUint("screenid"));
}

//=============================================================================
COMMAND("get layer <layerid>")
//=============================================================================
{
    printLayerProperties(input->getUint("layerid"));
}

//=============================================================================
COMMAND("get surface <surfaceid>")
//=============================================================================
{
    printSurfaceProperties(input->getUint("surfaceid"));
}

//=============================================================================
COMMAND("dump screen <screenid> to <file>")
//=============================================================================
{
    ilm_takeScreenshot(input->getUint("screenid"),
                       input->getString("file").c_str());
}

//=============================================================================
COMMAND("dump layer <layerid> to <file>")
//=============================================================================
{
    ilm_takeLayerScreenshot(input->getString("file").c_str(),
                            input->getUint("layerid"));
}

//=============================================================================
COMMAND("dump surface <surfaceid> to <file>")
//=============================================================================
{
    ilm_takeSurfaceScreenshot(input->getString("file").c_str(),
                              input->getUint("surfaceid"));
}

//=============================================================================
COMMAND("set layer <layerid> source region <x> <y> <w> <h>")
//=============================================================================
{
    ilm_layerSetSourceRectangle(input->getUint("layerid"),
                                input->getUint("x"),
                                input->getUint("y"),
                                input->getUint("w"),
                                input->getUint("h"));
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set surface <surfaceid> source region <x> <y> <w> <h>")
//=============================================================================
{
    ilm_surfaceSetSourceRectangle(input->getUint("surfaceid"),
                                  input->getUint("x"),
                                  input->getUint("y"),
                                  input->getUint("w"),
                                  input->getUint("h"));
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set layer <layerid> destination region <x> <y> <w> <h>")
//=============================================================================
{
    ilm_layerSetDestinationRectangle(input->getUint("layerid"),
                                     input->getUint("x"),
                                     input->getUint("y"),
                                     input->getUint("w"),
                                     input->getUint("h"));
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set surface <surfaceid> destination region <x> <y> <w> <h>")
//=============================================================================
{
    ilm_surfaceSetDestinationRectangle(input->getUint("surfaceid"),
                                       input->getUint("x"),
                                       input->getUint("y"),
                                       input->getUint("w"),
                                       input->getUint("h"));
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set layer <layerid> opacity <opacity>")
//=============================================================================
{
    ilm_layerSetOpacity(input->getUint("layerid"),
                        input->getDouble("opacity"));
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set surface <surfaceid> opacity <opacity>")
//=============================================================================
{
    ilm_surfaceSetOpacity(input->getUint("surfaceid"),
                          input->getDouble("opacity"));
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set layer <layerid> visibility <visibility>")
//=============================================================================
{
    ilm_layerSetVisibility(input->getUint("layerid"),
                           input->getBool("visibility"));
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set surface <surfaceid> visibility <visibility>")
//=============================================================================
{
    ilm_surfaceSetVisibility(input->getUint("surfaceid"),
                             input->getBool("visibility"));
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set layer <layerid> orientation <orientation>")
//=============================================================================
{
    ilm_layerSetOrientation(input->getUint("layerid"),
            (ilmOrientation)input->getInt("orientation"));
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set surface <surfaceid> orientation <orientation>")
//=============================================================================
{
    ilm_surfaceSetOrientation(input->getUint("surfaceid"),
              (ilmOrientation)input->getInt("orientation"));
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set screen <screenid> render order <layeridarray>")
//=============================================================================
{
    unsigned int count = 0;
    unsigned int* array = NULL;
    unsigned int screenid = input->getUint("screenid");
    input->getUintArray("layeridarray", &array, &count);
    ilm_displaySetRenderOrder(screenid, array, count);
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set screen <screenid> render order")
//=============================================================================
{
    unsigned int screenid = input->getUint("screenid");
    ilm_displaySetRenderOrder(screenid, NULL, 0);
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set layer <layerid> render order <surfaceidarray>")
//=============================================================================
{
    unsigned int count = 0;
    unsigned int* array = NULL;
    unsigned int layerid = input->getUint("layerid");
    input->getUintArray("surfaceidarray", &array, &count);
    ilm_layerSetRenderOrder(layerid, array, count);
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set layer <layerid> render order")
//=============================================================================
{
    unsigned int layerid = input->getUint("layerid");
    ilm_layerSetRenderOrder(layerid, NULL, 0);
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set layer <layerid> width <width>")
//=============================================================================
{
    unsigned int dimension[2];
    unsigned int layerid = input->getUint("layerid");
    ilm_layerGetDimension(layerid, dimension);
    dimension[0] = input->getUint("width");
    ilm_layerSetDimension(layerid, dimension);
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set surface <surfaceid> width <width>")
//=============================================================================
{
    unsigned int dimension[2];
    unsigned int surfaceid = input->getUint("layerid");
    ilm_surfaceGetDimension(surfaceid, dimension);
    dimension[0] = input->getUint("width");
    ilm_surfaceSetDimension(surfaceid, dimension);
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set layer <layerid> height <height>")
//=============================================================================
{
    unsigned int count = 0;
    unsigned int dimension[2];
    unsigned int layerid = input->getUint("layerid");
    ilm_layerGetDimension(layerid, dimension);
    dimension[1] = input->getUint("height");
    ilm_layerSetDimension(layerid, dimension);
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set surface <surfaceid> height <height>")
//=============================================================================
{
    unsigned int count = 0;
    unsigned int dimension[2];
    unsigned int surfaceid = input->getUint("surfaceid");
    ilm_surfaceGetDimension(surfaceid, dimension);
    dimension[1] = input->getUint("height");
    ilm_surfaceSetDimension(surfaceid, dimension);
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set layer <layerid> position <x> <y>")
//=============================================================================
{
    unsigned int dimension[2];
    unsigned int layerid = input->getUint("layerid");
    dimension[0] = input->getUint("x");
    dimension[1] = input->getUint("y");
    ilm_layerSetPosition(layerid, dimension);
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set surface <surfaceid> position <x> <y>")
//=============================================================================
{
    unsigned int dimension[2];
    unsigned int surfaceid = input->getUint("surfaceid");
    dimension[0] = input->getUint("x");
    dimension[1] = input->getUint("y");
    ilm_surfaceSetPosition(surfaceid, dimension);
    ilm_commitChanges();
}

//=============================================================================
COMMAND("create layer <layerid>")
//=============================================================================
{
    unsigned int layerid = input->getUint("layerid");
    ilm_layerCreate(&layerid);
}

//=============================================================================
COMMAND("create layer <layerid> <width> <height>")
//=============================================================================
{
    unsigned int layerid = input->getUint("layerid");
    unsigned int width = input->getUint("width");
    unsigned int height = input->getUint("height");
    ilm_layerCreateWithDimension(&layerid, width, height);
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
COMMAND("destroy layer <layerid>")
//=============================================================================
{
    unsigned int layerid = input->getUint("layerid");
    ilm_layerRemove(layerid);
}

//=============================================================================
COMMAND("destroy surface <surfaceid>")
//=============================================================================
{
    unsigned int surfaceid = input->getUint("surfaceid");
    ilm_surfaceRemove(surfaceid);
}

//=============================================================================
COMMAND("get scene")
//=============================================================================
{
    unsigned int screenCount = 0;
    unsigned int* screenArray = NULL;

    ilm_getScreenIDs(&screenCount, &screenArray);

    for (unsigned int screenIndex = 0; screenIndex < screenCount; ++screenIndex)
    {
        unsigned int screenid = screenArray[screenIndex];
        printScreenProperties(screenid);
        cout << "\n";

        int layerCount = 0;
        unsigned int* layerArray = NULL;
        ilm_getLayerIDsOnScreen(screenid, &layerCount, &layerArray);
        for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex)
        {
            unsigned int layerid = layerArray[layerIndex];
            printLayerProperties(layerid, "    ");
            cout << "\n";

            int surfaceCount = 0;
            unsigned int* surfaceArray = NULL;
            ilm_getSurfaceIDsOnLayer(layerid, &surfaceCount, &surfaceArray);
            for (int surfaceIndex = 0; surfaceIndex < surfaceCount; ++surfaceIndex)
            {
                unsigned int surfaceid = surfaceArray[surfaceIndex];
                printSurfaceProperties(surfaceid, "        ");
                cout << "\n";
            }
        }
    }
}

//=============================================================================
bool gBenchmark_running;

void benchmarkSigHandler(int sig)
{
    gBenchmark_running = false;
}

COMMAND("get communicator performance")
//=============================================================================
{
    int runs = 0;
    int runtimeInSec = 5;
    unsigned int hwLayerCnt = 0;
    cout << "running performance test for " << runtimeInSec << " seconds... ";
    flush(cout);

    signal(SIGALRM, benchmarkSigHandler);

    gBenchmark_running = true;

    alarm(runtimeInSec);

    while (gBenchmark_running)
    {
        ilm_getNumberOfHardwareLayers(0, &hwLayerCnt);
        ++runs;
    }

    signal(SIGALRM, SIG_DFL);

    cout << (runs/runtimeInSec) << " transactions/second\n";
}

//=============================================================================
COMMAND("set surface <surfaceid> keyboard focus")
//=============================================================================
{
    if (ilm_SetKeyboardFocusOn(input->getUint("surfaceid")) != ILM_SUCCESS)
    {
        cerr << "Error during communication" << endl;
    }
}

//=============================================================================
COMMAND("get keyboard focus")
//=============================================================================
{
    t_ilm_surface surfaceId;
    if (ilm_GetKeyboardFocusSurfaceId(&surfaceId) == ILM_SUCCESS)
    {
        cout << "keyboardFocusSurfaceId == " << surfaceId << endl;
    }
    else
    {
        cerr << "Error during communication" << endl;
    }
}

//=============================================================================
COMMAND("set surface <surfaceid> accept <acceptance> input events from devices <kbd:pointer:touch>")
//=============================================================================
{
    t_ilm_surface surfaceId;
    ilmInputDevice devices;
    t_ilm_bool acceptance;
    char* str;
    char* tok;

    devices = (ilmInputDevice)0;
    surfaceId = input->getUint("surfaceid");
    acceptance = input->getBool("acceptance");
    str = new char [input->getString("kbd:pointer:touch").size()+1];
    strcpy (str, input->getString("kbd:pointer:touch").c_str());
    tok = strtok(str, ":");
    while (tok != NULL)
    {
        if (!strcmp(tok, "kbd"))
        {
            devices |= ILM_INPUT_DEVICE_KEYBOARD;
        }
        else if (!strcmp(tok, "pointer"))
        {
            devices |= ILM_INPUT_DEVICE_POINTER;
        }
        else if (!strcmp(tok, "touch"))
        {
          devices |= ILM_INPUT_DEVICE_TOUCH;
        }
        else
        {
          cerr << "Unknown devices specified." << endl;
        }
        tok = strtok(NULL, ":");
    }
    ilm_UpdateInputEventAcceptanceOn(surfaceId, devices, acceptance);
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set surface <surfaceid> chromakey <red> <green> <blue>")
//=============================================================================
{
    t_ilm_surface surface = input->getUint("surfaceid");
    t_ilm_int color[3] =
    {
        input->getInt("red"),
        input->getInt("green"),
        input->getInt("blue")
    };

    ilm_surfaceSetChromaKey(surface, color);
    ilm_commitChanges();
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
COMMAND("set layer <layerid> chromakey <red> <green> <blue>")
//=============================================================================
{
    t_ilm_surface surface = input->getUint("layerid");
    t_ilm_int color[3] =
    {
        input->getInt("red"),
        input->getInt("green"),
        input->getInt("blue")
    };

    ilm_layerSetChromaKey(surface, color);
    ilm_commitChanges();
}

//=============================================================================
COMMAND("set layer <layerid> chromakey disabled")
//=============================================================================
{
    t_ilm_surface surface = input->getUint("layerid");
    ilm_layerSetChromaKey(surface, NULL);
    ilm_commitChanges();
}


