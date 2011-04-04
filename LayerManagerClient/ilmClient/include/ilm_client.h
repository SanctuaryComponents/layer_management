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
#ifndef _ILM_CLIENT_H_
#define _ILM_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "ilm_types.h"

/* Initializes the IVI LayerManagement Client.
 *
 * @return ILM_FAILED if a connection can not be established to the services.
 */
ilmErrorTypes ilm_init();

/* Get the screen resolution of a specific screen from the Layermanagement
 *
 * @return ILM_FAILED if the client can not get the resolution.
 */
ilmErrorTypes ilm_getScreenResolution(t_ilm_uint screenID, t_ilm_uint* width, t_ilm_uint* height);

/* Get the surface properties from the Layermanagement
 *
 * @return ILM_FAILED if the client can not get the resolution.
 */
ilmErrorTypes ilm_getPropertiesOfSurface(t_ilm_uint surfaceID, struct ilmSurfaceProperties* surfaceProperties);

/* Get the layer properties from the Layermanagement
 *
 * @return ILM_FAILED if the client can not get the resolution.
 */
ilmErrorTypes ilm_getPropertiesOfLayer(t_ilm_uint layerID, struct ilmLayerProperties* layerProperties);

/* Get the number of hardware layers of a screen
 *
 * @return ILM_FAILED if the client can not get the resolution.
 */
ilmErrorTypes ilm_getNumberOfHardwareLayers(t_ilm_uint screenID, t_ilm_uint* NumberOfHardwareLayers);

/* Get the screen Ids
 *
 * @return ILM_FAILED if the client can not get the resolution.
 */
ilmErrorTypes ilm_getScreenIDs(t_ilm_uint* numberOfIDs, t_ilm_uint** IDs);

/* Destroys the IVI LayerManagement Client.
 *
 * @return ILM_FAILED if the client can not be closed or was not initialized.
 */
ilmErrorTypes ilm_destroy();

