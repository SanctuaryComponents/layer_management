/***************************************************************************
*
* Copyright 2010,2011 BMW Car IT GmbH
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*		http://www.apache.org/licenses/LICENSE-2.0
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

#define LAYERWIDTH 800
#define LAYERHEIGHT 480

int main(int argc, char **argv)
{
	ilmErrorTypes error = ILM_FAILED;
	int length,i = 0;
	t_ilm_int LayerWidth = LAYERWIDTH;
	t_ilm_int LayerHeight = LAYERHEIGHT;
	t_ilm_layer *layerIds;
	printf("Calling ilm_init ... \n");
	error = ilm_init();
	if (error == ILM_SUCCESS)
	{
          printf("Calling ilm_init successfull.\n");
          ilm_getScreenResolution(0,&LayerWidth, &LayerHeight);
          if (argc == 2 && atoi(argv[1]) == 1)
               {
                 printf("Do Scene Layout \n");

                 ilm_surfaceSetDestinationRectangle(SURFACE_EXAMPLE_VIDEO_APPLICATION,0,0,LayerWidth,LayerHeight);
                 ilm_surfaceSetSourceRectangle(SURFACE_EXAMPLE_VIDEO_APPLICATION,107,0,426,360);
                 ilm_surfaceSetDestinationRectangle(SURFACE_EXAMPLE_GDTESTENV_APPLICATION_1,LayerWidth-160,0,160,120);
                 ilm_surfaceSetDestinationRectangle(SURFACE_EXAMPLE_EGLX11_APPLICATION,LayerWidth-320,LayerHeight-240,320,240);
                 ilm_commitChanges();
                 sleep(20);
                 t_ilm_string filename = "/var/ilmScreenShot.bmp";
                 ilm_takeScreenshot(0, filename);
                 ilm_layerSetOpacity(LAYER_EXAMPLE_VIDEO_APPLICATIONS,0.0);
                 ilm_layerSetOpacity(LAYER_EXAMPLE_GLES_APPLICATIONS,1.0);
                 ilm_layerSetOpacity(LAYER_EXAMPLE_X_APPLICATIONS,1.0);
                 ilm_commitChanges();
               }
          else if (argc == 2 && atoi(argv[1]) == 2)
               {
                 printf("Remove Scene Layout \n");
                 ilm_layerRemove(LAYER_EXAMPLE_VIDEO_APPLICATIONS);
                 ilm_layerRemove(LAYER_EXAMPLE_GLES_APPLICATIONS);
                 ilm_layerRemove(LAYER_EXAMPLE_X_APPLICATIONS);
               }

          else {
		t_ilm_layer layer[3] = {LAYER_EXAMPLE_VIDEO_APPLICATIONS,LAYER_EXAMPLE_GLES_APPLICATIONS,LAYER_EXAMPLE_X_APPLICATIONS};
		printf("Creating Layer\n");
		for ( i=0;i<3;i++ )
		  {
		    error = ilm_layerCreate(&layer[i]);
		    error = ilm_layerSetDestinationRectangle(layer[i],0,0,LayerWidth,LayerHeight);
		  }
		ilm_displaySetRenderOrder(0,&layer[0],3);
          }
          ilm_commitChanges();

          printf("Calling ilm_destroy ... \n");
          error = ilm_destroy();
          if (error = ILM_SUCCESS)
          {
                  printf("Calling ilm_destroy successful, close\n");
          }
	}
}
