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
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

#include "ilm_client.h"
#include "LMControl.h"

#include <algorithm>
using std::find;

#include <cmath>
using std::max;
using std::min;

#include <iterator>
using std::iterator;

#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#include <vector>
using std::vector;

#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>


tuple4 getSurfaceScreenCoordinates(ilmSurfaceProperties targetSurfaceProperties, ilmLayerProperties targetLayerProperties)
{
    t_ilm_float horizontalScale = targetLayerProperties.sourceWidth ?
                    1.0 * targetLayerProperties.destWidth / targetLayerProperties.sourceWidth : 0;

    t_ilm_float targetX1 = targetLayerProperties.destX + horizontalScale
            * (targetSurfaceProperties.destX- targetLayerProperties.sourceX);
    t_ilm_float targetX2 = targetX1 + horizontalScale * targetSurfaceProperties.destWidth;

    t_ilm_float verticalScale = targetLayerProperties.sourceHeight ?
            1.0 * targetLayerProperties.destHeight/ targetLayerProperties.sourceHeight : 0;

    t_ilm_float targetY1 = targetLayerProperties.destY + verticalScale
            * (targetSurfaceProperties.destY - targetLayerProperties.sourceY);
    t_ilm_float targetY2 = targetY1 + verticalScale * targetSurfaceProperties.destHeight;

    tuple4 targetSurfaceCoordinates(static_cast<t_ilm_int>(targetX1),
            static_cast<t_ilm_int>(targetY1),
            max(0, static_cast<t_ilm_int>(targetX2) - 1),
            max(0, static_cast<t_ilm_int>(targetY2) - 1));

    return targetSurfaceCoordinates;
}

tuple4 getSurfaceScreenCoordinates(t_scene_data* pScene, t_ilm_surface surface)
{
    tuple4 surfaceCoordinates;

    //if surface belongs to a layer make it appear exacrly as it would appear on its current layer
    if (pScene->surfaceLayer.find(surface) != pScene->surfaceLayer.end())
    {
        //set dimensions of the surface to map to the extra layer according to its current placement in the
        t_ilm_layer layer = pScene->surfaceLayer[surface];
        ilmLayerProperties layerProperties = pScene->layerProperties[layer];
        ilmSurfaceProperties surfaceProperties = pScene->surfaceProperties[surface];

        surfaceCoordinates = getSurfaceScreenCoordinates(surfaceProperties, layerProperties);
    }
    //if surface does not belong to a layer just assume it belongs to a layer that fills the screen
    else
    {
        ilmSurfaceProperties surfaceProperties = pScene->surfaceProperties[surface];

        surfaceCoordinates.x = surfaceProperties.destX;
        surfaceCoordinates.y = surfaceProperties.destY;
        surfaceCoordinates.z = surfaceProperties.destX + surfaceProperties.destWidth;
        surfaceCoordinates.w = surfaceProperties.destX + surfaceProperties.destHeight;
    }

    return surfaceCoordinates;
}

t_ilm_bool surfaceRenderedOnScreen(t_scene_data& scene, t_ilm_surface surface)
{
    //if scene belongs to a layer and that layer belongs to a screen
    if (scene.surfaceLayer.find(surface) != scene.surfaceLayer.end())
    {
        t_ilm_layer layer = scene.surfaceLayer[surface];
        if (scene.layerScreen.find(layer) != scene.layerScreen.end())
        {
            return ILM_TRUE;
        }
    }

    return ILM_FALSE;
}

vector<t_ilm_surface> getSceneRenderOrder(t_scene_data* pScene)
{
    t_scene_data& scene = *pScene;
    vector<t_ilm_surface> renderOrder;

    //iterate over screens
    for (vector<t_ilm_display>::iterator it = scene.screens.begin(); it != scene.screens.end(); ++it)
    {
        t_ilm_display screen = *it;
        vector<t_ilm_layer> layers = scene.screenLayers[screen];

        //iterate over layers
        for (vector<t_ilm_layer>::iterator layerIterator = layers.begin();
                layerIterator != layers.end(); ++layerIterator)
        {
            t_ilm_layer layer = (*layerIterator);
            vector<t_ilm_surface> surfaces = scene.layerSurfaces[layer];

            //iterate over surfaces
            for (vector<t_ilm_surface>::iterator it = surfaces.begin(); it != surfaces.end(); ++it)
            {
                t_ilm_surface surface = *it;

                renderOrder.push_back(surface);
            }
        }
    }

    return renderOrder;
}

