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
using namespace std;

//=============================================================================
// common helper functions
//=============================================================================

void printArray(const char* text, unsigned int* array, int count)
{
    cout << count << " " << text << "(s):\n";
    for (int i = 0; i < count; ++i)
    {
        cout << "- " << text << " "
            << dec << array[i]
            << hex << " (0x" << array[i] << ")"
            << dec << "\n";
    }
}

void printScreenProperties(unsigned int screenid, const char* prefix = "")
{
    cout << prefix << "screen " << screenid << " (0x" << hex << screenid << dec << ")\n";
    cout << prefix << "---------------------------------------\n";

    unsigned int resolutionX = 0;
    unsigned int resolutionY = 0;
    ilm_getScreenResolution(screenid, &resolutionX, &resolutionY);
    cout << prefix << "- resolution:           x=" << resolutionX << ", y=" << resolutionY << "\n";

    unsigned int hwLayerCount = 0;
    ilm_getNumberOfHardwareLayers(screenid, &hwLayerCount);
    cout << prefix << "- hardware layer count: " << hwLayerCount << "\n";

    cout << prefix << "- layer render order:   ";
    int layerCount = 0;
    unsigned int* layerArray = NULL;
    ilm_getLayerIDsOnScreen(screenid, &layerCount, &layerArray);
    for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex)
    {
        unsigned int layerid = layerArray[layerIndex];
        cout << layerid << "(0x" << hex << layerid << dec << "), ";
    }
    cout << "\n";
}

void printLayerProperties(unsigned int layerid, const char* prefix = "")
{
    cout << prefix << "layer " << layerid << " (0x" << hex << layerid << dec << ")\n";
    cout << prefix << "---------------------------------------\n";

    ilmLayerProperties p;
    ilm_getPropertiesOfLayer(layerid, &p);

    cout << prefix << "- original size:        x=" << p.origSourceWidth << ", y=" << p.origSourceHeight << "\n";
    cout << prefix << "- destination region:   x=" << p.destX << ", y=" << p.destY<< ", w=" << p.destWidth<< ", h=" << p.destHeight << "\n";
    cout << prefix << "- source region:        x=" << p.sourceX << ", y=" << p.sourceY<< ", w=" << p.sourceWidth<< ", h=" << p.sourceHeight << "\n";

    cout << prefix << "- orientation:          " << p.orientation << " (";
    switch (p.orientation)
    {
    case 0/*Zero*/:
        cout << "up is top)\n";
        break;
    case 1/*Ninety*/:
        cout << "up is right)\n";
        break;
    case 2/*OneEighty*/:
        cout << "up is bottom)\n";
        break;
    case 3/*TwoSeventy*/:
        cout << "up is left)\n";
        break;
    default:
        cout << "unknown)\n";
        break;
    }

    cout << prefix << "- opacity:              " << p.opacity << "\n";
    cout << prefix << "- visibility:           " << p.visibility << "\n";

    cout << prefix << "- type:                 " << p.type << " (";
    switch (p.type)
    {
    case 1/*Hardware*/:
        cout << "hardware)\n";
        break;
    case 2/*Software_2D*/:
        cout << "software 2d)\n";
        break;
    case 3/*Software_2_5D*/:
        cout << "software 2.5d)\n";
        break;
    default:
        cout << "unknown)\n";
        break;
    }

    cout << prefix << "- chromakey:            "
         << (p.chromaKeyEnabled ? "enabled " : "disabled ")
         << "(r=" << p.chromaKeyRed << ", g=" << p.chromaKeyGreen << ", b=" << p.chromaKeyBlue << ")\n";

    cout << prefix << "- surface render order: ";
    int surfaceCount = 0;
    unsigned int* surfaceArray = NULL;
    ilm_getSurfaceIDsOnLayer(layerid, &surfaceCount, &surfaceArray);
    for (int surfaceIndex = 0; surfaceIndex < surfaceCount; ++surfaceIndex)
    {
        cout << surfaceArray[surfaceIndex] << "(0x" << hex << surfaceArray[surfaceIndex] << dec << "), ";
    }
    cout << "\n";

    cout << prefix << "- on screen:            ";
    unsigned int screenCount = 0;
    unsigned int* screenArray = NULL;
    ilm_getScreenIDs(&screenCount, &screenArray);
    for (unsigned int screenIndex = 0; screenIndex < screenCount; ++screenIndex)
    {
        unsigned int screenid = screenArray[screenIndex];
        int layerCount = 0;
        unsigned int* layerArray = NULL;
        ilm_getLayerIDsOnScreen(screenid, &layerCount, &layerArray);
        for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex)
        {
            unsigned int id = layerArray[layerIndex];
            if (id == layerid)
            {
                cout << screenid << "(0x" << hex << screenid << dec << ") ";
            }
        }
    }
    cout << "\n";

    // TODO: print layer groups, this layer is part of
}

