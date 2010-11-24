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

#include "IlmMatrix.h"
#include <math.h>

#define ILM_PI 3.14159265

static const IlmMatrix m_Identity =
{
                {
                    1.0f,0.0f,0.0f,0.0f,
                    0.0f,1.0f,0.0f,0.0f,
                    0.0f,0.0f,1.0f,0.0f,
                    0.0f,0.0f,0.0f,1.0f
                }
};

void IlmMatrixIdentity(	IlmMatrix &mOut		)
{
	mOut.f[0] = 1.0f;	mOut.f[4] = 0.0f;	mOut.f[8] = 0.0f;	mOut.f[12] = 0.0f;
	mOut.f[1] = 0.0f;	mOut.f[5] = 1.0f;	mOut.f[9] = 0.0f;	mOut.f[13] = 0.0f;
	mOut.f[2] = 0.0f;	mOut.f[6] = 0.0f;	mOut.f[10] = 1.0f;	mOut.f[14] = 0.0f;
	mOut.f[3] = 0.0f;	mOut.f[7] = 0.0f;	mOut.f[11] = 0.0f;	mOut.f[15] = 1.0f;
}

void IlmMatrixMultiply(	IlmMatrix &mOut,
		const IlmMatrix &mA,
		const IlmMatrix &mB )
{
	IlmMatrix mRet;

	mRet.f[0] = mA.f[0] * mB.f[0] + mA.f[1]*mB.f[4] + mA.f[2]*mB.f[8]+mA.f[3]*mB.f[12];
	mRet.f[1] = mA.f[0] * mB.f[1] + mA.f[1]*mB.f[5] + mA.f[2]*mB.f[9]+mA.f[3]*mB.f[13];
	mRet.f[2] = mA.f[0] * mB.f[2] + mA.f[1]*mB.f[6] + mA.f[2]*mB.f[10]+mA.f[3]*mB.f[14];
	mRet.f[3] = mA.f[0] * mB.f[3] + mA.f[1]*mB.f[7] + mA.f[2]*mB.f[11]+mA.f[3]*mB.f[15];

	mRet.f[4] = mA.f[4] * mB.f[0] + mA.f[5]*mB.f[4] + mA.f[6]*mB.f[8]+mA.f[7]*mB.f[12];
	mRet.f[5] = mA.f[4] * mB.f[1] + mA.f[5]*mB.f[5] + mA.f[6]*mB.f[9]+mA.f[7]*mB.f[13];
	mRet.f[6] = mA.f[4] * mB.f[2] + mA.f[5]*mB.f[6] + mA.f[6]*mB.f[10]+mA.f[7]*mB.f[14];
	mRet.f[7] = mA.f[4] * mB.f[3] + mA.f[5]*mB.f[7] + mA.f[6]*mB.f[11]+mA.f[7]*mB.f[15];

	mRet.f[8] = mA.f[8] * mB.f[0] + mA.f[9]*mB.f[4] + mA.f[10]*mB.f[8]+mA.f[11]*mB.f[12];
	mRet.f[9] = mA.f[8] * mB.f[1] + mA.f[9]*mB.f[5] + mA.f[10]*mB.f[9]+mA.f[11]*mB.f[13];
	mRet.f[10] = mA.f[8] * mB.f[2] + mA.f[9]*mB.f[6] + mA.f[10]*mB.f[10]+mA.f[11]*mB.f[14];
	mRet.f[11] = mA.f[8] * mB.f[3] + mA.f[9]*mB.f[7] + mA.f[10]*mB.f[11]+mA.f[11]*mB.f[15];

	mRet.f[12] = mA.f[12] * mB.f[0] + mA.f[13]*mB.f[4] + mA.f[14]*mB.f[8]+mA.f[15]*mB.f[12];
	mRet.f[13] = mA.f[12] * mB.f[1] + mA.f[13]*mB.f[5] + mA.f[14]*mB.f[9]+mA.f[15]*mB.f[13];
	mRet.f[14] = mA.f[12] * mB.f[2] + mA.f[13]*mB.f[6] + mA.f[14]*mB.f[10]+mA.f[15]*mB.f[14];
	mRet.f[15] = mA.f[12] * mB.f[3] + mA.f[13]*mB.f[7] + mA.f[14]*mB.f[11]+mA.f[15]*mB.f[15];

	mOut = mRet;
}