void captureSceneData(t_scene_data* pScene)
{
    t_scene_data& scene = *pScene;

    //get screen information
    t_ilm_uint screenWidth = 0, screenHeight = 0;
    ilm_getScreenResolution(0, &screenWidth, &screenHeight);
    scene.screenWidth = screenWidth;
    scene.screenHeight = screenHeight;

    //extra layer for debugging
    scene.extraLayer = 0xFFFFFFFF;

    //get screens
    unsigned int screenCount = 0;
    t_ilm_display* screenArray = NULL;
    ilm_getScreenIDs(&screenCount, &screenArray);
    scene.screens = vector<t_ilm_display>(screenArray, screenArray + screenCount);

    //layers on each screen
    for (int i = 0; i < screenCount; ++i)
    {
        t_ilm_display screenId = screenArray[i];

        t_ilm_int layerCount = 0;
        t_ilm_layer* layerArray = NULL;
        ilm_getLayerIDsOnScreen(screenId, &layerCount, &layerArray);

        scene.screenLayers[screenId] = vector<t_ilm_layer>(layerArray, layerArray + layerCount);

        //preserve rendering order for layers on each screen
        for (int j = 0; j < layerCount; ++j)
        {
            t_ilm_layer layerId = layerArray[j];

            scene.layerScreen[layerId] = screenId;
        }
    }

    //get all layers (rendered and not rendered)
    t_ilm_int layerCount = 0;
    t_ilm_layer* layerArray = NULL;
    ilm_getLayerIDs(&layerCount, &layerArray);
    scene.layers = vector<t_ilm_layer>(layerArray, layerArray + layerCount);

    for (int j = 0; j < layerCount; ++j)
    {
        t_ilm_layer layerId = layerArray[j];

        //layer properties
        ilmLayerProperties lp;
        ilm_getPropertiesOfLayer(layerId, &lp);
        scene.layerProperties[layerId] = lp;
    }

    //surfaces on each layer
    for (int j = 0; j < layerCount; ++j)
    {
        t_ilm_layer layerId = layerArray[j];

        //surfaces on layer (in rendering order)
        int surfaceCount = 0;
        t_ilm_surface* surfaceArray = NULL;
        ilm_getSurfaceIDsOnLayer(layerId, &surfaceCount, &surfaceArray);

        //rendering order on layer
        scene.layerSurfaces[layerId] = vector<t_ilm_surface>(surfaceArray, surfaceArray + surfaceCount);

        //make each surface aware of its layer
        for (int k = 0; k < surfaceCount; ++k)
        {
            t_ilm_surface surfaceId = surfaceArray[k];
            scene.surfaceLayer[surfaceId] = layerId;
        }
    }

    //get all surfaces (on layers and without layers)
    t_ilm_int surfaceCount = 0;
    t_ilm_surface* surfaceArray = NULL;
    ilm_getSurfaceIDs(&surfaceCount, &surfaceArray);
    scene.surfaces = vector<t_ilm_surface>(surfaceArray, surfaceArray + surfaceCount);

    for (int k = 0; k < surfaceCount; ++k)
    {
        t_ilm_surface surfaceId = surfaceArray[k];

        //surface properties
        ilmSurfaceProperties sp;
        ilm_getPropertiesOfSurface(surfaceId, &sp);
        scene.surfaceProperties[surfaceId] = sp;
    }
}

