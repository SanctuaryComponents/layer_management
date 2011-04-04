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
#ifndef _ILM_TYPES_H_
#define _ILM_TYPES_H_
#include "ilm_client_platform.h"
#define ILM_TRUE 	1
#define ILM_FALSE 	0

typedef enum e_ilmErrorTypes
{
	ILM_SUCCESS = 0,
	ILM_FAILED= 1,
	ILM_ERROR_INVALID_ARGUMENTS = 2,
	ILM_ERROR_ON_CONNECTION = 3
} ilmErrorTypes;

typedef enum e_ilmPixelFormat
{
	ILM_PIXELFORMAT_R_8 = 0,
	ILM_PIXELFORMAT_RGB_888 = 1,
	ILM_PIXELFORMAT_RGBA_8888 =2,
	ILM_PIXELFORMAT_RGB_565 = 3,
	ILM_PIXELFORMAT_RGBA_5551 = 4,
	ILM_PIXELFORMAT_RGBA_6661 = 5,
	ILM_PIXELFORMAT_RGBA_4444 = 6,
	ILM_PIXEL_FORMAT_UNKNOWN = 7
} ilmPixelFormat;

typedef enum e_ilmLayerType
{
	ILM_LAYERTYPE_UNKNOWN = 0,
	ILM_LAYERTYPE_HARDWARE = 1,
	ILM_LAYERTYPE_SOFTWARE2D = 2,
	ILM_LAYERTYPE_SOFTWARE2_5D = 3
} ilmLayerType;

typedef enum e_ilmObjectType
{
	ILM_SURFACE = 0,
	ILM_LAYER = 1,
	ILM_SURFACEGROUP = 2,
	ILM_LAYERGROUP = 3
} ilmObjectType;

typedef enum e_ilmOrientation
{
	ILM_ZERO = 0,
	ILM_NINETY = 1,
	ILM_ONEHUNDREDEIGHTY = 2,
	ILM_TWOHUNDREDSEVENTY = 3
} ilmOrientation;

typedef t_ilm_uint 	t_ilm_layer;
typedef t_ilm_uint 	t_ilm_surface;

typedef t_ilm_uint 	t_ilm_layergroup;
typedef t_ilm_uint 	t_ilm_surfacegroup;
typedef t_ilm_uint 	t_ilm_display;
typedef t_ilm_uint 	t_ilm_layercapabilities;
typedef t_ilm_ulong	t_ilm_nativehandle;
typedef t_ilm_char* t_ilm_string;
typedef t_ilm_const_char* t_ilm_const_string;

struct ilmSurfaceProperties
{
	t_ilm_float opacity;
	t_ilm_uint sourceX;
	t_ilm_uint sourceY;
	t_ilm_uint sourceWidth;
	t_ilm_uint sourceHeight;
	t_ilm_uint destX;
	t_ilm_uint destY;
	t_ilm_uint destWidth;
	t_ilm_uint destHeight;
	ilmOrientation orientation;
	t_ilm_bool visibility;
};

struct ilmLayerProperties
{
	t_ilm_float opacity;
	t_ilm_uint sourceX;
	t_ilm_uint sourceY;
	t_ilm_uint sourceWidth;
	t_ilm_uint sourceHeight;
	t_ilm_uint destX;
	t_ilm_uint destY;
	t_ilm_uint destWidth;
	t_ilm_uint destHeight;
	ilmOrientation orientation;
	t_ilm_bool visibility;
};

#endif // _ILM_TYPES_H_
