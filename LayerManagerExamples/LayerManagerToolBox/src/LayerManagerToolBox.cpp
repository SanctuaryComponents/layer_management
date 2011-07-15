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
#include <string.h>

#define LAYERWIDTH 800
#define LAYERHEIGHT 480


const char* USAGE_DESCRIPTION = "Usage:\t LayerManagerToolBox [options]\n"
                                "options:\t\n\n";                              

typedef enum e_toolbox_state 
{
    TOOLBOX_ADD_LAYER = 0,
    TOOLBOX_ADD_SURFACE,
    TOOLBOX_LIST_LAYER,
    TOOLBOX_LIST_SURFACE,
    TOOLBOX_REMOVE_LAYER,
    TOOLBOX_REMOVE_SURFACE,
    TOOLBOX_PROPERTIES_LAYER,
    TOOLBOX_PROPERTIES_SURFACE,
    TOOLBOX_CHANGE_LAYER,
    TOOLBOX_CHANGE_SURFACE,
    TOOLBOX_SET_DISPLAY_RO,
    TOOLBOX_SET_LAYER_RO,
    TOOLBOX_INIT_STATE
} toolbox_state;

typedef enum e_arguments 
{
    ARGUMENT_NONE = 0,
    ARGUMENT_SURFACEID,
    ARGUMENT_LAYERID,
    ARGUMENT_SCREENID,
    ARGUMENT_WINDOW,
    ARGUMENT_SOURCE,
    ARGUMENT_DEST,
    ARGUMENT_VISIBILITY,
    ARGUMENT_OPACITY,
    ARGUMENT_LAYER_RO,
    ARGUMENT_SURFACE_RO,
    ARGUMENT_PIXELFORMAT
} argumentType;

typedef struct s_argument_params 
{
    bool required;
    char* argument;
    char* description;
    argumentType type;    
} t_argument_params;

typedef struct s_state_params 
{
    toolbox_state state;
    int numberArguments;
    t_argument_params params[10];
} t_state_params;

t_state_params state_params[] = 
{
    {   TOOLBOX_ADD_LAYER, 4, 
        {
            {true, "lid:","par = layer id in hex", ARGUMENT_LAYERID},
            {false,"dest:","par = dest rec in x,y,w,h",     ARGUMENT_DEST},
            {false,"o:","par = opacity 0.0 .. 1.0",        ARGUMENT_OPACITY},
            {false,"v:","par = visibility 0 / 1",        ARGUMENT_VISIBILITY}
        }
    },
    {   TOOLBOX_ADD_SURFACE, 8,
        {
            {true, "lid:","par = layer id in hex",      ARGUMENT_LAYERID},
            {true, "sid:","par = surface id in hex",      ARGUMENT_SURFACEID},
            {true, "wid:","par = window id in hex",      ARGUMENT_WINDOW},        
            {false,"src:","par = src rec in x,y,w,h",      ARGUMENT_SOURCE},
            {false,"dest:","par = dest rec in x,y,w,h",     ARGUMENT_DEST},
            {false,"o:","par = opacity 0.0 ... 1.0",       ARGUMENT_OPACITY},
            {false,"v:","par = visibility 0 / 1",         ARGUMENT_VISIBILITY},
            {false,"pf:","par = pixelformat 0 .. 6 ",        ARGUMENT_PIXELFORMAT}
        }
    },
    {   TOOLBOX_LIST_LAYER, 1,
        {
            {false, "screen:","par = screen id",   ARGUMENT_SCREENID}
        }
    },
    {   TOOLBOX_LIST_SURFACE, 1,
        {
            {true, "lid:","par = layer id in hex",   ARGUMENT_LAYERID}
        }
    },
    {   TOOLBOX_REMOVE_LAYER, 1,
        {
            {true, "lid:","par = layer id in hex",   ARGUMENT_LAYERID}
        }
    },
    {   TOOLBOX_REMOVE_SURFACE, 1,
        {
            {true, "sid:","par = surface id in hex",   ARGUMENT_SURFACEID}
        }
    },
    {   TOOLBOX_PROPERTIES_LAYER, 1,
        {
            {true, "lid:","par = layer id in hex",   ARGUMENT_LAYERID}
        }
    },
    {   TOOLBOX_PROPERTIES_SURFACE, 1,
        {
            {true, "sid:","par = surface id in hex",   ARGUMENT_SURFACEID}
        }
    },
    {   TOOLBOX_CHANGE_LAYER, 4,
        {
            {true, "lid:","par = layer id in hex",     ARGUMENT_LAYERID},
            {true,"dest:","par = dest rec in x,y,w,h",     ARGUMENT_DEST},
            {true,"o:","par = opacity 0.0 ... 1.0",        ARGUMENT_OPACITY},
            {true,"v:","par = visibility 0 / 1",        ARGUMENT_VISIBILITY}
        }
    },
    {   TOOLBOX_CHANGE_SURFACE, 5,
        {
            {true, "sid:","par = surface id in hex",     ARGUMENT_SURFACEID},
            {true,"src:","par = src rec in x,y,w,h",      ARGUMENT_SOURCE},
            {true,"dest:","par = dest rec in x,y,w,h",     ARGUMENT_DEST},
            {true,"o:","par = opacity 0.0 ... 1.0",        ARGUMENT_OPACITY},
            {true,"v:","par = visibility 0 / 1",        ARGUMENT_VISIBILITY}
        }
    },
    {   
        TOOLBOX_SET_DISPLAY_RO, 2,
        {
            {true,"screen:","par = screenid",   ARGUMENT_SCREENID},
            {true,"ro:","par = renderorder in L_1,...,L_N",       ARGUMENT_LAYER_RO}
        }
    }

};