void setScene(t_scene_data* pScene, bool clean)
{
    t_scene_data initialScene;
    captureSceneData(&initialScene);

    //dismantel current scene
    for (map<t_ilm_surface, t_ilm_layer>::iterator it = initialScene.surfaceLayer.begin();
            it != initialScene.surfaceLayer.end(); ++it)
    {
        t_ilm_surface surface = it->first;
        t_ilm_layer layer = it->second;

        ilm_layerRemoveSurface(layer, surface);
    }

    ilm_commitChanges();

    //cleaning scene if needed
    if (clean)
    {
        //remove unneeded surfaces and layers!
        for (vector<t_ilm_surface>::iterator it = initialScene.surfaces.begin();
                it != initialScene.surfaces.end(); ++it)
        {
            t_ilm_surface surface = *it;
            //if surface does not exist (in final scene)
            if (find(pScene->surfaces.begin(), pScene->surfaces.end(), surface) == pScene->surfaces.end())
            {
                //remove surface !
                ilm_surfaceRemove(surface);
            }
        }

        ilm_commitChanges();

        for (vector<t_ilm_layer>::iterator it = initialScene.layers.begin();
                it != initialScene.layers.end(); ++it)
        {
            t_ilm_layer layer = *it;
            //if layer does not exist (in final scene)
            if (find(pScene->layers.begin(), pScene->layers.end(), layer) == pScene->layers.end())
            {
                //remove surface !
                ilm_layerRemove(layer);
            }
        }

        ilm_commitChanges();
    }

    //make sure all layers and surfaces of the new scene are created
    for (vector<t_ilm_layer>::iterator it = pScene->layers.begin();
            it != pScene->layers.end(); ++it)
    {
        t_ilm_layer layer = *it;
        //if layer does not exist (in initial scene)
        if (find(initialScene.layers.begin(), initialScene.layers.end(), layer) == initialScene.layers.end())
        {
            ilmLayerProperties& props = pScene->layerProperties[layer];
            ilm_layerCreateWithDimension(&layer, props.origSourceWidth, props.origSourceHeight);
        }
    }

    ilm_commitChanges();

    for (vector<t_ilm_surface>::iterator it = pScene->surfaces.begin();
            it != pScene->surfaces.end(); ++it)
    {
        t_ilm_surface surface = *it;
        //if surface does not exist (in initial scene)
        if (find(initialScene.surfaces.begin(), initialScene.surfaces.end(), surface)
                == initialScene.surfaces.end())
        {
            ilmSurfaceProperties& props = pScene->surfaceProperties[surface];

            ilm_surfaceCreate(props.nativeSurface,
                    props.origSourceWidth,
                    props.origSourceHeight,
                    (e_ilmPixelFormat) props.pixelformat,
                    &surface);
        }

        //if surface exists but mapped to different native: remove then recreate it
        if (initialScene.surfaceProperties[surface].nativeSurface
                != pScene->surfaceProperties[surface].nativeSurface)
        {
            ilm_surfaceRemove(surface);
            ilmSurfaceProperties& props = pScene->surfaceProperties[surface];
            ilm_surfaceCreate(props.nativeSurface,
                    props.origSourceWidth,
                    props.origSourceHeight,
                    (e_ilmPixelFormat) props.pixelformat,
                    &surface);
        }
    }

    ilm_commitChanges();

    //set render order of layers on each screen
    for (vector<t_ilm_display>::iterator it = pScene->screens.begin();
            it != pScene->screens.end(); ++it)
    {
        t_ilm_display screen = *it;
        vector<t_ilm_layer>& layers = pScene->screenLayers[screen];

        ilm_displaySetRenderOrder(screen, layers.data(), layers.size());
    }

    ilm_commitChanges();

    //set render order of surfaces on each layer
    for (map<t_ilm_layer, vector<t_ilm_surface> >::iterator it = pScene->layerSurfaces.begin();
            it != pScene->layerSurfaces.end(); ++it)
    {
        t_ilm_layer layer = it->first;
        vector<t_ilm_surface>& surfaces = it->second;
        ilm_layerSetRenderOrder(layer, surfaces.data(), surfaces.size());
    }

    ilm_commitChanges();

    //set properties of layers
    for (map<t_ilm_layer, ilmLayerProperties>::iterator it = pScene->layerProperties.begin();
            it != pScene->layerProperties.end(); ++it)
    {
        t_ilm_layer layer = it->first;
        ilmLayerProperties& props = it->second;

        //set layer properties
        ilm_layerSetDestinationRectangle(layer, props.destX, props.destY, props.destWidth, props.destHeight);
        ilm_commitChanges();

        ilm_layerSetOpacity(layer, props.opacity);
        ilm_commitChanges();

        ilm_layerSetOrientation(layer, props.orientation);
        ilm_commitChanges();

        ilm_layerSetSourceRectangle(layer, props.sourceX, props.sourceY, props.sourceWidth, props.sourceHeight);
        ilm_commitChanges();

        ilm_layerSetVisibility(layer, props.visibility);
        ilm_commitChanges();
    }

    //set properties of surfaces
    for (map<t_ilm_surface, ilmSurfaceProperties>::iterator it = pScene->surfaceProperties.begin();
            it != pScene->surfaceProperties.end(); ++it)
    {
        t_ilm_surface surface = it->first;
        ilmSurfaceProperties& props = it->second;

        ilm_surfaceSetNativeContent(props.nativeSurface,
                props.origSourceWidth,
                props.origSourceHeight,
                (e_ilmPixelFormat) props.pixelformat,
                surface);
        ilm_commitChanges();

        ilm_surfaceSetOpacity(surface, props.opacity);
        ilm_commitChanges();

        ilm_surfaceSetOrientation(surface, props.orientation);
        ilm_commitChanges();

        ilm_surfaceSetSourceRectangle(surface, props.sourceX, props.sourceY, props.sourceWidth, props.sourceHeight);
        ilm_commitChanges();

        ilm_surfaceSetDestinationRectangle(surface, props.destX, props.destY, props.destWidth, props.destHeight);
        ilm_commitChanges();

        ilm_surfaceSetVisibility(surface, props.visibility);
        ilm_commitChanges();
    }

    ilm_commitChanges();
}

