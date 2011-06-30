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
#include "ilm_client.h"
#include "LayerScene.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string>

#define LAYERWIDTH 800
#define LAYERHEIGHT 480


const char* USAGE_DESCRIPTION = "Usage:\t LayerManagerToolBox [options]\n"
                                "options:\t\n\n"
                                "\t-w: Window id\t\n"
                                "\t-s: surface id\t\n"
                                "\t-o: opacity \t\n"
                                "\t-v: visibility [0;1]\n"
                                "\t-r: surface id to remove\n"
                                "\t-x: window dimension x\n"
                                "\t-y: window dimension y\n"
                                "\t-p: 0..6\n"
                                "\t\t  ILM_PIXELFORMAT_R_8 = 0\n"
                                "\t\t  ILM_PIXELFORMAT_RGB_888 = 1\n"
                                "\t\t  ILM_PIXELFORMAT_RGBA_8888 = 2\n"
                                "\t\t  ILM_PIXELFORMAT_RGB_565 = 3\n"
                                "\t\t  ILM_PIXELFORMAT_RGBA_5551 = 4\n"
                                "\t\t  ILM_PIXELFORMAT_RGBA_6661 = 5\n"
                                "\t\t  ILM_PIXELFORMAT_RGBA_4444 = 6\n"
                                "\nexample: LayerManagerToolbox -w0x000 -s10 -x320 -y240 -v1 -o0.5 -p2\n";

typedef enum e_toolbox_state 
{
    TOOLBOX_INIT_STATE = 0,    
    TOOLBOX_ADD_WINDOW = 1,
    TOOLBOX_CHANGE_SURFACE_VALUES = 2,
    TOOLBOX_REMOVE_SURFACE= 3
} toolbox_state;

typedef struct s_global_parameter 
{
    t_ilm_nativehandle windowid;
    t_ilm_surface surfaceid;
    t_ilm_int width;
    t_ilm_int height;
    t_ilm_bool visibility;
    t_ilm_float opacity;
    ilmPixelFormat pixelformat;
    toolbox_state state;
} t_param_struct;

ilmErrorTypes removeSurface(t_param_struct* param_struct) 
{  
    ilmErrorTypes error;
    ilm_surfaceRemove(param_struct->surfaceid);
    ilm_commitChanges();
    return error;
}
ilmErrorTypes changeSurfaceValues(t_param_struct* param_struct) 
{  
    ilmErrorTypes error;
    error = ilm_surfaceSetDestinationRectangle(param_struct->surfaceid, 0, 0, param_struct->width,param_struct->height);

    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    printf("set surface src region\n");
    error = ilm_surfaceSetSourceRectangle(param_struct->surfaceid, 0, 0, param_struct->width, param_struct->height);

    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    printf("Set surface visible\n");
    error = ilm_surfaceSetVisibility(param_struct->surfaceid, param_struct->visibility);
    printf("Set surface opacity\n");
    error = ilm_surfaceSetOpacity(param_struct->surfaceid, param_struct->opacity);

    //if (error == ILM_FAILED) return ILM_FALSE;

    printf("commit\n");
    error = ilm_commitChanges();

    return error;
}

ilmErrorTypes addWindowToLayer(t_param_struct* param_struct) 
{   
    ilmErrorTypes error;
    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    printf("create a surface %lu\n", (t_ilm_nativehandle) param_struct->windowid);
    error = ilm_surfaceCreate( (t_ilm_nativehandle) param_struct->windowid, param_struct->width,param_struct->height,
            param_struct->pixelformat, &param_struct->surfaceid);

    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    printf("set surface dest region\n");
    error = ilm_surfaceSetDestinationRectangle(param_struct->surfaceid, 0, 0, param_struct->width,param_struct->height);

    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    printf("set surface src region\n");
    error = ilm_surfaceSetSourceRectangle(param_struct->surfaceid, 0, 0, param_struct->width, param_struct->height);

    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    printf("add surface to layer\n");
    error = ilm_layerAddSurface(LAYER_EXAMPLE_X_APPLICATIONS,param_struct->surfaceid);
    printf("Set surface visible\n");
    error = ilm_surfaceSetVisibility(param_struct->surfaceid, param_struct->visibility);
    printf("Set surface opacity\n");
    error = ilm_surfaceSetOpacity(param_struct->surfaceid, param_struct->opacity);

    //if (error == ILM_FAILED) return ILM_FALSE;

    printf("commit\n");
    error = ilm_commitChanges();
    return error;
}