static const struct option toolbox_options[] = 
{
    {"add-layer",           required_argument,      0,   0},
    {"add-surface",         required_argument,      0,   1},
    {"list-layer",          required_argument,      0,   2},
    {"list-surface",        required_argument,      0,   3},
    {"remove-layer",        required_argument,      0,   4},
    {"remove-surface",      required_argument,      0,   5},
    {"properties-layer",    required_argument,      0,   6},
    {"properties-surface",  required_argument,      0,   7},
    {"change-layer",        required_argument,      0,   8},
    {"change-surface",      required_argument,      0,   9},
    {"set-display-ro",      required_argument,      0,   10},
    {"set-layer-ro",        required_argument,      0,   11},
    {0,0,0,0}
};


typedef struct s_global_parameter 
{
    t_ilm_nativehandle  windowid;
    t_ilm_surface       surfaceid;
    t_ilm_layer         layerid;
    t_ilm_int           screenid;    
    t_ilm_int           src[4];
    t_ilm_int           dest[4];
    t_ilm_bool          visibility;
    t_ilm_float         opacity;
    ilmPixelFormat      pixelformat;
    t_ilm_surface*      surfacero;
    t_ilm_int           surfacero_length;
    t_ilm_layer*        layerro;
    t_ilm_int           layerro_length;
    toolbox_state state;
} t_param_struct;


const char* parseParameters(char* argv, const char* argument, const char* beginterm , const char* endterm) 
{
    const char* cresult = NULL;
    std::string result = std::string(argv);
    printf("Argument is %s\n",result.c_str());
    size_t pos = result.find(std::string (argument));
    if (pos != std::string::npos) 
    {
        size_t endpos = 0;
        size_t beginpos = result.find( std::string (beginterm), pos + strlen(argument) );
        if ( pos != std::string::npos ) 
        {
            beginpos += strlen(beginterm);
            endpos = result.find( std::string (endterm), beginpos); 
            if ( endpos != std::string::npos ) 
            {
                cresult = (result.substr(beginpos,endpos-beginpos)).c_str();
                printf("Value is %s\n",cresult);
            }
        }
    }
    return cresult;
}

void setDisplayRenderOrder(t_param_struct *pStruct) 
{
    ilm_displaySetRenderOrder(pStruct->screenid, pStruct->layerro, pStruct->layerro_length);    
    ilm_commitChanges();
}




