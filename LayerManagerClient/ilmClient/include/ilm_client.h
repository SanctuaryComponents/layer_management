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
#ifndef _ILM_CLIENT_H_
#define _ILM_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "ilm_types.h"
/**
 * \defgroup LifeCycleManagement Life Cycle Management
 * \defgroup Configuration Configuration Functions
 * \defgroup Screen Screen Functions
 * \defgroup LayerGroup Layer Group Functions
 * \defgroup Layer Layer Functions
 * \defgroup SurfaceGroup Surface Group Functions
 * \defgroup Surface Surface Functions
 */

/**
 * \brief Initializes the IVI LayerManagement Client.
 * \ingroup LifeCycleManagement
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if a connection can not be established to the services.
 */
ilmErrorTypes ilm_init();

/**
 * \brief Get the screen resolution of a specific screen from the Layermanagement
 * \ingroup Screen 
 * \param[in] screenID Screen Indentifier as a Number from 0 .. MaxNumber of Screens
 * \param[out] pWidth pointer where width of screen should be stored
 * \param[out] pHeight pointer where height of screen should be stored
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not get the resolution.
 */
ilmErrorTypes ilm_getScreenResolution(t_ilm_uint screenID, t_ilm_uint* pWidth, t_ilm_uint* pHeight);

/**
 * \brief Get the surface properties from the Layermanagement
 * \ingroup Surface
 * \param[in] surfaceID surface Indentifier as a Number from 0 .. MaxNumber of Surfaces
 * \param[out] pSurfaceProperties pointer where the surface properties should be stored
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not get the resolution.
 */
ilmErrorTypes ilm_getPropertiesOfSurface(t_ilm_uint surfaceID, struct ilmSurfaceProperties* pSurfaceProperties);

/**
 * \brief  Get the layer properties from the Layermanagement
 * \ingroup Layer
 * \param[in] layerID layer Indentifier as a Number from 0 .. MaxNumber of Layer
 * \param[out] pLayerProperties pointer where the layer properties should be stored
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not get the resolution.
 */
ilmErrorTypes ilm_getPropertiesOfLayer(t_ilm_uint layerID, struct ilmLayerProperties* pLayerProperties);

/**
 * \brief  Get the number of hardware layers of a screen
 * \ingroup Layer 
 * \param[in] screenID id of the screen, where the number of Hardware Layers should be returned
 * \param[out] pNumberOfHardwareLayers pointer where the number of hardware layers should be stored
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not get the resolution.
 */
ilmErrorTypes ilm_getNumberOfHardwareLayers(t_ilm_uint screenID, t_ilm_uint* pNumberOfHardwareLayers);

/**
 * \brief Get the screen Ids
 * \ingroup Screen 
 * \param[out] pNumberOfIDs pointer where the number of Screen Ids should be returned
 * \param[out] ppIDs pointer to array where the IDs should be stored
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not get the resolution.
 */
ilmErrorTypes ilm_getScreenIDs(t_ilm_uint* pNumberOfIDs, t_ilm_uint** ppIDs);

/**
 * \brief Destroys the IVI LayerManagement Client.
 * \ingroup LifeCycleManagement 
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not be closed or was not initialized.
 */
ilmErrorTypes ilm_destroy();

