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

#include "LayerSceneProvider.h"

LayerSceneProvider::LayerSceneProvider(ICommandExecutor* executor)
: ISceneProvider(executor)
{
}

typedef struct t_layerScene 
{
    unsigned int layer;
    bool visibility;
    float opacity;
} layerScene;

static layerScene gInitialLayerScene[] = 
{
    { LAYER_EXAMPLE_VIDEO_APPLICATIONS, true, 1.0 },
    { LAYER_EXAMPLE_GLES_APPLICATIONS, true, 1.0 },
    { LAYER_EXAMPLE_X_APPLICATIONS, true, 1.0 }
};

bool LayerSceneProvider::delegateScene() 
{
    bool result = true;
    int i = 0;
    int numberOfLayers = sizeof(gInitialLayerScene) / sizeof (layerScene);
    unsigned int *renderOrder = new unsigned int [numberOfLayers];
    unsigned int* screenResolution = m_executor->getScreenResolution(0);    
    if ( numberOfLayers > 0 ) 
    {
        /* setup inital layer scenery */
        for (i = 0;i<numberOfLayers;i++)
        {
            m_executor->execute(new CreateCommand(0, TypeLayer, PIXELFORMAT_R8, screenResolution[0], screenResolution[1], &(gInitialLayerScene[i].layer)));            
            m_executor->execute(new SetSourceRectangleCommand(gInitialLayerScene[i].layer, TypeLayer, 0, 0, screenResolution[0], screenResolution[1]));
            m_executor->execute(new SetDestinationRectangleCommand(gInitialLayerScene[i].layer, TypeLayer, 0, 0, screenResolution[0], screenResolution[1]));
            m_executor->execute(new SetOpacityCommand(gInitialLayerScene[i].layer,TypeLayer,gInitialLayerScene[i].opacity) );
            m_executor->execute(new SetVisibilityCommand(gInitialLayerScene[i].layer,TypeLayer,gInitialLayerScene[i].visibility) );
            m_executor->execute(new CommitCommand());            
            renderOrder[i]=gInitialLayerScene[i].layer;
        }        
        /* Finally set the first executed renderorder */
        m_executor->execute(new SetLayerOrderCommand(renderOrder,numberOfLayers));
        m_executor->execute(new CommitCommand());
    }
    return result;    
}

extern "C" ISceneProvider* createLayerScene(ICommandExecutor* executor)
{
    return new LayerSceneProvider(executor);
}

extern "C" void destroyLayerScene(LayerSceneProvider* p)
{
    delete p;
}