bool fillLayerRenderOrder(const char * param, t_param_struct* pStruct) 
{
    bool result = true;
    char * pRes = new char[strlen(param)+1];
    strcpy(pRes,param);
    int i = 0;
    pRes = strtok (pRes,",");
    while (pRes != NULL)
    {
        pRes = strtok (NULL, ",");
        i++;
    }
    if (i ==  0) 
    {
        result = false;
    } 
    else 
    {
        delete[] pRes;
        pRes = new char[strlen(param)+1];
        strcpy(pRes,param);
        pStruct->layerro = new t_ilm_layer[i];
        pStruct->layerro_length = i;
        i = 0;
        pRes = strtok (pRes,",");
        printf("Set Renderorder : [\n");
        while (pRes != NULL)
        {
            sscanf(pRes,"%x",&pStruct->layerro[i]);
            printf("\tLayer 0x%08x\n",pStruct->layerro[i]);
            pRes = strtok (NULL, ",");
            i++;
        }
        printf("]\n");
    }
    delete[] pRes;
    return result;
}

bool fillDimension(const char * param, t_ilm_int *pArray) 
{
    t_ilm_int *tempArray = new t_ilm_int[4];
    bool result = true;
    char * pRes = new char[strlen(param)+1];
    strcpy(pRes,param);
    int i = 0;
    pRes = strtok (pRes,",");
    while (pRes != NULL)
    {
        tempArray[i++] = atoi(pRes);
        pRes = strtok (NULL, ",");
    }
    if (i<4) 
    {
        result = false;
    } 
    else 
    {
        memcpy(pArray,tempArray,4*sizeof(t_ilm_int));
        printf("Dimension Parameter successfully filled %i %i %i %i\n",pArray[0],pArray[1],pArray[2],pArray[3]);        
    }
    delete[] pRes;
    delete[] tempArray;
    return result;
}

void printUsage() 
{
    int i,j = 0;
    puts(USAGE_DESCRIPTION);
    for (j=0;j<TOOLBOX_INIT_STATE;j++)
    {
        t_state_params curParam = state_params[j];
        i = 0;
        printf("\t--%s\n\n",toolbox_options[j].name);       
        while ( i < curParam.numberArguments ) 
        {
            if (curParam.params[i].required) 
            {
                printf("\t\t%s[par], \trequired\n",curParam.params[i].argument);
            }
            else 
            {
                printf("\t\t%s[par], \toptional\n",curParam.params[i].argument);
            }
            printf("\t\t%s\n\n",curParam.params[i].description);
            i++;
        }
        printf("\n");
    }
    
     printf("Example LayerManagerToolBox --change-surface sid:[0xa],src:[0,0,320,240],dest:[0,0,800,480],o:[1.0],v:[1]\n\n");
}