void emptyScene(t_scene_data* pScene)
{
    pScene->extraLayer = -1;
    pScene->layerProperties.clear();
    pScene->layerScreen.clear();
    pScene->layerSurfaces.clear();
    pScene->layers.clear();
    pScene->screenHeight;
    pScene->screenLayers.clear();
    pScene->screenWidth;
    pScene->screens.clear();
    pScene->surfaceLayer.clear();
    pScene->surfaceProperties.clear();
    pScene->surfaces.clear();

    t_ilm_uint count;
    t_ilm_display* screenArray;
    ilm_getScreenIDs(&count, &screenArray);
    for(int i = 0 ; i < count ; ++i)
    {
        pScene->screens.push_back(screenArray[i]);
        ilm_getScreenResolution(screenArray[0], & pScene->screenWidth, & pScene->screenHeight);
    }
}

t_scene_data cloneToUniLayerScene(t_scene_data* pScene)
{
    //prepare values needed for dummy (animation) scene
    t_ilm_layer extraLayer = -1;
    //if the scene is already uni layer just reuse same layer
    if(pScene->layers.size() == 1)
    {
        extraLayer = pScene->layers[0];
    }

    ilm_layerCreate(&extraLayer);

    ilmLayerProperties extraLayerProperties;
    extraLayerProperties.destHeight = pScene->screenHeight;
    extraLayerProperties.destWidth = pScene->screenWidth;
    extraLayerProperties.destX = 0;
    extraLayerProperties.destY = 0;
    extraLayerProperties.opacity = 1;
    extraLayerProperties.orientation = (e_ilmOrientation) 0;
    extraLayerProperties.origSourceHeight = pScene->screenHeight;
    extraLayerProperties.origSourceWidth = pScene->screenWidth;
    extraLayerProperties.sourceHeight = pScene->screenHeight;
    extraLayerProperties.sourceWidth = pScene->screenWidth;
    extraLayerProperties.sourceX = 0;
    extraLayerProperties.sourceY = 0;
    extraLayerProperties.type = 2;
    extraLayerProperties.visibility = 1;

    t_ilm_display screen = pScene->screens[0];

    vector<t_ilm_surface> finalRenderOrder = getSceneRenderOrder(pScene);

    //build dummy scene to be used for animation
    t_scene_data dummyScene;
    //to avoid destroying and recreating surfaces and layers
    dummyScene.surfaces = pScene->surfaces;

    //the real deal !
    dummyScene.screens.push_back(screen);
    dummyScene.screenLayers[screen] = vector<t_ilm_layer>(&extraLayer, &extraLayer + 1);
    dummyScene.layerScreen[extraLayer] = screen;
    dummyScene.layerSurfaces[extraLayer] = finalRenderOrder;

    for (vector<t_ilm_surface>::iterator it = finalRenderOrder.begin();
            it != finalRenderOrder.end(); ++it)
    {
        dummyScene.surfaceProperties[*it] = pScene->surfaceProperties[*it];

        tuple4 coords = getSurfaceScreenCoordinates(pScene, *it);

        dummyScene.surfaceProperties[*it].destX = coords.x;
        dummyScene.surfaceProperties[*it].destY = coords.y;
        dummyScene.surfaceProperties[*it].destWidth = coords.z - coords.x;
        dummyScene.surfaceProperties[*it].destHeight = coords.w - coords.y;
    }

    dummyScene.layerProperties[extraLayer] = extraLayerProperties;

    dummyScene.layerScreen[extraLayer] = screen;

    for (vector<t_ilm_surface>::iterator it = finalRenderOrder.begin();
            it != finalRenderOrder.end(); ++it)
    {
        dummyScene.surfaceLayer[*it] = extraLayer;
    }

    dummyScene.surfaces = finalRenderOrder;
    dummyScene.layers = vector<t_ilm_layer>(&extraLayer, &extraLayer + 1);

    dummyScene.extraLayer = -1;
    dummyScene.screenWidth = pScene->screenWidth;
    dummyScene.screenHeight = pScene->screenHeight;

    return dummyScene;
}