/**
 * \brief Get all LayerIds which are currently registered and managed by the services
 * \ingroup Layer 
 * \param[out] pLength Pointer where length of ids array should be stored
 * \param[out] ppArray Array where the ids should be stored,
 *                     the array will be allocated inside
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_getLayerIDs(t_ilm_int* pLength,t_ilm_layer** ppArray);

/**
 * \brief Get all LayerIds of the given screen
 * \ingroup Screen 
 * \param[in] screenID The id of the screen to get the layer IDs of
 * \param[out] pLength Pointer where length of ids array should be stored
 * \param[out] ppArray Array where the ids should be stored,
 *                     the array will be allocated inside
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_getLayerIDsOnScreen(t_ilm_uint screenID, t_ilm_int* pLength,t_ilm_layer** ppArray);

/**
 * \brief Get all SurfaceIDs which are currently registered and managed by the services
 * \ingroup Surface 
 * \param[out] pLength Pointer where length of ids array should be stored
 * \param[out] ppArray Array where the ids should be stored,
 *                     the array will be allocated inside
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_getSurfaceIDs(t_ilm_int* pLength,t_ilm_surface** ppArray);

/**
 * \brief Get all LayerGroupIds which are currently registered and managed by the services
 * \ingroup LayerGroup
 * \param[out] pLength Pointer where array length of ids should be stored
 * \param[out] ppArray Array where the id should be stored,
 *                     the array will be allocated inside
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_getLayerGroupIDs(t_ilm_int* pLength,t_ilm_layergroup** ppArray);

/**
 * \brief Get all SurfaceGroupIds which are currently registered and managed by the services
 * \ingroup SurfaceGroup 
 * \param[out] pLength Pointer where array length of ids should be stored
 * \param[out] ppArray Array where the id should be stored,
 *                     the array will be allocated inside
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_getSurfaceGroupIDs(t_ilm_int* pLength,t_ilm_surfacegroup** ppArray);

/**
 * \brief Get all SurfaceIds which are currently registered to a given layer and are managed by the services
 * \ingroup Layer 
 * \param[in] layer Id of the Layer whose surfaces are to be returned
 * \param[out] pLength Pointer where the array length of ids should be stored
 * \param[out] ppArray Array where the surface id should be stored,
 *                     the array will be allocated inside
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_getSurfaceIDsOnLayer(t_ilm_layer layer,t_ilm_int* pLength,t_ilm_surface** ppArray);

/**
 * \brief Create a layer which should be managed by the service
 * \deprecated Will be removed in later version please use ilm_layerCreateWithDimension
 * \ingroup Layer 
 * \param[out] pLayerId pointer where the id should be/is stored. It is possible
 *                      to set a id from outside, 0 will create a new id.
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerCreate(t_ilm_layer* pLayerId);

/**
 * \brief Create a layer which should be managed by the service
 * \ingroup Layer
 * \param[out] pLayerId pointer where the id should be/is stored. It is possible
 *                      to set a id from outside, 0 will create a new id.
  *\param[in] width     horizontal dimension of the layer
 *  
 * \param[in] height    vertical dimension of the layer
 *                      
* 
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerCreateWithDimension(t_ilm_layer* pLayerId, t_ilm_uint width, t_ilm_uint height);

/**
 * \brief Removes a layer which is currently managed by the service
 * \ingroup Layer
 * \param[in] layerId Layer to be removed
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerRemove(t_ilm_layer layerId);

/**
 * \brief Add a surface to a layer which is currently managed by the service
 * \ingroup Layer
 * \param[in] layerId Id of layer which should host the surface.
 * \param[in] surfaceId Id of surface which should be added to the layer.
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerAddSurface(t_ilm_layer layerId, t_ilm_surface surfaceId);

/**
 * \brief Removes a surface from a layer which is currently managed by the service
 * \ingroup Layer
 * \param[in] layerId Id of the layer which contains the surface.
 * \param[in] surfaceId Id of the surface which should be removed from the layer.
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerRemoveSurface(t_ilm_layer layerId, t_ilm_surface surfaceId);

/**
 * \brief Get the current type of the layer.
 * \ingroup Layer
 * \param[in] layerId Id of the layer.
 * \param[out] pLayerType pointer to the layerType where the result should be stored.
 * \note ilmLayerType for information on supported types
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetType(t_ilm_layer layerId, ilmLayerType* pLayerType);

/**
 * \brief Set the visibility of a layer. If a layer is not visible, the layer and its
 * surfaces will not be rendered.
 * \ingroup Layer
 * \param[in] layerId Id of the layer.
 * \param[in] newVisibility ILM_TRUE sets layer visible, ILM_FALSE disables the visibility.
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetVisibility(t_ilm_layer layerId, t_ilm_bool newVisibility);

/**
 * \brief Get the visibility of a layer. If a layer is not visible, the layer and its
 * surfaces will not be rendered.
 * \ingroup Layer
 * \param[in] layerId Id of layer.
 * \param[out] pVisibility pointer where the visibility of the layer should be stored
 *                         ILM_TRUE if the Layer is visible,
 *                         ILM_FALSE if the visibility is disabled.
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetVisibility(t_ilm_layer layerId,t_ilm_bool *pVisibility);

/**
 * \brief Set the opacity of a layer.
 * \ingroup Layer
 * \param[in] layerId Id of the layer.
 * \param[in] opacity 0.0 means the layer is fully transparent,
 *                    1.0 means the layer is fully opaque
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetOpacity(t_ilm_layer layerId, t_ilm_float opacity);

/**
 * \brief Get the opacity of a layer.
 * \ingroup Layer
 * \param[in] layerId Id of the layer to obtain the opacity of.
 * \param[out] pOpacity pointer where the layer opacity should be stored.
 *                      0.0 means the layer is fully transparent,
 *                      1.0 means the layer is fully opaque
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetOpacity(t_ilm_layer layerId, t_ilm_float *pOpacity);

/**
 * \brief Set the area of a layer which should be used for the rendering. Only this part will be visible.
 * \ingroup Layer
 * \param[in] layerId Id of the layer.
 * \param[in] x horizontal start position of the used area
 * \param[in] y vertical start position of the used area
 * \param[in] width width of the area
 * \param[in] height height of the area
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetSourceRectangle(t_ilm_layer layerId, t_ilm_uint x, t_ilm_uint y, t_ilm_uint width, t_ilm_uint height);

/**
 * \brief Set the destination area on the display for a layer. The layer will be scaled and positioned to this rectangle for rendering
 * \ingroup Layer
 * \param[in] layerId Id of the layer.
 * \param[in] x horizontal start position of the used area
 * \param[in] y vertical start position of the used area
 * \param[in] width width of the area
 * \param[in] height height of the area
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetDestinationRectangle(t_ilm_layer layerId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height);

/**
 * \brief Get the horizontal and vertical dimension of the layer.
 * \ingroup Layer
 * \param[in] layerId Id of layer.
 * \param[out] pDimension pointer to an array where the dimension should be stored.
 *                       dimension[0]=width, dimension[1]=height
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetDimension(t_ilm_layer layerId, t_ilm_uint *pDimension);

/**
 * \brief Set the horizontal and vertical dimension of the layer.
 * \ingroup Layer
 * \param[in] layerId Id of layer.
 * \param[in] pDimension pointer to an array where the dimension is stored.
 *                       dimension[0]=width, dimension[1]=height
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetDimension(t_ilm_layer layerId, t_ilm_uint *pDimension);

/**
 * \brief Get the horizontal and vertical position of the layer.
 * \ingroup Layer
 * \param[in] layerId Id of layer.
 * \param[out] pPosition pointer to an array where the position should be stored.
 *                       dimension[0]=width, dimension[1]=height
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetPosition(t_ilm_layer layerId, t_ilm_uint *pPosition);

/**
 * \brief Sets the horizontal and vertical position of the layer.
 * \ingroup Layer
 * \param[in] layerId Id of layer.
 * \param[in] Pposition pointer to an array where the position is stored.
 *                      dimension[0]=x, dimension[1]=y
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetPosition(t_ilm_layer layerId, t_ilm_uint *pPosition);

/**
 * \brief Sets the orientation of a layer.
 * \ingroup Layer
 * \param[in] layerId Id of layer.
 * \param[in] orientation Orientation of the layer.
 * \note ilmOrientation for more information on orientation values
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetOrientation(t_ilm_layer layerId, ilmOrientation orientation);

/**
 * \brief Gets the orientation of a layer.
 * \ingroup Layer
 * \param[in] layerId Id of layer.
 * \param[out] pOrientation Address where orientation of the layer should be stored.
 * \note ilmOrientation for more information on orientation values
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetOrientation(t_ilm_layer layerId, ilmOrientation *pOrientation);

/**
 * \brief Sets the color value which defines the transparency value.
 * \ingroup Layer
 * \param[in] layerId Id of layer.
 * \param[in] pColor array of the color value which is defined in red,green, blue
 * \todo This method is currently not implemented.
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetChromaKey(t_ilm_layer layerId, t_ilm_int* pColor);

/**
 * \brief Sets render order of surfaces within one layer
 * \ingroup Layer
 * \param[in] layerId Id of layer.
 * \param[in] pSurfaceId array of surface ids
 * \param[in] number Number of elements in the given array of ids
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerSetRenderOrder(t_ilm_layer layerId, t_ilm_layer *pSurfaceId, t_ilm_int number);

/**
 * \brief Get the capabilities of a layer
 * \ingroup Layer
 * \param[in] layerId Id of the layer to obtain the capabilities of
 * \param[out] pCapabilities The address where the capabilites are returned
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerGetCapabilities(t_ilm_layer layerId, t_ilm_layercapabilities *pCapabilities);

/**
 * \brief Get the possible capabilities of a layertype
 * \ingroup Layer
 * \param[in] layerType The layertype to obtain the capabilities of
 * \param[out] pCapabilities The address where the capabilites are returned
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layerTypeGetCapabilities(ilmLayerType layerType, t_ilm_layercapabilities *pCapabilities);

/**
 * \brief Create a layergroup
 * \ingroup LayerGroup
 * \param[out] pLayergroup The id of the created layergroup is returned in this parameter
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layergroupCreate(t_ilm_layergroup *pLayergroup);

/**
 * \brief Remove a layergroup
 * \ingroup LayerGroup
 * \param[in] layergroup The layergroup to be removed
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layergroupRemove(t_ilm_layergroup layergroup);

/**
 * \brief Add a layer to a layergroup
 * \ingroup LayerGroup
 * \param[in] group The layergroup to add the layer
 * \param[in] layer The layer to add to the group
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layergroupAddLayer(t_ilm_layergroup group, t_ilm_layer layer);

/**
 * \brief Remove a layer from a layergroup
 * \ingroup LayerGroup
 * \param[in] layergroup The layergroup to remove the layer from
 * \param[in] layer The layer to be removed from the group
 *
 *
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layergroupRemoveLayer(t_ilm_layergroup layergroup, t_ilm_layer layer);

/**
 * \brief Set the visibility of a layergroup. If a layergroup is not visible, the layers and their
 * surfaces will not be rendered.
 * \ingroup LayerGroup
 * \param[in] layergroup Id of the layergroup to set the visibility of
 * \param[in] newVisibility ILM_TRUE sets layergroup visible, ILM_FALSE disables the visibility.
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layergroupSetVisibility(t_ilm_layergroup layergroup, t_ilm_bool newVisibility);

/**
 * \brief Set the opacity of a layergroup.
 * \ingroup LayerGroup
 * \param[in] group Id of the layergroup to set the opacity of.
 * \param[in] opacity 0.0 means the layergroup is fully transparent,
 *                    1.0 means the layergroup is fully opaque
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_layergroupSetOpacity(t_ilm_layergroup group, t_ilm_float opacity);

/**
 * \brief Create a surface
 * \ingroup Surface
 * \param[in] nativehandle The native windowsystem's handle for the surface
 * \param[in] width The original width of the surface
 * \param[in] height The original height of the surface
 * \param[in] pixelFormat The pixelformat to be used for the surface
 * \param[in/out] pSurfaceId
 *                The value pSurfaceId points to is used as ID for new surface;
 *                The ID of the newly created surface is returned in this parameter
 *
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceCreate(t_ilm_nativehandle nativehandle, t_ilm_int width, t_ilm_int height, ilmPixelFormat pixelFormat, t_ilm_surface *pSurfaceId);

/**
 * \brief Create a placeholder surface, which has no render buffer associated
 * \ingroup Surface
 * \param[in/out] pSurfaceId
 *                The value pSurfaceId points to is used as ID for new surface;
 *                The ID of the newly created surface is returned in this parameter
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceCreatePlaceholder(t_ilm_surface *pSurfaceId);

/**
 * \brief Associate the render buffer of an application with a placeholder surface
 * \ingroup Surface
 * \param[in] nativehandle The native windowsystem's handle for the surface
 * \param[in] width The original width of the surface
 * \param[in] height The original height of the surface
 * \param[in] pixelFormat The pixelformat to be used for the surface
 * \param[in] surfaceId The ID of the surface
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceAssociateRenderBuffer(t_ilm_nativehandle nativehandle, t_ilm_int width, t_ilm_int height, ilmPixelFormat pixelFormat, t_ilm_surface surfaceId);

/**
 * \brief Remove a surface
 * \ingroup Surface 
 * \param[in] surfaceId The id of the surface to be removed
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceRemove(const t_ilm_surface surfaceId);

/**
 * \brief Set the visibility of a surface. If a surface is not visible it will not be rendered.
 * \ingroup Surface
 * \param[in] surfaceId Id of the surface to set the visibility of
 * \param[in] newVisibility ILM_TRUE sets surface visible, ILM_FALSE disables the visibility.
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetVisibility(t_ilm_surface surfaceId, t_ilm_bool newVisibility);

/**
 * \brief Get the visibility of a surface. If a surface is not visible, the surface
 * will not be rendered.
 * \ingroup Surface
 * \param[in] surfaceId Id of the surface to get the visibility of.
 * \param[out] pVisibility pointer where the visibility of a surface should be stored
 *                         ILM_TRUE if the surface is visible,
 *                         ILM_FALSE if the visibility is disabled.
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceGetVisibility(t_ilm_surface surfaceId, t_ilm_bool *pVisibility);

/**
 * \brief Set the opacity of a surface.
 * \ingroup Surface
 * \param surfaceId Id of the surface to set the opacity of.
 * \param opacity 0.0 means the surface is fully transparent,
 *                1.0 means the surface is fully opaque
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetOpacity(const t_ilm_surface surfaceId, t_ilm_float opacity);

/**
 * \brief Get the opacity of a surface.
 * \ingroup Surface
 * \param[in] surfaceId Id of the surface to get the opacity of.
 * \param[out] pOpacity pointer where the surface opacity should be stored.
 *                      0.0 means the surface is fully transparent,
 *                      1.0 means the surface is fully opaque
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceGetOpacity(const t_ilm_surface surfaceId, t_ilm_float *pOpacity);

/**
 * \brief Set the area of a surface which should be used for the rendering.
 * \ingroup Surface
 * \param[in] surfaceId Id of surface.
 * \param[in] x horizontal start position of the used area
 * \param[in] y vertical start position of the used area
 * \param[in] width width of the area
 * \param[in] height height of the area
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetSourceRectangle(t_ilm_surface surfaceId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height);

/**
 * \brief Set the destination area of a surface within a layer for rendering. The surface will be scaled to this rectangle for rendering.
 * \ingroup Surface
 * \param[in] surfaceId Id of surface.
 * \param[in] x horizontal start position of the used area
 * \param[in] y vertical start position of the used area
 * \param[in] width width of the area
 * \param[in] height height of the area
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetDestinationRectangle(t_ilm_surface surfaceId, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height);

/**
 * \brief Get the horizontal and vertical dimension of the surface.
 * \ingroup Surface
 * \param[in] surfaceId Id of surface.
 * \param[out] pDimension pointer to an array where the dimension should be stored.
 *                        dimension[0]=width, dimension[1]=height
 *
 *
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceGetDimension(t_ilm_surface surfaceId, t_ilm_uint *pDimension);

/**
 * \brief Set the horizontal and vertical dimension of the surface.
 * \ingroup Surface
 * \param[in] surfaceId Id of surface.
 * \param[in] pDimension pointer to an array where the dimension is stored.
 *                       dimension[0]=width, dimension[1]=height
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetDimension(t_ilm_surface surfaceId, t_ilm_uint *pDimension);

/**
 * \brief Get the horizontal and vertical position of the surface.
 * \ingroup Surface
 * \param[in] surfaceId Id of surface.
 * \param[out] pPosition pointer to an array where the position should be stored.
 *                       position[0]=x, position[1]=y
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceGetPosition(t_ilm_surface surfaceId, t_ilm_uint *pPosition);

/**
 * \brief Sets the horizontal and vertical position of the surface.
 * \ingroup Surface
 * \param[in] surfaceId Id of surface.
 * \param[in] pPosition pointer to an array where the position is stored.
 *                      position[0]=x, position[1]=y
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetPosition(t_ilm_surface surfaceId, t_ilm_uint *pPosition);

/**
 * \brief Sets the orientation of a surface.
 * \ingroup Surface
 * \param[in] surfaceId Id of surface.
 * \param[in] orientation Orientation of the surface.
 * \note ilmOrientation for information about orientation values
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetOrientation(t_ilm_surface surfaceId, ilmOrientation orientation);

/**
 * \brief Gets the orientation of a surface.
 * \ingroup Surface
 * \param[in]  surfaceId Id of surface.
 * \param[out] pOrientation Address where orientation of the surface should be stored.
 * \note ilmOrientation for information about orientation values
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceGetOrientation(t_ilm_surface surfaceId, ilmOrientation *pOrientation);

/**
 * \brief Gets the pixelformat of a surface.
 * \ingroup Surface
 * \param[in] surfaceId Id of surface.
 * \param[out] pPixelformat Pointer where the pixelformat of the surface should be stored
 * \note ilmPixelFormat for information about pixel format values
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceGetPixelformat(t_ilm_layer surfaceId, ilmPixelFormat *pPixelformat);

/**
 * \brief Sets the color value which defines the transparency value of a surface.
 * \ingroup Surface
 * \param[in] surfaceId Id of the surface to set the chromakey of.
 * \param[in] pColor array of the color value which is defined in red, green, blue
 * \todo This method is currently not implemented.
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceSetChromaKey(t_ilm_surface surfaceId, t_ilm_int* pColor);

/**
 * \brief Inform that a certain rectangle of a surface has been invalidated and must consequently be redrawn
 * \ingroup Surface
 * \param[in] surfaceId Id of the surface
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfaceInvalidateRectangle(t_ilm_surface surfaceId);

/**
 * \brief Create a surfacegroup
 * \ingroup SurfaceGroup
 * \param[in] pSurfacegroup The created surfacegroup is returned in this parameter
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfacegroupCreate(t_ilm_surfacegroup *pSurfacegroup);

/**
*  \brief Remove a surfacegroup
 * \ingroup SurfaceGroup
 * \param[in] surfacegroup The surfacegroup to be removed
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfacegroupRemove(t_ilm_surfacegroup surfacegroup);

/**
 * \brief Add a surface to a surfacegroup
 * \ingroup SurfaceGroup
 * \param[in] surfacegroup The surfacegroup to add the surface
 * \param[in] surface The surface to add to the group
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfacegroupAddSurface(t_ilm_surfacegroup surfacegroup, t_ilm_surface surface);

/**
 * \brief Remove a surface from a surfacegroup
 * \ingroup SurfaceGroup
 * \param[in] surfacegroup The surfacegroup to remove the surface from
 * \param[in] surface The surface to be removed from the group
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfacegroupRemoveSurface(t_ilm_surfacegroup surfacegroup, t_ilm_surface surface);

/**
 * \brief Set the visibility of a surfacegroup. If a surfacegroup is not visible the contained
 * surfaces will not be rendered.
 * \ingroup SurfaceGroup
 * \param[in] surfacegroup Id of the surfacegroup to set the visibility of
 * \param[in] newVisibility ILM_TRUE sets surfacegroup visible, ILM_FALSE disables the visibility.
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfacegroupSetVisibility(t_ilm_surfacegroup surfacegroup, t_ilm_bool newVisibility);

/**
 * \brief Set the opacity of a surfacegroup.
 * \ingroup SurfaceGroup
 * \param[in] surfacegroup Id of the surfacegroup to set the opacity of.
 * \param[in] opacity 0.0 means the surfacegroup is fully transparent,
 *                    1.0 means the surfacegroup is fully opaque
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_surfacegroupSetOpacity(t_ilm_surfacegroup surfacegroup, t_ilm_float opacity);

/**
 * \brief Sets render order of layers on a display
 * \ingroup SurfaceGroup
 * \param[in] display Id of display to set the given order of layers.
 * \param[in] pLayerId array of layer ids
 * \param[in] number number of layerids in the given array
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_displaySetRenderOrder(t_ilm_display display, t_ilm_layer *pLayerId, const t_ilm_uint number);

/**
 * \brief Take a screenshot from the current displayed layer scene.
 * The screenshot is saved as bmp file with the corresponding filename.
 * \ingroup Screen
 * \param[in] screen Id of screen where screenshot should be taken
 * \param[in] filename Location where the screenshot should be stored
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_takeScreenshot(t_ilm_uint screen, t_ilm_const_string filename);

/**
 * \brief Take a screenshot of a certain layer
 * The screenshot is saved as bmp file with the corresponding filename.
 * \ingroup Layer
 * \param[in] filename Location where the screenshot should be stored
 * \param[in] layerid Identifier of the layer to take the screenshot of
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_takeLayerScreenshot(t_ilm_const_string filename, t_ilm_layer layerid);

/**
 * \brief Take a screenshot of a certain surface
 * The screenshot is saved as bmp file with the corresponding filename.
 * \ingroup Surface
 * \param[in] filename Location where the screenshot should be stored
 * \param[in] surfaceid Identifier of the surface to take the screenshot of
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_takeSurfaceScreenshot(t_ilm_const_string filename, t_ilm_surface surfaceid);

/**
 * \brief Commit all changes and executed commands since last commit.
 * \ingroup LifeCycleManagement
 * \return ILM_TRUE if the method call was successful
 * \return ILM_FAILED if the client can not call the method on the service.
 */
ilmErrorTypes ilm_commitChanges();

#ifdef __cplusplus
} //
#endif // __cplusplus

#endif /* _ILM_CLIENT_H_ */

