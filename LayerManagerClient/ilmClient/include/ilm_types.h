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
#ifndef _ILM_TYPES_H_
#define _ILM_TYPES_H_

#include "ilm_client_platform.h"

/**
 * \defgroup Defines
 * \defgroup Enumerations
 * \defgroup TypeDefinitions
 **/
/**
 * \brief Represent the logical true value
 * \ingroup Defines
 **/
#define ILM_TRUE     1u

/**
 * \brief Represent the logical false value
 * \ingroup Defines
 **/
#define ILM_FALSE     0u

/**
 * \brief Enumeration on possible error codes
 * \ingroup Enumerations
 **/
typedef enum e_ilmErrorTypes
{
    ILM_SUCCESS = 0,                  /*!< ErrorCode if the method call was successfull */
    ILM_FAILED= 1,                    /*!< ErrorCode if the method call has failed */
    ILM_ERROR_INVALID_ARGUMENTS = 2,  /*!< ErrorCode if the method was called with invalid arguments */
    ILM_ERROR_ON_CONNECTION = 3       /*!< ErrorCode if connection error has occured */
} ilmErrorTypes;

/**
 * \brief Enumeration for supported pixelformats
 * \ingroup Enumerations
 **/
typedef enum e_ilmPixelFormat
{
    ILM_PIXELFORMAT_R_8 = 0,           /*!< Pixelformat value, to describe a 8 bit luminance surface */
    ILM_PIXELFORMAT_RGB_888 = 1,       /*!< Pixelformat value, to describe a 24 bit rgb surface */
    ILM_PIXELFORMAT_RGBA_8888 =2,      /*!< Pixelformat value, to describe a 24 bit rgb surface with 8 bit alpha */
    ILM_PIXELFORMAT_RGB_565 = 3,       /*!< Pixelformat value, to describe a 16 bit rgb surface */
    ILM_PIXELFORMAT_RGBA_5551 = 4,     /*!< Pixelformat value, to desicribe a 16 bit rgb surface, with binary mask */
    ILM_PIXELFORMAT_RGBA_6661 = 5,     /*!< Pixelformat value, to desicribe a 18 bit rgb surface, with binars mask */
    ILM_PIXELFORMAT_RGBA_4444 = 6,     /*!< Pixelformat value, to desicribe a 12 bit rgb surface, with 4 bit alpha */
    ILM_PIXEL_FORMAT_UNKNOWN = 7       /*!< Pixelformat not known */
} ilmPixelFormat;

/**
 * \brief Enumeration for supported layertypes
 * \ingroup Enumerations
 **/
typedef enum e_ilmLayerType
{
    ILM_LAYERTYPE_UNKNOWN = 0,         /*!< LayerType not known */
    ILM_LAYERTYPE_HARDWARE = 1,        /*!< LayerType value, to describe a hardware layer */
    ILM_LAYERTYPE_SOFTWARE2D = 2,      /*!< LayerType value, to describe a redirected offscreen buffer layer */
    ILM_LAYERTYPE_SOFTWARE2_5D = 3     /*!< LayerType value, to describe a redirected offscreen buffer layer, which can be rotated in the 3d space */
} ilmLayerType;

/**
 * \brief Enumeration for supported graphical objects
 * \ingroup Enumerations
 **/
typedef enum e_ilmObjectType
{
    ILM_SURFACE = 0,                   /*!< Surface Object Type */
    ILM_LAYER = 1,                     /*!< Layer Object Type */
    ILM_SURFACEGROUP = 2,              /*!< SurfaceGroup Object Type */
    ILM_LAYERGROUP = 3                 /*!< LayerGroup Object Type */
} ilmObjectType;

/**
 * \brief Enumeration for supported orientations of booth, surface and layer
 * \ingroup Enumerations
 **/
typedef enum e_ilmOrientation
{
    ILM_ZERO = 0,                       /*!< Orientation value, to describe 0 degree of rotation regarding the z-axis*/
    ILM_NINETY = 1,                     /*!< Orientation value, to describe 90 degree of rotation regarding the z-axis*/
    ILM_ONEHUNDREDEIGHTY = 2,           /*!< Orientation value, to describe 180 degree of rotation regarding the z-axis*/
    ILM_TWOHUNDREDSEVENTY = 3           /*!< Orientation value, to describe 270 degree of rotation regarding the z-axis*/
} ilmOrientation;