tuple4 interpolateCoordinatesHelper(tuple4& start, tuple4& end, float t)
{
    t = static_cast<float>(1 - pow((t - 1), 4));
    return tuple4(static_cast<int>(start.x * (1 - t) + end.x * t),
            static_cast<int>(start.y * (1 - t) + end.y * t),
            static_cast<int>(start.z * (1 - t) + end.z * t),
            static_cast<int>(start.w * (1 - t) + end.w * t));
}

void transformScene(t_scene_data* pInitialScene, t_scene_data* pFinalScene, t_ilm_long durationMillis, t_ilm_int frameCount)
{
    t_scene_data dummyScene = cloneToUniLayerScene(pFinalScene);
    //set dummy scene
    setScene(&dummyScene);

    //animate dummy scene !

    if (durationMillis > 0 && frameCount > 0)
    {
        //sleep time
        long sleepMillis = durationMillis / frameCount;
        struct timespec sleepTime;
        sleepTime.tv_nsec = (sleepMillis% 1000) * 1000000;
        sleepTime.tv_sec = sleepMillis / 1000;

        struct timespec remTime;

        //start and end coordinates of surfaces
        map<t_ilm_surface, tuple4> start;
        map<t_ilm_surface, tuple4> end;
        for(vector<t_ilm_surface>::iterator it = dummyScene.surfaces.begin();
                it != dummyScene.surfaces.end(); ++it)
        {
            t_ilm_surface surface = *it;
            start[surface] = getSurfaceScreenCoordinates(pInitialScene, surface);

            end[surface] = getSurfaceScreenCoordinates(pFinalScene, surface);
        }

        for (int i = 0; i < frameCount; ++i)
        {
            float t = 1.0 * i / frameCount;

            //interpolate properties of each surface
            for(vector<t_ilm_surface>::iterator it = dummyScene.surfaces.begin();
                    it != dummyScene.surfaces.end(); ++it)
            {
                t_ilm_surface surface = *it;
                tuple4 coords = interpolateCoordinatesHelper(start[surface], end[surface], t);

                ilm_surfaceSetDestinationRectangle(surface, coords.x, coords.y, coords.z - coords.x, coords.w - coords.y);

                float opacity = t * pFinalScene->surfaceProperties[surface].opacity
                        + (1-t) * pInitialScene->surfaceProperties[surface].opacity;
                ilm_surfaceSetOpacity(surface, opacity);
            }

            ilm_commitChanges();

            //sleep
            nanosleep(&sleepTime, &remTime);
        }
    }

    //set final scene
    setScene(pFinalScene);
}

static t_scene_data* global_pOriginalScene = NULL;

void interruptSignalRestoreScene(int s)
{
    cout<<"LayerManagerControl :Interrupt signal...\n";
    if (global_pOriginalScene != NULL)
    {
        setScene(global_pOriginalScene, true);
        sleep(3);
        exit(0);
    }
}

void setSceneToRestore(t_scene_data* pScene)
{
    if (global_pOriginalScene == NULL)
    {
        global_pOriginalScene = pScene;
        struct sigaction signalAction;

        signalAction.sa_handler = &interruptSignalRestoreScene;
        sigemptyset(&signalAction.sa_mask);
        signalAction.sa_flags = 0;

        sigaction(SIGINT, &signalAction, NULL);
    }
}