void printSurfaceProperties(unsigned int surfaceid, const char* prefix = "")
{
    cout << prefix << "surface " << surfaceid << " (0x" << hex << surfaceid << dec << ")\n";
    cout << prefix << "---------------------------------------\n";

    ilmSurfaceProperties p;
    ilm_getPropertiesOfSurface(surfaceid, &p);

    cout << prefix << "- original size:      x=" << p.origSourceWidth << ", y=" << p.origSourceHeight << "\n";
    cout << prefix << "- destination region: x=" << p.destX << ", y=" << p.destY<< ", w=" << p.destWidth<< ", h=" << p.destHeight << "\n";
    cout << prefix << "- source region:      x=" << p.sourceX << ", y=" << p.sourceY<< ", w=" << p.sourceWidth<< ", h=" << p.sourceHeight << "\n";

    cout << prefix << "- orientation:        " << p.orientation << " (";
    switch (p.orientation)
    {
    case 0/*Zero*/:
        cout << "up is top)\n";
        break;
    case 1/*Ninety*/:
        cout << "up is right)\n";
        break;
    case 2/*OneEighty*/:
        cout << "up is bottom)\n";
        break;
    case 3/*TwoSeventy*/:
        cout << "up is left)\n";
        break;
    default:
        cout << "unknown)\n";
        break;
    }

    cout << prefix << "- opacity:            " << p.opacity << "\n";
    cout << prefix << "- visibility:         " << p.visibility << "\n";

    cout << prefix << "- pixel format:       " << p.pixelformat << " (";
    switch (p.pixelformat)
    {
    case 0/*PIXELFORMAT_R8*/:
        cout << "R-8)\n";
        break;
    case 1/*PIXELFORMAT_RGB888*/:
        cout << "RGB-888)\n";
        break;
    case 2/*PIXELFORMAT_RGBA8888*/:
        cout << "RGBA-8888)\n";
        break;
    case 3/*PIXELFORMAT_RGB565*/:
        cout << "RGB-565)\n";
        break;
    case 4/*PIXELFORMAT_RGBA5551*/:
        cout << "RGBA-5551)\n";
        break;
    case 5/*PIXELFORMAT_RGBA6661*/:
        cout << "RGBA-6661)\n";
        break;
    case 6/*PIXELFORMAT_RGBA4444*/:
        cout << "RGBA-4444)\n";
        break;
    default:
        cout << "unknown)\n";
        break;
    }

    cout << prefix << "- native surface:     " << p.nativeSurface << "\n";

    cout << prefix << "- accepts input from: "
         << (p.inputDevicesAcceptance & ILM_INPUT_DEVICE_KEYBOARD ? "keyboard " : "")
         << (p.inputDevicesAcceptance & ILM_INPUT_DEVICE_POINTER ? "mouse " : "")
         << (p.inputDevicesAcceptance & ILM_INPUT_DEVICE_TOUCH ? "touch " : "")
         << "\n";

    t_ilm_surface keyboardFocusSurfaceId;
    ilm_GetKeyboardFocusSurfaceId(&keyboardFocusSurfaceId);

    cout << prefix << "- has keyboard focus: "
         << (keyboardFocusSurfaceId == surfaceid ? "true" : "false")
         << "\n";

    cout << prefix << "- chromakey:          "
         << (p.chromaKeyEnabled ? "enabled " : "disabled ")
         << "(r=" << p.chromaKeyRed << ", g=" << p.chromaKeyGreen << ", b=" << p.chromaKeyBlue << ")\n";

    cout << prefix << "- counters:           frame=" << p.frameCounter
                                       << ", draw=" << p.drawCounter
                                       << ", update=" << p.updateCounter << "\n";

    cout << prefix << "- on layer:           ";
    int layerCount = 0;
    unsigned int* layerArray = NULL;
    ilm_getLayerIDs(&layerCount, &layerArray);
    for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex)
    {
        unsigned int layerid = layerArray[layerIndex];
        int surfaceCount = 0;
        unsigned int* surfaceArray = NULL;
        ilm_getSurfaceIDsOnLayer(layerid, &surfaceCount, &surfaceArray);
        for (int surfaceIndex = 0; surfaceIndex < surfaceCount; ++surfaceIndex)
        {
            unsigned int id = surfaceArray[surfaceIndex];
            if (id == surfaceid)
            {
                cout << layerid << "(0x" << hex << layerid << dec << ") ";
            }
        }
    }
    cout << "\n";

    // TODO: print surface groups, this surface is part of
}