/**
 * \brief Typedef for representing a layer
 * \ingroup TypeDefinitions
 **/
typedef t_ilm_uint     t_ilm_layer;

/**
 * \brief Typedef for representing a surface
 * \ingroup TypeDefinitions
 **/
typedef t_ilm_uint     t_ilm_surface;

/**
 * \brief Typedef for representing a layergroup
 * \ingroup TypeDefinitions
 **/
typedef t_ilm_uint     t_ilm_layergroup;

/**
 * \brief Typedef for representing a surfacegroup
 * \ingroup TypeDefinitions
 **/
typedef t_ilm_uint     t_ilm_surfacegroup;

/**
 * \brief Typedef for representing a display number
 * \ingroup TypeDefinitions
 **/
typedef t_ilm_uint     t_ilm_display;

/**
 * \brief Typedef for representing layer capabilities
 * \ingroup TypeDefinitions
 **/
typedef t_ilm_uint     t_ilm_layercapabilities;

/**
 * \brief Typedef for representing a native window handle
 * \ingroup TypeDefinitions
 **/
typedef t_ilm_ulong    t_ilm_nativehandle;

/**
 * \brief Typedef for representing a ascii string
 * \ingroup TypeDefinitions
 **/
typedef t_ilm_char* t_ilm_string;

/**
 * \brief Typedef for representing a const ascii string
 * \ingroup TypeDefinitions
 **/
typedef t_ilm_const_char* t_ilm_const_string;

/**
 * \brief Typedef for representing a the surface properties structure
 * \ingroup TypeDefinitions
 **/
struct ilmSurfaceProperties
{
    t_ilm_float opacity;         /*!< opacity value of the surface */
    t_ilm_uint sourceX;        /*!< x source position value of the surface */
    t_ilm_uint sourceY;        /*!< y source position value of the surface */
    t_ilm_uint sourceWidth;        /*!< source width value of the surface */
    t_ilm_uint sourceHeight;    /*!< source height value of the surface */
    t_ilm_uint origSourceWidth;        /*!< original source width value of the surface */
    t_ilm_uint origSourceHeight;    /*!< original source height value of the surface */
    t_ilm_uint destX;        /*!< x destination position value of the surface */
    t_ilm_uint destY;        /*!< y desitination position value of the surface */
    t_ilm_uint destWidth;        /*!< destination width value of the surface */
    t_ilm_uint destHeight;        /*!< destination height value of the surface */
    ilmOrientation orientation;    /*!< orientation value of the surface */
    t_ilm_bool visibility;        /*!< visibility value of the surface */
    t_ilm_uint frameCounter;      /*!< already rendered frames of surface */
};

/**
 * \brief Typedef for representing a the layer properties structure
 * \ingroup TypeDefinitions
 **/
struct ilmLayerProperties
{
    t_ilm_float opacity;         /*!< opacity value of the layer */
    t_ilm_uint sourceX;        /*!< x source position value of the layer */
    t_ilm_uint sourceY;        /*!< y source position value of the layer */
    t_ilm_uint sourceWidth;        /*!< source width value of the layer */
    t_ilm_uint sourceHeight;    /*!< source height value of the layer */
    t_ilm_uint origSourceWidth;        /*!< original source width value of the layer */
    t_ilm_uint origSourceHeight;    /*!< original source height value of the layer */    
    t_ilm_uint destX;        /*!< x destination position value of the layer */
    t_ilm_uint destY;        /*!< y desitination position value of the layer */
    t_ilm_uint destWidth;        /*!< destination width value of the layer */
    t_ilm_uint destHeight;        /*!< destination height value of the layer */
    ilmOrientation orientation;    /*!< orientation value of the layer */
    t_ilm_bool visibility;        /*!< visibility value of the layer */
};

#endif // _ILM_TYPES_H_