bool initParamStruct(t_param_struct* pStruct,char* argv) 
{
    bool result = true;
    toolbox_state activeState = pStruct->state;
    int i = 0;
    t_state_params curParam = state_params[activeState];
    
    while ( result && i < curParam.numberArguments ) 
    {
        bool found = false;
        const char * param = NULL;
        switch (curParam.params[i].type) 
        {
            case ARGUMENT_LAYERID :
                param = parseParameters(argv,curParam.params[i].argument,"[","]");
                if ( param != 0 ) 
                {
                    found = true;
                    sscanf(param,"%x",&pStruct->layerid);
                }
                printf("Layer ID is 0x%08x\n",pStruct->layerid);                
            break;
            case ARGUMENT_SURFACEID :
                param = parseParameters(argv,curParam.params[i].argument,"[","]");
                if ( param != 0 ) 
                {
                    found = true;
                    sscanf(param,"%x",&pStruct->surfaceid);
                }
                printf("Surface id is 0x%08x\n",pStruct->surfaceid);                
            break;
            case ARGUMENT_SCREENID:
                param = parseParameters(argv,curParam.params[i].argument,"[","]");
                if ( param != 0 ) 
                {
                    found = true;
                    pStruct->screenid = atoi(param);
                }
                printf("Screen id is 0x%08x\n",pStruct->screenid);                
            break;
            case ARGUMENT_OPACITY:
                param = parseParameters(argv,curParam.params[i].argument,"[","]");
                if ( param != 0 ) 
                {
                    found = true;
                    pStruct->opacity=atof(param);
                }            
                printf("Opacity is %1.2f\n",pStruct->opacity);                
            break;
            case ARGUMENT_WINDOW:
                param = parseParameters(argv,curParam.params[i].argument,"[","]");
                if ( param != 0 ) 
                {
                    found = true;
                    sscanf(param,"%x",&pStruct->windowid);
                }
                printf("Window id is 0x%08x\n",pStruct->windowid);                
            break;
            case ARGUMENT_SURFACE_RO:
            break;
            case ARGUMENT_LAYER_RO:
                param = parseParameters(argv,curParam.params[i].argument,"[","]");
                if ( param != 0 ) 
                {
                    found = true;
                    if ( !fillLayerRenderOrder(param,pStruct) ) 
                    {
                        found = false;
                        printf("Layer Render Order Arguments not complete !\n");
                    }
                }
            break;
            case ARGUMENT_VISIBILITY:
                param = parseParameters(argv,curParam.params[i].argument,"[","]");
                if ( param != 0 ) 
                {
                    found = true;
                    pStruct->visibility = atoi(param);
                }            
                printf("Visibility is %i\n",pStruct->visibility);                
            break;
            case ARGUMENT_DEST:
                param = parseParameters(argv,curParam.params[i].argument,"[","]");
                if ( param != 0 ) 
                {
                    found = true;
                    if ( !fillDimension(param,&pStruct->dest[0]) ) 
                    {
                        found = false;
                        printf("Dest Arguments not complete !\n");
                    }
                }
            break;
            case ARGUMENT_SOURCE:
                param = parseParameters(argv,curParam.params[i].argument,"[","]");
                if ( param != 0 ) 
                {
                    found = true;
                    if ( !fillDimension(param,&pStruct->src[0]) ) 
                    {
                        found = false;
                        printf("Src Arguments not complete !\n");
                    }
                }
            break;            
            case ARGUMENT_PIXELFORMAT:
                param = parseParameters(argv,curParam.params[i].argument,"[","]");
                if ( param != 0 ) 
                {
                    found = true;
                    pStruct->pixelformat = (ilmPixelFormat)atoi(param);
                }
                printf("Pixelformat is %i\n",pStruct->pixelformat);                                
            break;            
            
            default:
                found = false;
                printf("Argument not supported\n");
        }
        if ( (curParam.params[i].required==true) && (found==false) )
        {
            printf("Required Argument %s not found\n",curParam.params[i].argument);
            result = false;
        } 
        i++;
    }
    return result;
}
ilmErrorTypes removeSurface(t_param_struct* param_struct) 
{  
    ilmErrorTypes error;
    ilm_surfaceRemove(param_struct->surfaceid);
    ilm_commitChanges();
    return error;
}

ilmErrorTypes removeLayer(t_param_struct* param_struct) 
{  
    ilmErrorTypes error;
    ilm_layerRemove(param_struct->layerid);
    ilm_commitChanges();
    return error;
}

