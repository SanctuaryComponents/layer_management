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
#include "Bitmap.h"
#include "Log.h"
#include <stdint.h>

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint32_t LONG;

typedef struct tagBMPFileHeader {
	DWORD bfSize; // bytes of entire file
	WORD bfReserved1; // nothing
	WORD bfReserved2; // nothing
	DWORD bfOffBits; // address of bitmap data in file
} BMPFileHeader;


typedef struct tagBMPInfoHeader {
	DWORD biSize; // Size of this BMPInfoHeader
	int32_t biWidth;
	int32_t biHeight;
	WORD biPlanes; // 1 plane, so this equals 1
	WORD biBitCount; // 24 bits/pixel
	DWORD biCompression; // 0 = BMP
	DWORD biSizeImage; // size of actual bitmap image
	int32_t biXPelsPerMeter; // Horizontal resolution
	int32_t biYPelsPerMeter; // Vertical resolution
	DWORD biClrUsed; // 0
	DWORD biClrImportant; // 0 important colors (very old)
} BMPInfoHeader;

void writeBitmap(std::string FileName, char* imagedataRGB, int width, int height)
{
	LOG_DEBUG("Bitmap","writing Bitmap to file:" <<FileName);
	/*
	The reason these 2 are seperate is because sometimes (never)
	you might want to use a different BMPInfoHeader
	*/

	BMPFileHeader bitmapFileHeader;
	BMPInfoHeader bitmapInfoHeader;
	int imagebytes=width*height*3;
	//printf("File Header Size(12) = %lu\n",sizeof(BMPFileHeader));
	//printf("Info Header Size(40) = %lu\n",sizeof(BMPInfoHeader));
	char magic[2];
	magic[0]='B';
	magic[1]='M';
	bitmapFileHeader.bfSize = 2+sizeof(BMPFileHeader)+sizeof(BMPInfoHeader)+imagebytes;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits = 2+sizeof(BMPFileHeader)+sizeof(BMPInfoHeader);
	bitmapInfoHeader.biSize = sizeof(BMPInfoHeader);
	bitmapInfoHeader.biWidth = width;
	bitmapInfoHeader.biHeight = height;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24; // 24 bits/pixel
	bitmapInfoHeader.biCompression = 0; // Zero is the defaut Bitmap
	bitmapInfoHeader.biSizeImage = imagebytes;
	bitmapInfoHeader.biXPelsPerMeter = 2835; // 72 pixels/inch = 2834.64567 pixels per meter
	bitmapInfoHeader.biYPelsPerMeter = 2835;
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;

	// Now to write the file
	// Open the file in "write binary" mode
	FILE *f=fopen(FileName.c_str(),"wb");
	if (NULL != f){
		fwrite(magic, 2, 1, f);
		fwrite((void*)&bitmapFileHeader, sizeof(BMPFileHeader), 1, f);
		fwrite((void*)&bitmapInfoHeader, sizeof(BMPInfoHeader), 1, f);
		fwrite(imagedataRGB, bitmapInfoHeader.biSizeImage, 1, f);
		fclose(f);
	}else{
		LOG_DEBUG("Bitmap","File could not be opened for writing");
	}

}
