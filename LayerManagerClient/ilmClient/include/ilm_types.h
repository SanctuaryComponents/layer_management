/***************************************************************************
*
* Copyright 2010 BMW Car IT GmbH
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
	ILM_FAILED= 0,
	ILM_SUCCESS = 1
} ilmErrorTypes;

typedef enum e_ilmPixelFormat
{
	ILM_PIXELFORMAT_R_8 = 0,
	ILM_PIXELFORMAT_RGB_888 = 1,
	ILM_PIXELFORMAT_RGBA_8888 =2,
	ILM_PIXELFORMAT_RGB_565 = 3,
	ILM_PIXELFORMAT_RGBA_5551 = 4,
	ILM_PIXELFORMAT_RGBA_4444 = 5,
	ILM_PIXEL_FORMAT_UNKNOWN = 6
} ilmPixelFormat;

typedef enum e_ilmLayerType
{
	ILM_LAYETYPE_HARDWARE = 0,
	ILM_LAYETYPE_SOFTWARE = 1
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

typedef t_ilm_int 	t_ilm_layer;
typedef t_ilm_int 	t_ilm_surface;

typedef t_ilm_int 	t_ilm_layergroup;
typedef t_ilm_int 	t_ilm_surfacegroup;
typedef t_ilm_int 	t_ilm_display;
typedef t_ilm_uint 	t_ilm_layercapabilities;
typedef t_ilm_ulong	t_ilm_nativehandle;
typedef t_ilm_char*     t_ilm_string;

#endif // _ILM_TYPES_H_