ilmErrorTypes listLayer(t_param_struct* param_struct) 
{  
    ilmErrorTypes error;
    t_ilm_int   layerLength = 0;
    t_ilm_layer* layerIds = NULL;
    int i=0;
    if ( param_struct->screenid != -1 )
    {
        error = ilm_getLayerIDsOnScreen(param_struct->screenid, &layerLength,&layerIds);
    } else {
        error = ilm_getLayerIDs(&layerLength,&layerIds);
    }
    if (layerLength > 0 && error == ILM_SUCCESS) 
    { 
        printf("List Layer Ids for screen %i\n[\n",param_struct->screenid);
        for (i=0;i<layerLength;i++) 
        {   
            t_ilm_int surfaceLength=0;
            t_ilm_surface* surfaceIds = NULL;
            int j=0;
            printf("\tLayer : 0x%08x\n",layerIds[i]);
            ilm_getSurfaceIDsOnLayer(layerIds[i], &surfaceLength,&surfaceIds);
            if (surfaceLength > 0 ) 
            {    
                for (j=0;j<surfaceLength;j++) 
                {   
                    printf("\t\tSurface : 0x%08x\n",surfaceIds[j]);
                }
                delete[] surfaceIds;
            }
        }
        printf("]\n");
        delete [] layerIds;
    }
    return error;
}

ilmErrorTypes listSurface(t_param_struct* param_struct) 
{  
    ilmErrorTypes error;
    t_ilm_int surfaceLength=0;
    t_ilm_surface* surfaceIds = NULL;
    int j=0;
    printf("List Surface ids on Layer : 0x%08x\n[\n",param_struct->layerid);
    ilm_getSurfaceIDsOnLayer(param_struct->layerid, &surfaceLength,&surfaceIds);
    if (surfaceLength > 0 ) 
    {    
        for (j=0;j<surfaceLength;j++) 
        {   
            printf("\tSurface : 0x%08x\n",surfaceIds[j]);
        }
        delete[] surfaceIds;
        printf("]\n");
    }
    return error;
}