/* Get all LayerIds which are currently registered and managed by the services
 *
 * @param length        Pointer where length of ids array should be stored
 * @param array         Array where the ids should be stored,
 *                      the array will be allocated inside
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_getLayerIDs(t_ilm_int* length,t_ilm_layer** array);

/* Get all LayerIds of the given screen
 *
 * @param screenID		The id of the screen to get the layer IDs of
 * @param length        Pointer where length of ids array should be stored
 * @param array         Array where the ids should be stored,
 *                      the array will be allocated inside
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_getLayerIDsOnScreen(t_ilm_uint screenID, t_ilm_int* length,t_ilm_layer** array);

/* Get all SurfaceIDs which are currently registered and managed by the services
 *
 * @param length        Pointer where length of ids array should be stored
 * @param array         Array where the ids should be stored,
 *                      the array will be allocated inside
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_getSurfaceIDs(t_ilm_int* length,t_ilm_surface** array);

/* Get all LayerGroupIds which are currently registered and managed by the services
 *
 * @param length        Pointer where array length of ids should be stored
 * @param array         Array where the id should be stored,
 *                      the array will be allocated inside
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_getLayerGroupIDs(t_ilm_int* length,t_ilm_layergroup** array);

/* Get all SurfaceGroupIds which are currently registered and managed by the services
 *
 * @param length        Pointer where array length of ids should be stored
 * @param array         Array where the id should be stored,
 *                      the array will be allocated inside
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_getSurfaceGroupIDs(t_ilm_int* length,t_ilm_surfacegroup** array);

/* Get all SurfaceIds which are currently registered to a given layer and are managed by the services
 *
 * @param layer         Id of the Layer whose surfaces are to be returned
 * @param length        Pointer where the array length of ids should be stored
 * @param array         Array where the surface id should be stored,
 *                      the array will be allocated inside
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_getSurfaceIDsOnLayer(t_ilm_layer layer,t_ilm_int* length,t_ilm_surface** array);

/* Create a layer which should be managed by the service
 *
 * @param layerId       pointer where the id should be/is stored. It is possible
 *                      to set a id from outside, 0 will create a new id.
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerCreate(t_ilm_layer* layerId);

/* Removes a layer which is currently managed by the service
 *
 * @param layerId       Layer to be removed
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerRemove(t_ilm_layer layerId);

/* Add a surface to a layer which is currently managed by the service
 *
 * @param layerId       Id of layer which should host the surface.
 * @param surfaceId     Id of surface which should be added to the layer.
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerAddSurface(t_ilm_layer layerId,t_ilm_surface surfaceId);

/* Removes a surface from a layer which is currently managed by the service
 *
 * @param layerId       Id of the layer which contains the surface.
 * @param surfaceId     Id of the surface which should be removed from the layer.
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerRemoveSurface(t_ilm_layer layerId,t_ilm_surface surfaceId);

/* Get the current type of the layer.
 *
 * @param layerId       Id of the layer.
 * @param layerType     pointer to the layerType where the result should be stored.
 * @see ilmLayerType    for information on supported types
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetType(t_ilm_layer layerId,ilmLayerType* layerType);

/* Set the visibility of a layer. If a layer is not visible, the layer and its
 * surfaces will not be rendered.
 *
 * @param layerId               Id of the layer.
 * @param newVisibility         ILM_TRUE sets layer visible, ILM_FALSE disables the visibility.
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetVisibility(t_ilm_layer layerId,t_ilm_bool newVisibility);

/* Get the visibility of a layer. If a layer is not visible, the layer and its
 * surfaces will not be rendered.
 *
 * @param layerId            Id of layer.
 * @param visibility         pointer where the visibility of the layer should be stored
 *                           ILM_TRUE if the Layer is visible,
 *                           ILM_FALSE if the visibility is disabled.
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetVisibility(t_ilm_layer layerId,t_ilm_bool *visibility);

/* Set the opacity of a layer.
 *
 * @param layerId         Id of the layer.
 * @param opacity         0.0 means the layer is fully transparent,
 *                        1.0 means the layer is fully opaque
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetOpacity(t_ilm_layer layerId,t_ilm_float opacity);

/* Get the opacity of a layer.
 *
 * @param layerId         Id of the layer to obtain the opacity of.
 * @param opacity         pointer where the layer opacity should be stored.
 *                        0.0 means the layer is fully transparent,
 *                        1.0 means the layer is fully opaque
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetOpacity(t_ilm_layer layerId,t_ilm_float *opacity);

/* Set the area of a layer which should be used for the rendering. Only this part will be visible.
 *
 * @param layerId         Id of the layer.
 * @param x               horizontal start position of the used area
 * @param y               vertical start position of the used area
 * @param width           width of the area
 * @param height          height of the area
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetSourceRectangle(t_ilm_layer layerId,t_ilm_uint x,t_ilm_uint y,t_ilm_uint width,t_ilm_uint height);

/* Set the destination area on the display for a layer. The layer will be scaled and positioned to this rectangle for rendering
 *
 * @param layerId         Id of the layer.
 * @param x               horizontal start position of the used area
 * @param y               vertical start position of the used area
 * @param width           width of the area
 * @param height          height of the area
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetDestinationRectangle(t_ilm_layer layerId,t_ilm_int x,t_ilm_int y,t_ilm_int width,t_ilm_int height);


/* Get the horizontal and vertical dimension of the layer.
 *
 * @param layerId         Id of layer.
 * @param dimension       pointer to an array where the dimension should be stored.
 *                        dimension[0]=width, dimension[1]=height
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetDimension(t_ilm_layer layerId,t_ilm_uint *dimension);

/* Set the horizontal and vertical dimension of the layer.
 *
 * @param layerId         Id of layer.
 * @param dimension       pointer to an array where the dimension is stored.
 *                        dimension[0]=width, dimension[1]=height
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetDimension(t_ilm_layer layerId, t_ilm_uint *dimension);


/* Get the horizontal and vertical position of the layer.
 *
 * @param layerId         Id of layer.
 * @param dimension       pointer to an array where the position should be stored.
 *                        dimension[0]=width, dimension[1]=height
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetPosition(t_ilm_layer layerId, t_ilm_uint *position);

/* Sets the horizontal and vertical position of the layer.
 *
 * @param layerId         Id of layer.
 * @param position	      pointer to an array where the position is stored.
 *                        dimension[0]=x, dimension[1]=y
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetPosition(t_ilm_layer layerId, t_ilm_uint *position);

/* Sets the orientation of a layer.
 *
 * @param layerId         Id of layer.
 * @param orientation     Orientation of the layer.
 * @see ilmOrientation
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetOrientation(t_ilm_layer layerId, ilmOrientation orientation);

/* Gets the orientation of a layer.
 *
 * @param layerId         Id of layer.
 * @param orientation     Address where orientation of the layer should be stored.
 * @see ilmOrientation
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetOrientation(t_ilm_layer layerId, ilmOrientation *orientation);

/* Sets the color value which defines the transparency value.
 *
 * @param layerId   Id of layer.
 * @param color     array of the color value which is defined in red,green, blue
 * @todo This method is currently not implemented.
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetChromaKey(t_ilm_layer layerId, t_ilm_int* color);

/* Sets render order of surfaces within one layer
 *
 * @param layerId       Id of layer.
 * @param surfaceId     array of surface ids
 * @param number     	Number of elements in the given array of ids
 * @todo This method is currently not implemented.
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetRenderOrder(t_ilm_layer layerId,t_ilm_layer *surfaceId, t_ilm_int number);

/* Get the capabilities of a layer
 *
 * @param layerId       Id of the layer to obtain the capabilities of
 * @param capabilities  The address where the capabilites are returned
 * @todo This method is currently not implemented.
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetCapabilities(t_ilm_layer layerId, t_ilm_layercapabilities *capabilities);

/* Get the possible capabilities of a layertype
 *
 * @param layerType     The layertype to obtain the capabilities of
 * @param capabilities  The address where the capabilites are returned
 * @todo This method is currently not implemented.
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerTypeGetCapabilities(ilmLayerType layerType, t_ilm_layercapabilities *capabilities);

/* Create a layergroup
 *
 * @param group       The id of the created layergroup is returned in this parameter
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layergroupCreate(t_ilm_layergroup *group);

/* Remove a layergroup
 *
 * @param group       The layergroup to be removed
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layergroupRemove(t_ilm_layergroup group);

/* Add a layer to a layergroup
 *
 * @param group       The layergroup to add the layer
 * @param layer       The layer to add to the group
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layergroupAddLayer(t_ilm_layergroup group, t_ilm_layer layer);

/* Remove a layer from a layergroup
 *
 * @param group       The layergroup to remove the layer from
 * @param layer       The layer to be removed from the group
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layergroupRemoveLayer(t_ilm_layergroup group, t_ilm_layer layer);

/* Set the visibility of a layergroup. If a layergroup is not visible, the layers and their
 * surfaces will not be rendered.
 *
 * @param group               Id of the layergroup to set the visibility of
 * @param newVisibility       ILM_TRUE sets layergroup visible, ILM_FALSE disables the visibility.
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layergroupSetVisibility(t_ilm_layergroup group, t_ilm_bool newVisibility);

/* Set the opacity of a layergroup.
 *
 * @param layerId         Id of the layergroup to set the opacity of.
 * @param opacity         0.0 means the layergroup is fully transparent,
 *                        1.0 means the layergroup is fully opaque
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layergroupSetOpacity(t_ilm_layergroup group, t_ilm_float opacity);

/* Create a surface
 *
 * @param nativehandle      The native windowsystem's handle for the surface
 * @param width       		The original width of the surface
 * @param height       		The original height of the surface
 * @param pixelFormat       The pixelformat to be used for the surface
 * @param surfaceId       	The ID of the newly created surface is returned in this parameter
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceCreate(t_ilm_nativehandle nativehandle, t_ilm_int width, t_ilm_int height, ilmPixelFormat pixelFormat, t_ilm_surface *surfaceId);

/* Remove a surface
 *
 * @param surfaceId      	The id of the surface to be removed
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceRemove(const t_ilm_surface surfaceId);

/* Set the visibility of a surface. If a surface is not visible it will not be rendered.
 *
 * @param surfaceId           Id of the surface to set the visibility of
 * @param newVisibility       ILM_TRUE sets surface visible, ILM_FALSE disables the visibility.
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetVisibility(t_ilm_surface surfaceId, t_ilm_bool newVisibility);

/* Get the visibility of a surface. If a surface is not visible, the surface
 * will not be rendered.
 *
 * @param surfaceId          Id of the surface to get the visibility of.
 * @param visibility         pointer where the visibility of a surface should be stored
 *                           ILM_TRUE if the surface is visible,
 *                           ILM_FALSE if the visibility is disabled.
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceGetVisibility(t_ilm_surface surfaceId, t_ilm_bool *visibility);

/* Set the opacity of a surface.
 *
 * @param surfaceId       Id of the surface to set the opacity of.
 * @param opacity         0.0 means the surface is fully transparent,
 *                        1.0 means the surface is fully opaque
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetOpacity(const t_ilm_surface surfaceId, t_ilm_float opacity);

/* Get the opacity of a surface.
 *
 * @param surfaceId       Id of the surface to get the opacity of.
 * @param opacity         pointer where the surface opacity should be stored.
 *                        0.0 means the surface is fully transparent,
 *                        1.0 means the surface is fully opaque
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceGetOpacity(const t_ilm_surface surfaceId, t_ilm_float *opacity);

/* Set the area of a surface which should be used for the rendering.
 *
 * @param surfaceId       Id of surface.
 * @param x               horizontal start position of the used area
 * @param y               vertical start position of the used area
 * @param width           width of the area
 * @param height          height of the area
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetSourceRectangle(t_ilm_surface surfaceId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height);

/* Set the destination area of a surface within a layer for rendering. The surface will be scaled to this rectangle for rendering.
 *
 * @param surfaceId       Id of surface.
 * @param x               horizontal start position of the used area
 * @param y               vertical start position of the used area
 * @param width           width of the area
 * @param height          height of the area
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetDestinationRectangle(t_ilm_surface surfaceId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height);

/* Get the horizontal and vertical dimension of the surface.
 *
 * @param surfaceId       Id of surface.
 * @param dimension       pointer to an array where the dimension should be stored.
 *                        dimension[0]=width, dimension[1]=height
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceGetDimension(t_ilm_surface surfaceId,t_ilm_uint *dimension);

/* Set the horizontal and vertical dimension of the surface.
 *
 * @param surfaceId       Id of surface.
 * @param dimension       pointer to an array where the dimension is stored.
 *                        dimension[0]=width, dimension[1]=height
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetDimension(t_ilm_surface surfaceId, t_ilm_uint *dimension);

/* Get the horizontal and vertical position of the surface.
 *
 * @param surfaceId       Id of surface.
 * @param position        pointer to an array where the position should be stored.
 *                        position[0]=x, position[1]=y
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceGetPosition(t_ilm_surface surfaceId, t_ilm_uint *position);

/* Sets the horizontal and vertical position of the surface.
 *
 * @param surfaceId      Id of surface.
 * @param position       pointer to an array where the position is stored.
 *                       position[0]=x, position[1]=y
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetPosition(t_ilm_surface surfaceId, t_ilm_uint *position);

/* Sets the orientation of a surface.
 *
 * @param surfaceId       Id of surface.
 * @param orientation     Orientation of the surface.
 * @see ilmOrientation
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetOrientation(t_ilm_surface surfaceId, ilmOrientation orientation);

/* Gets the orientation of a surface.
 *
 * @param surfaceId       Id of surface.
 * @param orientation     Address where orientation of the surface should be stored.
 * @see ilmOrientation
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceGetOrientation(t_ilm_surface surfaceId, ilmOrientation *orientation);

/* Gets the pixelformat of a surface.
 *
 * @param surfaceId       Id of surface.
 * @param pixelformat     Pointer where the pixelformat of the surface should be stored
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceGetPixelformat(t_ilm_layer surfaceId, ilmPixelFormat *pixelformat);

/* Sets the color value which defines the transparency value of a surface.
 *
 * @param surfaceId	    Id of the surface to set the chromakey of.
 * @param color     	array of the color value which is defined in red, green, blue
 * @todo 				This method is currently not implemented.
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetChromaKey(t_ilm_surface surfaceId, t_ilm_int* color);

/* Inform that a certain rectangle of a surface has been invalidated and must consequently be redrawn
 *
 * @param surfaceId	    Id of the surface
 * @param color     	array of the color value which is defined in red, green, blue
 * @todo 				This method is currently not implemented.
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceInvalidateRectangle(t_ilm_surface surfaceId);

/* Create a surfacegroup
 *
 * @param group       The created surfacegroup is returned in this parameter
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfacegroupCreate(t_ilm_surfacegroup *group);

/* Remove a surfacegroup
 *
 * @param group       The surfacegroup to be removed
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfacegroupRemove(t_ilm_surfacegroup group);

/* Add a surface to a surfacegroup
 *
 * @param group       The surfacegroup to add the surface
 * @param surface     The surface to add to the group
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfacegroupAddSurface(t_ilm_surfacegroup group, t_ilm_surface surface);

/* Remove a surface from a surfacegroup
 *
 * @param group       The surfacegroup to remove the surface from
 * @param surface     The surface to be removed from the group
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfacegroupRemoveSurface(t_ilm_surfacegroup group, t_ilm_surface surface);

/* Set the visibility of a surfacegroup. If a surfacegroup is not visible the contained
 * surfaces will not be rendered.
 *
 * @param group		          Id of the surfacegroup to set the visibility of
 * @param newVisibility       ILM_TRUE sets surfacegroup visible, ILM_FALSE disables the visibility.
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfacegroupSetVisibility(t_ilm_surfacegroup group, t_ilm_bool newVisibility);

/* Set the opacity of a surfacegroup.
 *
 * @param group       	  Id of the surfacegroup to set the opacity of.
 * @param opacity         0.0 means the surfacegroup is fully transparent,
 *                        1.0 means the surfacegroup is fully opaque
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfacegroupSetOpacity(t_ilm_surfacegroup group, t_ilm_float opacity);

/* Sets render order of layers on a display
 *
 * @param display       Id of display to set the given order of layers.
 * @param layerId	    array of layer ids
 * @param number     	number of layerids in the given array
 * @todo This display parameter is currently not implemented/not used.
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_displaySetRenderOrder(t_ilm_display display,t_ilm_layer *layerId,const t_ilm_uint number);

/* Take a screenshot from the current displayed layer scene.
 * The screenshot is saved as bmp file with the corresponding filename.
 *
 * @param filename     Location where the screenshot should be stored
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_takeScreenshot(t_ilm_uint screen, t_ilm_const_string filename);

/* Take a screenshot of a certain layer
 * The screenshot is saved as bmp file with the corresponding filename.
 *
 * @param filename     Location where the screenshot should be stored
 * @param layerid     	Identifier of the layer to take the screenshot of
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_takeLayerScreenshot(t_ilm_const_string filename, t_ilm_layer layerid);

/* Take a screenshot of a certain surface
 * The screenshot is saved as bmp file with the corresponding filename.
 *
 * @param filename     Location where the screenshot should be stored
 * @param surfaceid    Identifier of the surface to take the screenshot of
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_takeSurfaceScreenshot(t_ilm_const_string filename, t_ilm_surface surfaceid);



/* Commit all changes and executed commands since last commit.
 *
 * @return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_commitChanges();

#ifdef __cplusplus
} //
#endif // __cplusplus

#endif /* _ILM_CLIENT_H_ */

