/***************************************************************************
*
* Copyright 2010 BMW Car IT GmbH
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*               http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
****************************************************************************/

#ifndef _ILM_MATRIX_H_
#define _ILM_MATRIX_H_
#define MAT00 	0
#define MAT01 	1
#define MAT02 	2
#define MAT03 	3
#define MAT10 	4
#define MAT11 	5
#define MAT12 	6
#define MAT13 	7
#define MAT20 	8
#define MAT21 	9
#define MAT22 	10
#define MAT23 	11
#define MAT30 	12
#define MAT31 	13
#define MAT32 	14
#define MAT33 	15



typedef struct
{
	float x;
	float y;
} IlmVector2f;

typedef struct
{
	float x;
	float y;
	float z;
} IlmVector3f;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
} IlmVector4f;


class IlmMatrix {
public:
	float * operator [] (const int row)
	{
		return &f[row<<2];
	}
	float f[16];
};

void IlmMatrixIdentity(	IlmMatrix &mOut		);

void IlmMatrixMultiply(	IlmMatrix &mOut,
		const IlmMatrix &mA,
		const IlmMatrix &mB );

void IlmMatrixTranslation( IlmMatrix &mOut,
		const float X,
		const float Y,
		const float Z
);

void IlmMatrixScaling( IlmMatrix &mOut,
		const float X,
		const float Y,
		const float Z
);

void IlmMatrixRotateX( IlmMatrix &mOut,
		const float angle
);

void IlmMatrixRotateY( IlmMatrix &mOut,
		const float angle
);

void IlmMatrixRotateZ( IlmMatrix &mOut,
		const float angle
);


#endif /* _ILMMATRIX_H*/