ilmErrorTypes changeSurfaceValues(t_param_struct* param_struct) 
{  
    ilmErrorTypes error;
    error = ilm_surfaceSetDestinationRectangle(param_struct->surfaceid, param_struct->dest[0],param_struct->dest[1], 
                                                                        param_struct->dest[2],param_struct->dest[3]);

    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    printf("set surface src region\n");
    error = ilm_surfaceSetSourceRectangle(param_struct->surfaceid, param_struct->src[0],param_struct->src[1], 
                                                                        param_struct->src[2],param_struct->src[3]);

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

ilmErrorTypes changeLayerValues(t_param_struct* param_struct) 
{  
    ilmErrorTypes error;
    error = ilm_layerSetDestinationRectangle(param_struct->layerid, param_struct->dest[0],param_struct->dest[1], 
                                                                    param_struct->dest[2],param_struct->dest[3]);

    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    printf("set layer src region\n");
    error = ilm_layerSetSourceRectangle(param_struct->layerid, param_struct->src[0],param_struct->src[1], 
                                                                        param_struct->src[2],param_struct->src[3]);

    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    printf("Set layer visible\n");
    error = ilm_layerSetVisibility(param_struct->layerid, param_struct->visibility);
    printf("Set layer opacity\n");
    error = ilm_layerSetOpacity(param_struct->layerid, param_struct->opacity);

    //if (error == ILM_FAILED) return ILM_FALSE;

    printf("commit\n");
    error = ilm_commitChanges();

    return error;
}

ilmErrorTypes createLayer(t_param_struct* param_struct) 
{   
    ilmErrorTypes error;
    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    printf("create layer %lu\n", param_struct->layerid);
    error = ilm_layerCreate( &param_struct->layerid );

    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    error = changeLayerValues(param_struct);

   // TODO: if (error == ILM_FAILED) return ILM_FALSE;


    printf("commit\n");
    error = ilm_commitChanges();
    return error;
}


ilmErrorTypes addWindowToLayer(t_param_struct* param_struct) 
{   
    ilmErrorTypes error;
    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    printf("create a surface %lu\n", (t_ilm_nativehandle) param_struct->windowid);
    error = ilm_surfaceCreate( (t_ilm_nativehandle) param_struct->windowid, param_struct->src[3],param_struct->src[4],
            param_struct->pixelformat, &param_struct->surfaceid);

    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    error = changeSurfaceValues(param_struct);

    // TODO: if (error == ILM_FAILED) return ILM_FALSE;

    printf("add surface to layer\n");
    error = ilm_layerAddSurface(param_struct->layerid,param_struct->surfaceid);


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
        pStruct->src = {0,0,320,240};
        pStruct->dest = {0,0,320,240};
        pStruct->pixelformat = ILM_PIXELFORMAT_RGBA_8888;
        pStruct->layerid = 2000;
        pStruct->screenid = -1;
        pStruct->surfacero = NULL;
        pStruct->surfacero_length = 0;
        pStruct->layerro = NULL;
        pStruct->layerro_length = 0;
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
    while (1)
    {
        int option = 0;        
        int result = getopt_long(   argc, argv,"::?",
                                    toolbox_options, &option);
        char* param = 0;
        bool success = true;
        if ( result == -1 ) break;

        switch (result)
        {
        case TOOLBOX_ADD_LAYER :
            param_struct->state = TOOLBOX_ADD_LAYER;
            success = initParamStruct(param_struct,optarg);                        
            break;                     
        case TOOLBOX_ADD_SURFACE :
            param_struct->state = TOOLBOX_ADD_SURFACE;
            success = initParamStruct(param_struct,optarg);                      
            break;
        case TOOLBOX_CHANGE_SURFACE :
            param_struct->state = TOOLBOX_CHANGE_SURFACE;
            success = initParamStruct(param_struct,optarg);                      
            break;
        case TOOLBOX_CHANGE_LAYER :
            param_struct->state = TOOLBOX_CHANGE_LAYER;
            success = initParamStruct(param_struct,optarg);                      
            break;
        case TOOLBOX_REMOVE_SURFACE :
            param_struct->state = TOOLBOX_REMOVE_SURFACE;
            success = initParamStruct(param_struct,optarg);                      
            break;
        case TOOLBOX_REMOVE_LAYER :
            param_struct->state = TOOLBOX_REMOVE_LAYER;
            success = initParamStruct(param_struct,optarg);                      
            break;            
        case TOOLBOX_LIST_LAYER :
            param_struct->state = TOOLBOX_LIST_LAYER;
            success = initParamStruct(param_struct,optarg);                      
            break;            
        case TOOLBOX_LIST_SURFACE :
            param_struct->state = TOOLBOX_LIST_SURFACE;
            success = initParamStruct(param_struct,optarg);                      
            break;  
        case TOOLBOX_SET_DISPLAY_RO :
            param_struct->state = TOOLBOX_SET_DISPLAY_RO;
            success = initParamStruct(param_struct,optarg);                      
            break;                          
        case '?':   
        default:
            printUsage();
            exit(-1);
        }
        if (!success) 
        {
            printUsage();
            exit(-1);
        }
    }
}

int main(int argc, char **argv)
{
    t_param_struct pStruct;
    if (argc == 1) 
    {
        printUsage();
        return (-1);
    }

    if (ILM_SUCCESS != init_toolbox(&pStruct))
    {
        return -1;
    }
    parseCommandLine(&pStruct,argc, (char**) argv);
    switch(pStruct.state) 
    {
         case TOOLBOX_ADD_SURFACE :
            addWindowToLayer(&pStruct);
            break;
         case TOOLBOX_ADD_LAYER :
            createLayer(&pStruct);
            break;
         case TOOLBOX_CHANGE_LAYER :
            changeLayerValues(&pStruct);
            break;
         case TOOLBOX_CHANGE_SURFACE :
            changeSurfaceValues(&pStruct);
            break;            
         case TOOLBOX_LIST_LAYER :
            listLayer(&pStruct);
            break;
         case TOOLBOX_LIST_SURFACE :
            listSurface(&pStruct);
            break;
         case TOOLBOX_REMOVE_LAYER :
            removeLayer(&pStruct);
            break;
         case TOOLBOX_REMOVE_SURFACE :
            removeSurface(&pStruct);
            break;            
         case TOOLBOX_SET_DISPLAY_RO :
            setDisplayRenderOrder(&pStruct);
            break;            
            
        default:
         printf("Other options currently not implemented\n");
    }
    shutdown_toolbox();
}
