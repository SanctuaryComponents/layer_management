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
#include "LMControl.h"
#include <cstring>
#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <map>
#include <vector>


using namespace std;

//=============================================================================
// common helper functions
//=============================================================================

void printArray(const char* text, unsigned int* array, int count)
{
    cout << count << " " << text << "(s):\n";
    for (int i = 0; i < count; ++i)
    {
        cout << "- " << text << " " << dec << array[i] << hex << " (0x"
                << array[i] << ")" << dec << "\n";
    }
}

template<typename T>
void printArray(const char* text, T* array, int count)
{
    cout << count << " " << text << "(s):\n";
    for (int i = 0; i < count; ++i)
    {
        cout << "- " << text << " " << "[" << array[i] << "]" << "\n";
    }
}

template<typename T>
void printVector(const char* text, vector<T> v)
{
    cout << v.size() << " " << text << "(s) Vector:\n";
    for (int i = 0; i < v.size(); ++i)
    {
        cout << "- " << text << " " << v[i] << endl;
    }
}

template<typename K, typename V>
void printMap(const char* text, std::map<K, V> m)
{
    cout << m.size() << " " << text << endl;

    for (typename map<K, V>::iterator it = m.begin(); it != m.end(); ++it)
    {
        cout << "- " << (*it).first << " -> " << (*it).second << endl;
    }
}