void IlmMatrixTranslation( IlmMatrix &mOut,
		const float X,
		const float Y,
		const float Z
)
{
	mOut.f[ 0]=1.0f;	mOut.f[ 4]=0.0f;	mOut.f[ 8]=0.0f;	mOut.f[12]=X;
	mOut.f[ 1]=0.0f;	mOut.f[ 5]=1.0f;	mOut.f[ 9]=0.0f;	mOut.f[13]=Y;
	mOut.f[ 2]=0.0f;	mOut.f[ 6]=0.0f;	mOut.f[10]=1.0f;	mOut.f[14]=Z;
	mOut.f[ 3]=0.0f;	mOut.f[ 7]=0.0f;	mOut.f[11]=0.0f;	mOut.f[15]=1.0f;

}

void IlmMatrixScaling( IlmMatrix &mOut,
		const float X,
		const float Y,
		const float Z
)
{
	mOut.f[ 0]=X;		mOut.f[ 4]=0.0f;	mOut.f[ 8]=0.0f;	mOut.f[12]=0.0f;
	mOut.f[ 1]=0.0f;	mOut.f[ 5]=Y;		mOut.f[ 9]=0.0f;	mOut.f[13]=0.0f;
	mOut.f[ 2]=0.0f;	mOut.f[ 6]=0.0f;	mOut.f[10]=Z;		mOut.f[14]=0.0f;
	mOut.f[ 3]=0.0f;	mOut.f[ 7]=0.0f;	mOut.f[11]=0.0f;	mOut.f[15]=1.0f;

}

void IlmMatrixRotateX( IlmMatrix &mOut,
		const float angle )
{
	float		fCosine, fSine;

    /* Precompute cos and sin */
	fCosine	= (float)cos(angle*ILM_PI/180.0f);
    fSine	= (float)sin(angle*ILM_PI/180.0f);

	/* Create the trigonometric matrix corresponding to X Rotation */
	mOut.f[ 0]=1.0f;	mOut.f[ 4]=0.0f;	mOut.f[ 8]=0.0f;	mOut.f[12]=0.0f;
	mOut.f[ 1]=0.0f;	mOut.f[ 5]=fCosine;	mOut.f[ 9]=fSine;	mOut.f[13]=0.0f;
	mOut.f[ 2]=0.0f;	mOut.f[ 6]=-fSine;	mOut.f[10]=fCosine;	mOut.f[14]=0.0f;
	mOut.f[ 3]=0.0f;	mOut.f[ 7]=0.0f;	mOut.f[11]=0.0f;	mOut.f[15]=1.0f;

}

void IlmMatrixRotateY( IlmMatrix &mOut,
		const float angle )
{
	float fCosine	= (float)cos(angle*ILM_PI/180.0f);
    float fSine	= (float)sin(angle*ILM_PI/180.0f);

    /* Create the trigonometric matrix corresponding to Y Rotation */
	mOut.f[ 0]=fCosine;		mOut.f[ 4]=0.0f;	mOut.f[ 8]=-fSine;		mOut.f[12]=0.0f;
	mOut.f[ 1]=0.0f;		mOut.f[ 5]=1.0f;	mOut.f[ 9]=0.0f;		mOut.f[13]=0.0f;
	mOut.f[ 2]=fSine;		mOut.f[ 6]=0.0f;	mOut.f[10]=fCosine;		mOut.f[14]=0.0f;
	mOut.f[ 3]=0.0f;		mOut.f[ 7]=0.0f;	mOut.f[11]=0.0f;		mOut.f[15]=1.0f;

}
void IlmMatrixRotateZ( IlmMatrix &mOut,
		const float angle )
{
	float fCosine	= (float)cos(angle*ILM_PI/180.0f);
    float fSine	= (float)sin(angle*ILM_PI/180.0f);

	/* Create the trigonometric matrix corresponding to Z Rotation */
	mOut.f[ 0]=fCosine;		mOut.f[ 4]=fSine;	mOut.f[ 8]=0.0f;	mOut.f[12]=0.0f;
	mOut.f[ 1]=-fSine;		mOut.f[ 5]=fCosine;	mOut.f[ 9]=0.0f;	mOut.f[13]=0.0f;
	mOut.f[ 2]=0.0f;		mOut.f[ 6]=0.0f;	mOut.f[10]=1.0f;	mOut.f[14]=0.0f;
	mOut.f[ 3]=0.0f;		mOut.f[ 7]=0.0f;	mOut.f[11]=0.0f;	mOut.f[15]=1.0f;

}