ilmErrorTypes init_toolbox(t_param_struct* pStruct)
{
    ilmErrorTypes result = ILM_FAILED;

    result = ilm_init();

    if(ILM_SUCCESS == result)
    {
        printf("ilm_init success\n");
        pStruct->state = TOOLBOX_INIT_STATE;
        pStruct->opacity = 1.0f;
        pStruct->visibility = ILM_TRUE;
        pStruct->windowid = 0x0;
        pStruct->surfaceid =  SURFACE_EXAMPLE_GDTESTENV_APPLICATION_9;
        pStruct->width = 320;
        pStruct->height = 240;
        pStruct->pixelformat = ILM_PIXELFORMAT_RGBA_8888;
    }
    else
    {
        printf("ilm_init failed\n");
    }

    return result;
}

void shutdown_toolbox()
{
    ilmErrorTypes error = ILM_FAILED;
    printf("Calling ilm_destroy... ");
    error = ilm_destroy();

    if(ILM_SUCCESS == error)
    {
        printf("success\n");
    }
    else
    {
        printf("failed\n");
    }
}

void parseCommandLine(t_param_struct* param_struct, int argc, char **argv)
{   
    while (optind < argc)
    {
        int option = getopt(argc, argv, "x::y::w::s::r::v::o::p::?");
        switch (option)
        {
        case 'w':
            if (param_struct->state == TOOLBOX_INIT_STATE) 
            {
                sscanf(optarg, "%x", &param_struct->windowid);
                param_struct->state = TOOLBOX_ADD_WINDOW;
            } else {
                printf("Warning ignoring parameter window\n");
            }
            break;
        case 'x':
            if (param_struct->state == TOOLBOX_ADD_WINDOW
                 || param_struct->state == TOOLBOX_CHANGE_SURFACE_VALUES 
                ) 
            {
                param_struct->width = atoi(optarg);
            } else {
                printf("Warning ignoring parameter x\n");
            }
            break;
        case 'y':
            if (param_struct->state == TOOLBOX_ADD_WINDOW
                || param_struct->state == TOOLBOX_CHANGE_SURFACE_VALUES 
                ) 
            {
                param_struct->height = atoi(optarg);
            } else {
                printf("Warning ignoring parameter y\n");
            }
            break;
        case 's':
            param_struct->surfaceid = atoi(optarg);
            if (param_struct->state == TOOLBOX_INIT_STATE) 
            {
                param_struct->state = TOOLBOX_CHANGE_SURFACE_VALUES;
            }
            break;
        case 'o':
            if ( param_struct->state == TOOLBOX_ADD_WINDOW 
                 || param_struct->state == TOOLBOX_CHANGE_SURFACE_VALUES 
                ) 
            {
                param_struct->opacity = atof(optarg);
            } else {
                printf("Warning ignoring parameter o\n");
            }
            break;
        case 'v':
            if ( param_struct->state == TOOLBOX_ADD_WINDOW 
                 || param_struct->state == TOOLBOX_CHANGE_SURFACE_VALUES 
                ) 
            {
                param_struct->visibility = atoi(optarg);
            } else {
                printf("Warning ignoring parameter o\v");
            }
            break;
        case 'r':
            if ( param_struct->state == TOOLBOX_INIT_STATE
                ) 
            {
                param_struct->surfaceid = atoi(optarg);
                param_struct->state = TOOLBOX_REMOVE_SURFACE;
            } else {
                printf("Warning ignoring parameter r\v");
            }
            break;
        case 'p':
            if ( param_struct->state == TOOLBOX_ADD_WINDOW  
              || param_struct->state == TOOLBOX_CHANGE_SURFACE_VALUES 
               ) 
            {
                param_struct->pixelformat = (ilmPixelFormat)atoi(optarg);
            } else {
                printf("Warning ignoring parameter p\n");
            }
            break;
        case '?':   
        default:
            puts(USAGE_DESCRIPTION);
            exit(-1);
        }
    }
}

int main(int argc, char **argv)
{
    t_param_struct pStruct;
    if (argc == 1) 
    {
        puts(USAGE_DESCRIPTION);
        return (-1);
    }

    if (ILM_SUCCESS != init_toolbox(&pStruct))
    {
        return -1;
    }
    parseCommandLine(&pStruct,argc, (char**) argv);
    switch(pStruct.state) 
    {
        case TOOLBOX_ADD_WINDOW :
            addWindowToLayer(&pStruct);
            break;
        case TOOLBOX_CHANGE_SURFACE_VALUES :
            changeSurfaceValues(&pStruct);
            break;
        case TOOLBOX_REMOVE_SURFACE :
            removeSurface(&pStruct);
            break;
        default:
         printf("Other options currently not implemented\n");
    }
    shutdown_toolbox();
}