void printScreenProperties(unsigned int screenid, const char* prefix)
{
    cout << prefix << "screen " << screenid << " (0x" << hex << screenid << dec
            << ")\n";
    cout << prefix << "---------------------------------------\n";

    unsigned int resolutionX = 0;
    unsigned int resolutionY = 0;
    ilm_getScreenResolution(screenid, &resolutionX, &resolutionY);
    cout << prefix << "- resolution:           x=" << resolutionX << ", y="
            << resolutionY << "\n";

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

void printLayerProperties(unsigned int layerid, const char* prefix)
{
    cout << prefix << "layer " << layerid << " (0x" << hex << layerid << dec
            << ")\n";
    cout << prefix << "---------------------------------------\n";

    ilmLayerProperties p;
    ilm_getPropertiesOfLayer(layerid, &p);

    cout << prefix << "- created by pid:       " << p.creatorPid << "\n";

    cout << prefix << "- original size:        x=" << p.origSourceWidth
            << ", y=" << p.origSourceHeight << "\n";
    cout << prefix << "- destination region:   x=" << p.destX << ", y="
            << p.destY << ", w=" << p.destWidth << ", h=" << p.destHeight
            << "\n";
    cout << prefix << "- source region:        x=" << p.sourceX << ", y="
            << p.sourceY << ", w=" << p.sourceWidth << ", h=" << p.sourceHeight
            << "\n";

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
        cout << surfaceArray[surfaceIndex] << "(0x" << hex
                << surfaceArray[surfaceIndex] << dec << "), ";
    }
    cout << "\n";

    cout << prefix << "- on screen:            ";
    unsigned int screenCount = 0;
    unsigned int* screenArray = NULL;
    ilm_getScreenIDs(&screenCount, &screenArray);
    for (unsigned int screenIndex = 0; screenIndex < screenCount;
            ++screenIndex)
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

void printSurfaceProperties(unsigned int surfaceid, const char* prefix)
{
    cout << prefix << "surface " << surfaceid << " (0x" << hex << surfaceid
            << dec << ")\n";
    cout << prefix << "---------------------------------------\n";

    ilmSurfaceProperties p;
    ilm_getPropertiesOfSurface(surfaceid, &p);

    cout << prefix << "- original size:      x=" << p.origSourceWidth << ", y="
            << p.origSourceHeight << "\n";
    cout << prefix << "- destination region: x=" << p.destX << ", y=" << p.destY
            << ", w=" << p.destWidth << ", h=" << p.destHeight << "\n";
    cout << prefix << "- source region:      x=" << p.sourceX << ", y="
            << p.sourceY << ", w=" << p.sourceWidth << ", h=" << p.sourceHeight
            << "\n";

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
            << ", draw=" << p.drawCounter << ", update=" << p.updateCounter
            << "\n";

    cout << prefix << "- on layer:           ";
    int layerCount = 0;
    unsigned int* layerArray = NULL;
    ilm_getLayerIDs(&layerCount, &layerArray);
    for (unsigned int layerIndex = 0; layerIndex < layerCount; ++layerIndex)
    {
        unsigned int layerid = layerArray[layerIndex];
        int surfaceCount = 0;
        unsigned int* surfaceArray = NULL;
        ilm_getSurfaceIDsOnLayer(layerid, &surfaceCount, &surfaceArray);
        for (int surfaceIndex = 0; surfaceIndex < surfaceCount;
                ++surfaceIndex)
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

void printScene()
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

bool gBenchmark_running;

void benchmarkSigHandler(int sig)
{
    gBenchmark_running = false;
}

void getCommunicatorPerformance()
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

void setSurfaceKeyboardFocus(t_ilm_surface surface)
{
    if (ilm_SetKeyboardFocusOn(surface) != ILM_SUCCESS)
    {
        cerr << "Error during communication" << endl;
    }
}

void getKeyboardFocus()
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

void setSurfaceAcceptsInput(t_ilm_surface surfaceId, string kbdPointerTouch, t_ilm_bool acceptance)
{
    char* str;
    char* tok;

    ilmInputDevice devices = (ilmInputDevice)0;

    str = new char [kbdPointerTouch.size()+1];
    strcpy (str, kbdPointerTouch.c_str());
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

    delete[] str;
}

void layerNotificationCallback(t_ilm_layer layer, struct ilmLayerProperties* properties, t_ilm_notification_mask mask)
{
    cout << "\nNotification: layer " << layer << " updated properties:\n";

    if (ILM_NOTIFICATION_VISIBILITY & mask)
    {
        cout << "\tvisibility = " << properties->visibility << "\n";
    }

    if (ILM_NOTIFICATION_OPACITY & mask)
    {
        cout << "\topacity = " << properties->opacity << "\n";
    }

    if (ILM_NOTIFICATION_ORIENTATION & mask)
    {
        cout << "\torientation = " << properties->orientation << "\n";
    }

    if (ILM_NOTIFICATION_SOURCE_RECT & mask)
    {
        cout << "\tsource rect = x:" << properties->sourceX
             << ", y:" << properties->sourceY
             << ", width:" << properties->sourceWidth
             << ", height:" << properties->sourceHeight
             << "\n";
    }

    if (ILM_NOTIFICATION_DEST_RECT & mask)
    {
        cout << "\tdest rect = x:" << properties->destX
             << ", y:" << properties->destY
             << ", width:" << properties->destWidth
             << ", height:" << properties->destHeight
             << "\n";
    }
}

void testNotificationLayer(t_ilm_layer layerid)
{

    ilmErrorTypes ret;
    cout << "Setup notification for layer " << layerid << " \n";
    ret = ilm_layerAddNotification(layerid, layerNotificationCallback);

    if (ret != ILM_SUCCESS)
    {
        cerr << "ilm_layerAddNotification returned error " << ret << "\n";
    }

    for  (int i = 0; i < 2; ++i)
    {
        usleep(100 * 1000);
        cout << "Set layer 1000 visbility to FALSE\n";
        ilm_layerSetVisibility(layerid, ILM_FALSE);
        ilm_commitChanges();

        usleep(100 * 1000);
        cout << "Set layer 1000 visbility to TRUE\n";
        ilm_layerSetVisibility(layerid, ILM_TRUE);

        cout << "Set layer 1000 opacity to 0.3\n";
        ilm_layerSetOpacity(layerid, 0.3);
        ilm_commitChanges();

        usleep(100 * 1000);
        cout << "Set layer 1000 opacity to 1.0\n";
        ilm_layerSetOpacity(layerid, 1.0);
        ilm_commitChanges();
    }

    ilm_commitChanges(); // make sure, app lives long enough to receive last notification
}

void watchLayer(unsigned int* layerids, unsigned int layeridCount)
{
    for (unsigned int i = 0; i < layeridCount; ++i)
    {
        unsigned int layerid = layerids[i];
        cout << "Setup notification for layer " << layerid << "\n";
        ilmErrorTypes ret = ilm_layerAddNotification(layerid, layerNotificationCallback);

        if (ret != ILM_SUCCESS)
        {
            cerr << "ilm_layerAddNotification(" << layerid << ") returned error " << ret << "\n";
            return;
        }
    }

    cout << "Waiting for notifications...\n";
    int block;
    cin >> block;

    for (unsigned int i = 0; i < layeridCount; ++i)
    {
        unsigned int layerid = layerids[i];
        cout << "Removing notification for layer " << layerid << "\n";
        ilm_layerRemoveNotification(layerid);
    }

    if (layerids)
    {
        delete[] layerids;
    }
}

void surfaceNotificationCallback(t_ilm_layer surface, struct ilmSurfaceProperties* properties, t_ilm_notification_mask mask)
{
    cout << "\nNotification: surface " << surface << " updated properties:\n";

    if (ILM_NOTIFICATION_VISIBILITY & mask)
    {
        cout << "\tvisibility = " << properties->visibility << "\n";
    }

    if (ILM_NOTIFICATION_OPACITY & mask)
    {
        cout << "\topacity = " << properties->opacity << "\n";
    }

    if (ILM_NOTIFICATION_ORIENTATION & mask)
    {
        cout << "\torientation = " << properties->orientation << "\n";
    }

    if (ILM_NOTIFICATION_SOURCE_RECT & mask)
    {
        cout << "\tsource rect = x:" << properties->sourceX
             << ", y:" << properties->sourceY
             << ", width:" << properties->sourceWidth
             << ", height:" << properties->sourceHeight
             << "\n";
    }

    if (ILM_NOTIFICATION_DEST_RECT & mask)
    {
        cout << "\tdest rect = x:" << properties->destX
             << ", y:" << properties->destY
             << ", width:" << properties->destWidth
             << ", height:" << properties->destHeight
             << "\n";
    }
}

void watchSurface(unsigned int* surfaceids, unsigned int surfaceidCount)
{
    for (unsigned int i = 0; i < surfaceidCount; ++i)
    {
        unsigned int surfaceid = surfaceids[i];
        cout << "Setup notification for surface " << surfaceid << "\n";
        ilmErrorTypes ret = ilm_surfaceAddNotification(surfaceid, surfaceNotificationCallback);

        if (ret != ILM_SUCCESS)
        {
            cerr << "ilm_surfaceAddNotification(" << surfaceid << ") returned error " << ret << "\n";
            return;
        }
    }

    cout << "Waiting for notifications...\n";
    int block;
    cin >> block;

    for (unsigned int i = 0; i < surfaceidCount; ++i)
    {
        unsigned int surfaceid = surfaceids[i];
        cout << "Removing notification for surface " << surfaceid << "\n";
        ilm_surfaceRemoveNotification(surfaceid);
    }

    if (surfaceids)
    {
        delete[] surfaceids;
    }
}

void setOptimization(t_ilm_uint id, t_ilm_uint mode)
{
    if (ilm_SetOptimizationMode((ilmOptimization)id,
                                    (ilmOptimizationMode)mode) != ILM_SUCCESS)
    {
        cerr << "Error during communication" << endl;
    }

    ilm_commitChanges();
}

void getOptimization(t_ilm_uint id)
{
    ilmOptimization optimizationId = (ilmOptimization)id;
    ilmOptimizationMode optimizationMode;
    if (ilm_GetOptimizationMode(optimizationId, &optimizationMode) == ILM_SUCCESS)
    {
        switch ( optimizationId )
        {
        case ILM_OPT_MULTITEXTURE :
            cout << "Optimization " << (int)optimizationId << " (Multitexture Optimization)" << endl;
            break;

        case ILM_OPT_SKIP_CLEAR :
            cout << "Optimization " << (int)optimizationId << " (Skip Clear)" << endl;
            break;
        default:
            cout << "Optimization " << "unknown" << endl;
            break;
        }

        switch (optimizationMode)
        {
        case ILM_OPT_MODE_FORCE_OFF :
            cout << "Mode " << (int)optimizationMode << " (forced off)" << endl;
            break;

        case ILM_OPT_MODE_FORCE_ON :
            cout << "Mode " << (int)optimizationMode << " (forced on)" << endl;
            break;
        case ILM_OPT_MODE_HEURISTIC :
            cout << "Mode " << (int)optimizationMode << " (Heuristic / Render choose the optimization)" << endl;
            break;
        case ILM_OPT_MODE_TOGGLE :
            cout << "Mode " << (int)optimizationMode << " (Toggle on/and off rapidly for debugging)" << endl;
            break;

        default:
            cout <<"Mode " << "unknown" << endl ;
            break;
        }
    }
    else
    {
        cerr << "Error during communication" << endl;
    }

    ilm_commitChanges();
}
