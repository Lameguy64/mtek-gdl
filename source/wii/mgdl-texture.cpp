// Texture handling class functions (see mgdl-wii.h or library docs for info)

#ifndef _GDL_WII_TEXTURE
#define _GDL_WII_TEXTURE

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <gccore.h>

#include "mgdl-wii/mgdl-config.h"
#include "mgdl-wii/mgdl-types.h"
#include "mgdl-wii/mgdl-globals.h"
#include "mgdl-wii/mgdl-globals-internal.h"
#include "mgdl-wii/mgdl-main.h"
#include "mgdl-wii/mgdl-texture.h"


// Internal macros for texture conversion (from Metaphrasis)
#define RGBA_TO_RGB565(x) ((x & 0xf8000000) >> 16) | ((x & 0x00fc0000) >> 13) | ((x & 0x0000f800) >> 11)
#define RGBA_TO_RGB555(x) ((x & 0xf8000000) >> 17) | ((x & 0x00f80000) >> 14) | ((x & 0x0000f800) >> 11) | 0x8000


// Unlike the other conversion macros, I actually wrote this myself because the macro for RGB4A3
// conversion was bugged in Metaphrasis (stupid lazy coders) but at least mine is very readable
#define RGBA_TO_RGB4A3(x)	(((x & 0xff) / 32) << 12) | \
							((((x >> 24) & 0xff) / 16) << 8) | \
							((((x >> 16) & 0xff) / 16) << 4) | \
							((((x >> 8) & 0xff) / 16))

#define RGBA_TO_RGB5A3(x)	(x & 0xff) < 0xe0 ? RGBA_TO_RGB4A3(x) : RGBA_TO_RGB555(x)


// Module internal functions

static void *_gdl_texture_ConvertI8toIA8(short xres, short yres, void *buff) {

	void *retPtr = malloc2(2*(xres*yres));

	if (retPtr == NULL)
		return(NULL);

	for(short py=0; py<yres; py++) {
		for(short px=0; px<xres; px++) {

			int soffs = (px+(xres*py));
			int doffs = 2*(px+(xres*py));

			((u_char*)retPtr)[doffs] = ((u_char*)buff)[soffs]; doffs++;
			((u_char*)retPtr)[doffs] = 0xff;

		}
	}

	return(retPtr);

}

static void *_gdl_texture_ConvertIA8toI8(short xres, short yres, void *buff) {

	void *retPtr = malloc2(xres*yres);

	if (retPtr == NULL)
		return(NULL);

	for(short py=0; py<yres; py++) {
		for(short px=0; px<xres; px++) {

			int soffs = 2*(px+(xres*py));
			int doffs = (px+(xres*py));

			((u_char*)retPtr)[doffs] = ((u_char*)buff)[soffs];

		}
	}

	return(retPtr);

}

static void *_gdl_texture_ConvertRGBtoI8(short xres, short yres, void *buff, bool hasAlpha) {

	void	*retPtr;
	int		soffs;

	retPtr = malloc2(xres*yres);

	if (retPtr == NULL)
		return(NULL);

	for(short py=0; py<yres; py++) {
		for(short px=0; px<xres; px++) {

			if (hasAlpha == false)
				soffs = 3*(px+(xres*py));
			else
				soffs = 4*(px+(xres*py));

			((u_char*)retPtr)[px+(xres*py)] = ((((u_char*)buff)[soffs] + ((u_char*)buff)[soffs+1] + ((u_char*)buff)[soffs+2])/3);

		}
	}

	return(retPtr);

}

static void *_gdl_texture_ConvertI8toRGBA(short xres, short yres, void *buff) {

	void *retPtr = malloc2(4*(xres*yres));

	if (retPtr == NULL)
		return(NULL);

	for(short py=0; py<yres; py++) {
		for(short px=0; px<xres; px++) {

			int soffs = (px+(xres*py));
			int doffs = 4*(px+(xres*py));

			((u_char*)retPtr)[doffs] = ((u_char*)buff)[soffs]; doffs++;
			((u_char*)retPtr)[doffs] = ((u_char*)buff)[soffs]; doffs++;
			((u_char*)retPtr)[doffs] = ((u_char*)buff)[soffs]; doffs++;
			((u_char*)retPtr)[doffs] = 0xff;

		}
	}

	return(retPtr);

}

static void *_gdl_texture_ConvertIA8toRGBA(short xres, short yres, void *buff) {

	void *retPtr = malloc2(4*(xres*yres));

	if (retPtr == NULL)
		return(NULL);

	for(short py=0; py<yres; py++) {
		for(short px=0; px<xres; px++) {

			int soffs = 2*(px+(xres*py));
			int doffs = 4*(px+(xres*py));

			((u_char*)retPtr)[doffs] = ((u_char*)buff)[soffs]; doffs++;
			((u_char*)retPtr)[doffs] = ((u_char*)buff)[soffs]; doffs++;
			((u_char*)retPtr)[doffs] = ((u_char*)buff)[soffs]; doffs++; soffs++;
			((u_char*)retPtr)[doffs] = ((u_char*)buff)[soffs];

		}
	}

	return(retPtr);

}

static void *_gdl_texture_ConvertRGBtoIA8(short xres, short yres, void *buff, bool hasAlpha) {

	void	*retPtr;
	int		soffs;

	retPtr = malloc2(2*(xres*yres));

	if (retPtr == NULL)
		return(NULL);

	for(short py=0; py<yres; py++) {
		for(short px=0; px<xres; px++) {

			if (hasAlpha == false)
				soffs = 3*(px+(xres*py));
			else
				soffs = 4*(px+(xres*py));

			((u_char*)retPtr)[2*(px+(xres*py))] = ((((u_char*)buff)[soffs] + ((u_char*)buff)[soffs+1] + ((u_char*)buff)[soffs+2])/3);

			if (hasAlpha == false)
				((u_char*)retPtr)[(2*(px+(xres*py)))+1] = 0xff;
			else
				((u_char*)retPtr)[(2*(px+(xres*py)))+1] = ((u_char*)buff)[soffs+3];

		}
	}


	return(retPtr);

}

static void *_gdl_texture_ConvertRGBtoRGBA(short xres, short yres, void *buff) {

	void *retPtr = malloc2(4*(xres*yres));

	if (retPtr == NULL)
		return(NULL);

	for(short py=0; py<yres; py++) {
		for(short px=0; px<xres; px++) {

			int soffs = 3*(px+(xres*py));
			int doffs = 4*(px+(xres*py));

			((u_char*)retPtr)[doffs] = ((u_char*)buff)[soffs]; doffs++; soffs++;
			((u_char*)retPtr)[doffs] = ((u_char*)buff)[soffs]; doffs++; soffs++;
			((u_char*)retPtr)[doffs] = ((u_char*)buff)[soffs]; doffs++;
			((u_char*)retPtr)[doffs] = 0xff;

		}
	}

	return(retPtr);

}

static void _gdl_texture_ConvertI4(short xres, short yres, short stride, void *inBuff, void *outBuff) {

	for(short py=0; py<yres; py++) {
		for(short px=0; px<xres; px++) {

			int doffs = (((px/2)%4)+(32*((px/2)/4)))+((stride*4)*(py/8))+(4*(py%8));
			int soffs = px+(xres*py);

			if ((px%2) == 0)
				((u_char*)outBuff)[doffs] = (((u_char*)outBuff)[doffs]&0x0f)|((((u_char*)inBuff)[soffs]/16)<<4);
			else
				((u_char*)outBuff)[doffs] = (((u_char*)inBuff)[soffs]/16)|(((u_char*)outBuff)[doffs]&0xf0);

		}
	}

}

static void _gdl_texture_ConvertI8(short xres, short yres, short stride, void *inBuff, void *outBuff) {

	for(short py=0; py<yres; py++) {
		for(short px=0; px<xres; px++) {

			int doffs = ((px%8)+(32*(px/8)))+((stride*4)*(py/4))+(8*(py%4));
			int soffs = px+(xres*py);

			((u_char*)outBuff)[doffs] = ((u_char*)inBuff)[soffs];

		}
	}

}

static void _gdl_texture_ConvertIA4(short xres, short yres, short stride, void *inBuff, void *outBuff) {

	for(short py=0; py<yres; py++) {
		for(short px=0; px<xres; px++) {

			int doffs = ((px%8)+(32*(px/8)))+((stride*4)*(py/4))+(8*(py%4));
			int soffs = 2*(px+(xres*py));

			((u_char*)outBuff)[doffs] = (((u_char*)inBuff)[soffs]/16)|((((u_char*)inBuff)[soffs+1]/16)<<4);

		}
	}

}

static void _gdl_texture_ConvertIA8(short xres, short yres, short stride, void *inBuff, void *outBuff) {

	for(short py=0; py<yres; py++) {
		for(short px=0; px<xres; px++) {

			int doffs = (((px%4)+(16*(px/4)))+(4*(py%4))+((stride*4)*(py/4)));
			int soffs = 2*(px+(xres*py));

			((u_short*)outBuff)[doffs] = ((u_char*)inBuff)[soffs]|(((u_char*)inBuff)[soffs+1]<<8);

		}
	}

}

static void _gdl_texture_ConvertRGB5A3(short xres, short yres, short stride, void *inBuff, void *outBuff) {

	// Converts the raw image data into a RGB5A3 texture (RGB555 for opaque pixels, RGB4A3 for translucent)

	uint32_t *src = (uint32_t*)inBuff;
	uint16_t *dst = (uint16_t*)outBuff;

	for(uint16_t y=0; y<yres; y+=4) {
		for(uint16_t x=0; x<xres; x+=4) {

			uint16_t *doffs=dst+(16*((x/4)+((stride/4)*(y/4))));

			for(uint16_t rows = 0; rows < 4; rows++) {

				*doffs++ = RGBA_TO_RGB5A3(src[((y+rows)*xres)+(x+0)]);
				*doffs++ = RGBA_TO_RGB5A3(src[((y+rows)*xres)+(x+1)]);
				*doffs++ = RGBA_TO_RGB5A3(src[((y+rows)*xres)+(x+2)]);
				*doffs++ = RGBA_TO_RGB5A3(src[((y+rows)*xres)+(x+3)]);

			}

		}
	}

}

static void _gdl_texture_ConvertRGB565(short xres, short yres, short stride, void *inBuff, void *outBuff) {

	// Convert the raw image data into a RGB565 texture (alpha channel will be discarded)

	uint32_t *src = (uint32_t*)inBuff;
	uint16_t *dst = (uint16_t*)outBuff;

	for(uint16_t y=0; y<yres; y+=4) {
		for(uint16_t x=0; x<xres; x+=4) {

			uint16_t *doffs=dst+(16*( (x/4)+((stride/4)*(y/4) )));

			for(uint16_t rows = 0; rows < 4; rows++) {

				*doffs++ = RGBA_TO_RGB565(src[((y+rows)*xres)+(x+0)]);
				*doffs++ = RGBA_TO_RGB565(src[((y+rows)*xres)+(x+1)]);
				*doffs++ = RGBA_TO_RGB565(src[((y+rows)*xres)+(x+2)]);
				*doffs++ = RGBA_TO_RGB565(src[((y+rows)*xres)+(x+3)]);

			}

		}
	}

}

static void _gdl_texture_ConvertRGBA8(short xres, short yres, short stride, void *inBuff, void *outBuff) {

	// Converts the raw image data into a RGBA8 texture (or RGBA32)

	uint8_t *src = (uint8_t*)inBuff;
	uint8_t *dst = (uint8_t*)outBuff;

	for(uint16_t block=0; block<yres; block+=4) {
		for(uint16_t i=0; i<xres; i+=4) {

			uint8_t *doffs=dst+(64*( (i/4)+((stride/4)*(block/4) )));

			for (uint32_t c = 0; c < 4; c++) {

				uint32_t blockWid = (((block+c)*xres)+i)<<2;

				*doffs++ = src[blockWid + 3];  // ar = 0
				*doffs++ = src[blockWid + 0];
				*doffs++ = src[blockWid + 7];  // ar = 1
				*doffs++ = src[blockWid + 4];
				*doffs++ = src[blockWid + 11]; // ar = 2
				*doffs++ = src[blockWid + 8];
				*doffs++ = src[blockWid + 15]; // ar = 3
				*doffs++ = src[blockWid + 12];

			}

			for (uint32_t c = 0; c < 4; c++) {

				uint32_t blockWid = (((block+c)*xres)+i)<<2;

				*doffs++ = src[blockWid + 1];  // gb = 0
				*doffs++ = src[blockWid + 2];
				*doffs++ = src[blockWid + 5];  // gb = 1
				*doffs++ = src[blockWid + 6];
				*doffs++ = src[blockWid + 9];  // gb = 2
				*doffs++ = src[blockWid + 10];
				*doffs++ = src[blockWid + 13]; // gb = 3
				*doffs++ = src[blockWid + 14];

			}

		}
	}

}

static void *_gdl_texture_DownscaleImage(short imgWidth, short imgHeight, short newWidth, short newHeight, void *imgData) {

    void    *outBuff=NULL;

    int     px,py;
    int     soffs;
    int     doffs;


	outBuff = malloc2(4*(newWidth*newHeight));

	if (outBuff == NULL)
		return(NULL);

	for(int ny=0; ny<newHeight; ny++) {
		for(int nx=0; nx<newWidth; nx++) {

			px = imgWidth*((float)nx/newWidth);
			py = imgHeight*((float)ny/newHeight);

			u_short numVal = 0;
			u_short redVal = 0;
			u_short grnVal = 0;
			u_short bluVal = 0;
			u_short alpVal = 0;

			for(int ppy=-1; ppy<=1; ppy++) {
				for(int ppx=-1; ppx<=1; ppx++) {

					if (((px+ppx)>=0)&((px+ppx)<imgWidth)) {
						if (((py+ppy)>=0)&((py+ppy)<imgHeight)) {

							soffs = (px+ppx)+(imgWidth*(py+ppy));

							redVal += RED(((int*)imgData)[soffs]);
							grnVal += GREEN(((int*)imgData)[soffs]);
							bluVal += BLUE(((int*)imgData)[soffs]);
							alpVal += ALPHA(((int*)imgData)[soffs]);

							numVal++;

						}
					}

				}
			}

			doffs = nx+(newWidth*ny);
			((int*)outBuff)[doffs] = RGBA(redVal/numVal, grnVal/numVal, bluVal/numVal, alpVal/numVal);

		}
	}

    return(outBuff);

}


// Internal class functions

void gdl::Texture::_ConvertTexture(short xres, short yres, short stride, void *inBuff, void *outBuff) {

    switch(texFmt) {
	case gdl::I4:

		_gdl_texture_ConvertI4(xres, yres, stride, inBuff, outBuff);
		break;

	case gdl::I8:

		_gdl_texture_ConvertI8(xres, yres, stride, inBuff, outBuff);
		break;

	case gdl::IA4:

		_gdl_texture_ConvertIA4(xres, yres, stride, inBuff, outBuff);
		break;

	case gdl::IA8:

        _gdl_texture_ConvertIA8(xres, yres, stride, inBuff, outBuff);
		break;

	case gdl::RGB5A3:

		_gdl_texture_ConvertRGB5A3(xres, yres, stride, inBuff, outBuff);
		break;

	case gdl::RGB565:

		_gdl_texture_ConvertRGB565(xres, yres, stride, inBuff, outBuff);
		break;

	case gdl::RGBA8:

        _gdl_texture_ConvertRGBA8(xres, yres, stride, inBuff, outBuff);
		break;

    }

}

bool gdl::Texture::_ConvertMipmap(short xres, short yres, void *inBuff) {

    void	*mipmapBuff[2];
    void	*convertBuff = NULL;
	char	*outBuff	 = (char*)gdl::Texture::texData;

    short	mipXres		 = xres;
    short	mipYres		 = yres;


	// Base level mipmap
	switch(gdl::Texture::texFmt) {
	case gdl::I4:
	case gdl::I8:

		convertBuff = _gdl_texture_ConvertRGBtoI8(mipXres, mipYres, inBuff, true);

		if (convertBuff == NULL)
			return(false);

		gdl::Texture::_ConvertTexture(mipXres, mipYres, mipXres, convertBuff, outBuff);

		free2(convertBuff);
		break;

	case gdl::IA4:
	case gdl::IA8:

		convertBuff = _gdl_texture_ConvertRGBtoIA8(mipXres, mipYres, inBuff, true);

		if (convertBuff == NULL)
			return(false);

		gdl::Texture::_ConvertTexture(mipXres, mipYres, mipXres, convertBuff, outBuff);

		free2(convertBuff);
		break;

	default:

		gdl::Texture::_ConvertTexture(mipXres, mipYres, mipXres, inBuff, outBuff);
		break;

	}


	// Generate the other levels
    mipmapBuff[0] = inBuff;
	for(short mipLevel=1; mipLevel<texMipLevels; mipLevel++) {

		// Step to memory address of where mipmap should be stored
		switch(gdl::Texture::texFmt) {
		case gdl::I4:

			outBuff += (mipXres/2)*mipYres;
			break;

		case gdl::IA4:
		case gdl::I8:

			outBuff += mipXres*mipYres;
			break;

		case gdl::IA8:
		case gdl::RGB5A3:
		case gdl::RGB565:

			outBuff += (mipXres*mipYres)*2;
			break;

		case gdl::RGBA8:

			outBuff += (mipXres*mipYres)*4;
			break;

		}

		// Downscale texture
		mipmapBuff[1] = _gdl_texture_DownscaleImage(mipXres, mipYres, mipXres/2, mipYres/2, mipmapBuff[0]);

		if (mipmapBuff[1] == NULL) {

            if (mipmapBuff[0] != inBuff)
				free2(mipmapBuff[0]);

			return(false);

		}

		mipXres /= 2;
		mipYres /= 2;


		// Convert downscaled texture into an appropriate texture format
		switch(gdl::Texture::texFmt) {
		case gdl::I4:
		case gdl::I8:

			convertBuff = _gdl_texture_ConvertRGBtoI8(mipXres, mipYres, mipmapBuff[1], true);
			if (convertBuff == NULL)
				return(false);

			gdl::Texture::_ConvertTexture(mipXres, mipYres, mipXres, convertBuff, outBuff);

			free2(convertBuff);
			break;

		case gdl::IA4:
		case gdl::IA8:

			convertBuff = _gdl_texture_ConvertRGBtoIA8(mipXres, mipYres, mipmapBuff[1], true);
			if (convertBuff == NULL)
				return(false);

			gdl::Texture::_ConvertTexture(mipXres, mipYres, mipXres, convertBuff, outBuff);

			free2(convertBuff);
			break;

		case gdl::RGB565:
		case gdl::RGB5A3:
		case gdl::RGBA8:

			gdl::Texture::_ConvertTexture(mipXres, mipYres, mipXres, mipmapBuff[1], outBuff);
			break;

		}


        if (mipLevel > 1)
            free2(mipmapBuff[0]);

		mipmapBuff[0] = mipmapBuff[1];

	}

	free2(mipmapBuff[1]);

	return(true);

}


// Class functions

gdl::Texture::Texture() {

	gdl::Texture::texAlloc	   = 0;
	gdl::Texture::texFmt	   = 0;
	gdl::Texture::texMipLevels = 0;
	gdl::Texture::texMipmapped = false;
	gdl::Texture::texData	   = NULL;
	gdl::Texture::texXsize	   = 0;
	gdl::Texture::texYsize	   = 0;

}

gdl::Texture::~Texture() {

	if (gdl::Texture::texData != NULL){

		if (gdl::Texture::texAlloc == gdl::MEM1)
			free(gdl::Texture::texData);
		else
			free2(gdl::Texture::texData);

	}

}

bool gdl::Texture::Create(short xSize, short ySize, u_int filterMode, u_int format) {

	// Set stuff
	gdl::Texture::texAlloc	= gdl::wii::TexAllocMode;
	gdl::Texture::texFmt	= format;
	gdl::Texture::texXsize	= xSize;
	gdl::Texture::texYsize	= ySize;


	// Allocate a memory block for the texture image data
	switch(format) {
	case gdl::I4:		// I4 has 2 pixels per byte (4-bit)

		gdl::Texture::texSize = (xSize/2)*ySize;
		break;

	case gdl::I8:		// IA and IA4 is 1 byte per pixel (8-bit)
	case gdl::IA4:

		gdl::Texture::texSize = xSize*ySize;
		break;

	case gdl::IA8:		// IA8, RGB565 and RGB5A3 is 2 bytes per pixel (16-bit)
	case gdl::RGB565:
	case gdl::RGB5A3:

		gdl::Texture::texSize = (xSize*ySize)*2;
		break;

	case gdl::RGBA8:	// RGBA8 is 4 bytes per pixel (32-bit)

		gdl::Texture::texSize = (xSize*ySize)*4;
		break;

	}

	gdl::Texture::texMipmapped = false;


	// If texture is already allocated, delete it for reallocation
	if (gdl::Texture::texData != NULL) {

		if (gdl::Texture::texAlloc == gdl::MEM1)
			free(gdl::Texture::texData);
		else
			free2(gdl::Texture::texData);

	}


	// Allocate texture data block
	if (gdl::Texture::texAlloc == gdl::MEM1)
		gdl::Texture::texData = memalign(32, gdl::Texture::texSize);
	else
		gdl::Texture::texData = malloc2(gdl::Texture::texSize);


	// Out of memory?
	if (gdl::Texture::texData == NULL) {
		gdl::CallErrorCallback("Out of memory allocating %dx%d texture buffer", xSize, ySize);
        return(false);
	}


	// Fill it with zeros and then flush it
	memset(gdl::Texture::texData, 0x00, gdl::Texture::texSize);
	DCFlushRange(gdl::Texture::texData, gdl::Texture::texSize);


	// Setup the GXTexObj struct for rendering
	GX_InitTexObj(&texObj, gdl::Texture::texData, xSize, ySize, format, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_InitTexObjFilterMode(&texObj, filterMode, filterMode);

	return(true);

}

bool gdl::Texture::CreateMipmapped(short xSize, short ySize, u_int minFilt, u_int magFilt, short maxMipmaps, u_int format) {

	// Set stuff
	gdl::Texture::texAlloc	= gdl::wii::TexAllocMode;
	gdl::Texture::texFmt	= format;
	gdl::Texture::texXsize	= xSize;
	gdl::Texture::texYsize	= ySize;


	// Calculate the amount of memory needed to store a mipmapped texture image data
    short mXsize=xSize;
    short mYsize=ySize;
    short numMipmaps=0;

	switch(format) {
	case gdl::I4:		// I4 has 2 pixels per byte (4-bit)

		gdl::Texture::texSize = 0;

        for(numMipmaps=0; ((mXsize>2)|(mYsize>2)); numMipmaps++) {

			if (numMipmaps >= maxMipmaps)
				break;

			gdl::Texture::texSize += (mXsize/2)*mYsize;
			mXsize /= 2;
			mYsize /= 2;

        }

		break;

	case gdl::I8:		// I8 and IA4 is 1 byte per pixel (8-bit)
	case gdl::IA4:

		gdl::Texture::texSize = 0;

        for(numMipmaps=0; ((mXsize>2)|(mYsize>2)); numMipmaps++) {

			if (numMipmaps >= maxMipmaps)
				break;

			gdl::Texture::texSize += mXsize*mYsize;
			mXsize /= 2;
			mYsize /= 2;

        }

		break;

	case gdl::IA8:		// IA8, RGB565 and RGB5A3 is 2 bytes per pixel (16-bit)
	case gdl::RGB565:
	case gdl::RGB5A3:

		gdl::Texture::texSize = 0;

        for(numMipmaps=0; ((mXsize>2)|(mYsize>2)); numMipmaps++) {

			if (numMipmaps >= maxMipmaps)
				break;

			gdl::Texture::texSize += (mXsize*mYsize)*2;
			mXsize /= 2;
			mYsize /= 2;

        }

		break;

	case gdl::RGBA8:	// RGBA8 is 4 bytes per pixel (32-bit)

		gdl::Texture::texSize = 0;
        for(numMipmaps=0; ((mXsize>2)|(mYsize>2)); numMipmaps++) {

			if (numMipmaps >= maxMipmaps)
				break;

			gdl::Texture::texSize += (mXsize*mYsize)*4;
			mXsize /= 2;
			mYsize /= 2;

        }

		break;

	}

	gdl::Texture::texMipLevels = numMipmaps;
	gdl::Texture::texMipmapped = true;


	// If texture is already allocated, delete it for reallocation
	if (gdl::Texture::texData != NULL) {

		if (gdl::Texture::texAlloc == gdl::MEM1)
			free(gdl::Texture::texData);
		else
			free2(gdl::Texture::texData);

	}


	// Allocate texture data block
	if (gdl::Texture::texAlloc == gdl::MEM1)
		gdl::Texture::texData = memalign(32, gdl::Texture::texSize);
	else
		gdl::Texture::texData = malloc2(gdl::Texture::texSize);


	// Out of memory?
	if (gdl::Texture::texData == NULL) {
		gdl::CallErrorCallback("Out of memory for allocating mipmapped %dx%d texture buffer", xSize, ySize);
        return(false);
	}


	// Fill it with zeros and then flush it
	memset(gdl::Texture::texData, 0x00, gdl::Texture::texSize);
	DCFlushRange(gdl::Texture::texData, gdl::Texture::texSize);


	// Setup the GXTexObj struct for rendering
	GX_InitTexObj(&texObj, texData, xSize, ySize, format, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_InitTexObjLOD(&texObj, minFilt, magFilt, 0, texMipLevels-1, 0, GX_DISABLE, GX_TRUE, GX_ANISO_4);

	return(true);

}

bool gdl::Texture::LoadTexture(const char* fileName) {

	if (gdl::ConsoleActive)
		printf("gdl: Loading image %s... ", fileName);


	// Open file
	FILE *fp = fopen(fileName, "rb");
	if (!fp) {
		gdl::CallErrorCallback("Could not find image file %s", fileName);
		fclose(fp);
		return(false);

	}


	// Internal structs
	struct {
		u_int	magic;
		u_int	texCount;
		u_int	headerSize;
	} TPL_Header;

	struct {
		u_int	textureOffs;
		u_int	tlutOffs;
	} TPL_Addr;

	struct {
		u_short	height;
		u_short	width;
		u_int	format;
		u_int	dataOffs;
		u_int	wrap_s;
		u_int	wrap_t;
		u_int	minFilt;
		u_int	magFilt;
		float	lodBias;
		u_char	edgeLod;
		u_char	minLod;
		u_char	maxLod;
		u_char	unpacked;
	} TPL_Texture;


	// Read header
	fseek(fp, 0, SEEK_SET);
	fread(&TPL_Header, 1, sizeof(TPL_Header), fp);

	if (TPL_Header.headerSize != 12) {
		gdl::CallErrorCallback("Unsupported tpl format variant of %s", fileName);
		fclose(fp);
		return(false);
	}

	if (TPL_Header.magic != 0x20af30) {
		gdl::CallErrorCallback("Invalid tpl format of %s.", fileName);
		fclose(fp);
		return(false);
	}

	if (TPL_Header.headerSize != 12) {
		gdl::CallErrorCallback("Unsupported tpl format variant of %s.", fileName);
		fclose(fp);
		return(false);
	}

	if (TPL_Header.texCount != 1) {
		gdl::CallErrorCallback("Multitexture tpl files not supported in %s.", fileName);
		fclose(fp);
		return(false);
	}


	// Get texture offset
	fread(&TPL_Addr, 1, sizeof(TPL_Addr), fp);

	if (TPL_Addr.tlutOffs != 0) {
		gdl::CallErrorCallback("Color-index texture format not supported in %s.", fileName);
		fclose(fp);
		return(false);
	}


	// Load texture data
	fseek(fp, TPL_Addr.textureOffs, SEEK_SET);
	fread(&TPL_Texture, 1, sizeof(TPL_Texture), fp);


	// Set texture class parameters
	gdl::Texture::texAlloc = gdl::wii::TexAllocMode;
	gdl::Texture::texXsize = TPL_Texture.width;
	gdl::Texture::texYsize = TPL_Texture.height;
	gdl::Texture::texFmt   = TPL_Texture.format;


	// Calculate size of the texture data
	fseek(fp, 0, SEEK_END);
    gdl::Texture::texSize = ftell(fp);
	fseek(fp, TPL_Texture.dataOffs, SEEK_SET);
	gdl::Texture::texSize -= ftell(fp);


	// If texture is already allocated, delete it for reallocation
	if (gdl::Texture::texData != NULL) {

		if (gdl::Texture::texAlloc == gdl::MEM1)
			free(gdl::Texture::texData);
		else
			free2(gdl::Texture::texData);

	}


	// Allocate texture
	if (gdl::Texture::texAlloc == gdl::MEM1)
		gdl::Texture::texData = memalign(32, gdl::Texture::texSize);
	else
		gdl::Texture::texData = malloc2(gdl::Texture::texSize);


	// Out of memory?
	if (gdl::Texture::texData == NULL) {
		gdl::CallErrorCallback("Out of memory for allocating %dx%d texture buffer");
        return(false);
	}


	// Load the texture data and then flush it
	fread(gdl::Texture::texData, 1, gdl::Texture::texSize, fp);
	DCFlushRange(gdl::Texture::texData, gdl::Texture::texSize);


	// Setup texture object
	GX_InitTexObj(&texObj, gdl::Texture::texData, TPL_Texture.width, TPL_Texture.height, TPL_Texture.format, TPL_Texture.wrap_s, TPL_Texture.wrap_t, GX_FALSE);

	if (TPL_Texture.maxLod) {

		GX_InitTexObjLOD(&texObj, TPL_Texture.minFilt, TPL_Texture.magFilt, TPL_Texture.minLod, TPL_Texture.maxLod, TPL_Texture.lodBias, GX_DISABLE, TPL_Texture.edgeLod, GX_ANISO_4);

	} else {

		GX_InitTexObjFilterMode(&texObj, TPL_Texture.minFilt, TPL_Texture.magFilt);

	}

	fclose(fp);


	// Finish
	if (gdl::ConsoleActive)
		printf("Ok.\n");

	return(true);

}

bool gdl::Texture::ConvertRawImage(short rawXres, short rawYres, void *rawBuff, short srcFormat) {

	if (texData == NULL)
		return(false);


    void	*tempBuff=NULL;

	switch(srcFormat) {
	case gdl::Gray:

		if (texMipmapped == false) {

            switch(gdl::Texture::texFmt) {
			case gdl::IA4:
			case gdl::IA8:

                tempBuff = _gdl_texture_ConvertI8toIA8(rawXres, rawYres, rawBuff);

				// Out of memory?
				if (tempBuff == NULL) {
					gdl::CallErrorCallback("Out of memory allocating buffers for image conversion");
					return(false);
				}

                gdl::Texture::_ConvertTexture(rawXres, rawYres, gdl::Texture::texXsize, tempBuff, gdl::Texture::texData);

                free2(tempBuff);
				break;

			case gdl::RGB565:
			case gdl::RGB5A3:
			case gdl::RGBA8:

				tempBuff = _gdl_texture_ConvertI8toRGBA(rawXres, rawYres, rawBuff);

				// Out of memory?
				if (tempBuff == NULL) {
					gdl::CallErrorCallback("Out of memory allocating buffers for image conversion");
					return(false);
				}

				gdl::Texture::_ConvertTexture(rawXres, rawYres, gdl::Texture::texXsize, tempBuff, gdl::Texture::texData);

				free2(tempBuff);
				break;

			default:

				gdl::Texture::_ConvertTexture(rawXres, rawYres, gdl::Texture::texXsize, rawBuff, gdl::Texture::texData);

				break;

            }

		} else {

			tempBuff = _gdl_texture_ConvertI8toRGBA(rawXres, rawYres, rawBuff);

			// Out of memory?
			if (tempBuff == NULL) {
				gdl::CallErrorCallback("Out of memory allocating buffers for image conversion");
				return(false);
			}

			if (!gdl::Texture::_ConvertMipmap(rawXres, rawYres, tempBuff)) {
				gdl::CallErrorCallback("Out of memory during mipmap generation");
				free2(tempBuff);
				return(false);
			}

			free2(tempBuff);

		}

		break;

	case gdl::GrayAlpha:

		if (texMipmapped == false) {

			switch(gdl::Texture::texFmt) {
			case gdl::I4:
			case gdl::I8:

				tempBuff = _gdl_texture_ConvertIA8toI8(rawXres, rawYres, rawBuff);

				// Out of memory?
				if (tempBuff == NULL) {
					gdl::CallErrorCallback("Out of memory allocating buffers for image conversion");
					return(false);
				}

                gdl::Texture::_ConvertTexture(rawXres, rawYres, gdl::Texture::texXsize, tempBuff, gdl::Texture::texData);

				free2(tempBuff);
				break;

			case gdl::RGB565:
			case gdl::RGB5A3:
			case gdl::RGBA8:

				tempBuff = _gdl_texture_ConvertIA8toRGBA(rawXres, rawYres, rawBuff);

				// Out of memory?
				if (tempBuff == NULL) {
					gdl::CallErrorCallback("Out of memory allocating buffers for image conversion");
					return(false);
				}

				gdl::Texture::_ConvertTexture(rawXres, rawYres, gdl::Texture::texXsize, tempBuff, gdl::Texture::texData);

				free2(tempBuff);
				break;

			default:

				gdl::Texture::_ConvertTexture(rawXres, rawYres, gdl::Texture::texXsize, rawBuff, gdl::Texture::texData);
				break;

			}

		} else {

            tempBuff = _gdl_texture_ConvertIA8toRGBA(rawXres, rawYres, rawBuff);

			// Out of memory?
			if (tempBuff == NULL) {
				gdl::CallErrorCallback("Out of memory allocating buffers for image conversion");
				return(false);
			}

			if (!gdl::Texture::_ConvertMipmap(rawXres, rawYres, tempBuff)) {
				gdl::CallErrorCallback("Out of memory during mipmap generation");
				free2(tempBuff);
				return(false);
			}

			free2(tempBuff);

		}

		break;

	case gdl::RGB:		// 24-bit true-color image data

		if (texMipmapped == false) {

            switch(gdl::Texture::texFmt) {
			case gdl::I4:
			case gdl::I8:

				tempBuff = _gdl_texture_ConvertRGBtoI8(rawXres, rawYres, rawBuff, false);
				break;

			case gdl::IA4:
			case gdl::IA8:

				tempBuff = _gdl_texture_ConvertRGBtoIA8(rawXres, rawYres, rawBuff, false);
				break;

			default:

				tempBuff = _gdl_texture_ConvertRGBtoRGBA(rawXres, rawYres, rawBuff);
				break;

            }

			// Out of memory?
			if (tempBuff == NULL) {
				gdl::CallErrorCallback("Out of memory allocating buffers for image conversion");
				return(false);
			}

			gdl::Texture::_ConvertTexture(rawXres, rawYres, gdl::Texture::texXsize, tempBuff, gdl::Texture::texData);

			free2(tempBuff);

		} else {

			tempBuff = _gdl_texture_ConvertRGBtoRGBA(rawXres, rawYres, rawBuff);

			// Out of memory?
			if (tempBuff == NULL) {
				gdl::CallErrorCallback("Out of memory allocating buffers for image conversion");
				return(false);
			}

			if (!gdl::Texture::_ConvertMipmap(rawXres, rawYres, tempBuff)) {
				gdl::CallErrorCallback("Out of memory during mipmap generation");
				free2(tempBuff);
				return(false);
			}

			free2(tempBuff);

		}

		break;

	case gdl::RGBA:		// 32-bit true-color image data with alpha

		if (texMipmapped == false) {

            switch(gdl::Texture::texFmt) {
			case gdl::I4:
			case gdl::I8:

				tempBuff = _gdl_texture_ConvertRGBtoI8(rawXres, rawYres, rawBuff, true);

				// Out of memory?
				if (tempBuff == NULL) {
					gdl::CallErrorCallback("Out of memory allocating buffers for image conversion");
					return(false);
				}

				gdl::Texture::_ConvertTexture(rawXres, rawYres, gdl::Texture::texXsize, tempBuff, gdl::Texture::texData);

				free2(tempBuff);
				break;

			case gdl::IA4:
			case gdl::IA8:

				tempBuff = _gdl_texture_ConvertRGBtoIA8(rawXres, rawYres, rawBuff, true);

				// Out of memory?
				if (tempBuff == NULL) {
					gdl::CallErrorCallback("Out of memory allocating buffers for image conversion");
					return(false);
				}

				gdl::Texture::_ConvertTexture(rawXres, rawYres, gdl::Texture::texXsize, tempBuff, gdl::Texture::texData);

				free2(tempBuff);
				break;

			default:

				gdl::Texture::_ConvertTexture(rawXres, rawYres, gdl::Texture::texXsize, rawBuff, gdl::Texture::texData);
				break;

            }

		} else {

			if (!gdl::Texture::_ConvertMipmap(rawXres, rawYres, rawBuff)) {
				gdl::CallErrorCallback("Out of memory allocating buffers for mipmap generation");
				return(false);
			}

		}

		break;

	}

	gdl::Texture::Flush();
	return(true);

}

void gdl::Texture::CopyFromScreen(short x, short y, short width, short height, u_char clearScreen) {

	if (gdl::Texture::texData == NULL)
		return;

	GX_SetTexCopySrc(x, y, width, height);
	GX_SetTexCopyDst(gdl::Texture::texXsize, gdl::Texture::texYsize, texFmt, GX_FALSE);
	GX_CopyTex(gdl::Texture::texData, clearScreen);

}

void gdl::Texture::Delete() {

	if (gdl::Texture::texData != NULL) {

		gdl::Texture::texAlloc = 0;
		gdl::Texture::texFmt   = 0;
		gdl::Texture::texSize  = 0;
		gdl::Texture::texXsize = 0;
		gdl::Texture::texYsize = 0;

		if (gdl::Texture::texAlloc == gdl::MEM1)
			free(gdl::Texture::texData);
		else
			free2(gdl::Texture::texData);

		gdl::Texture::texData	 = NULL;

	}

}

void gdl::Texture::Flush() {

    DCFlushRange(texData, texSize);

}

void gdl::Texture::SetFilterMode(u_int minFilt, u_int magFilt) {

	GX_InitTexObjFilterMode(&texObj, minFilt, magFilt);

}

void gdl::Texture::SetWrapMode(u_int wrap_s, u_int wrap_t) {

	GX_InitTexObjWrapMode(&texObj, wrap_s, wrap_t);

}

void gdl::Texture::PokePixel(short x, short y, u_int col) {

	if ((texData == NULL) || (texMipmapped))
		return;


	register u8*	bp;

	switch(texFmt) {
	case gdl::I4:

		bp = &((u8*)texData)[(((x/2)%4)+(32*((x/2)/4)))+((texXsize*4)*(y/8))+(4*(y%8))];

		if ((x%2) == 0)
			*bp = (*bp&0x0f)|(col<<4);
		else
			*bp = col|(*bp&0xf0);

		break;

	case gdl::I8:

		bp = &((u8*)texData)[((x%8)+(32*(x/8)))+((texXsize*4)*(y/4))+(8*(y%4))];
		*bp = col;
		break;

	case gdl::IA4:

		bp = &((u8*)texData)[((x%8)+(32*(x/8)))+((texXsize*4)*(y/4))+(8*(y%4))];
		*bp = ((col&0xff)/16)|(((col>>8)/16)<<4);
		break;

	case gdl::IA8:

		bp = &((u8*)texData)[2*(((x%4)+(16*(x/4)))+(4*(y%4))+((texXsize*4)*(y/4)))];
		*bp = (col&0xff); bp++;
		*bp = (col>>8);
		break;

	case gdl::RGB565:

		bp = (u8*)(((u16*)texData) + ( ((x%4)+(16*(x/4)) + ((4*(y%4)) + ((texXsize*4)*(y/4))) ) ));
		*((u16*)bp) = RGBA_TO_RGB565(col);
		break;

	case gdl::RGB5A3:

		bp = (u8*)(((u16*)texData) + ( ((x%4)+(16*(x/4)) + ((4*(y%4)) + ((texXsize*4)*(y/4))) ) ));
		*((u16*)bp) = RGBA_TO_RGB5A3(col);
		break;

	case gdl::RGBA8:

		bp = ((u8*)texData) + (((y&(~3))<<2)*texXsize)+((x&(~3))<<4)+((((y&3)<<2)+(x&3))<<1);
		*((u16*)(bp   )) = (u16)((col<<8) | (col>>24));
		*((u16*)(bp+32)) = (u16) (col>>8);
		break;

	}

	if (gdl::wii::AutoFlush)
		DCFlushRange(texData, texSize);

}

void *gdl::Texture::TexAddr() {

	return(texData);

}

u_int gdl::Texture::TexSize() {

	return(texSize);

}

short gdl::Texture::TexFmt() {

	return(texFmt);

}

GXTexObj *gdl::Texture::TexObj() {

	return(&texObj);

}

short gdl::Texture::TXsize() {

	return(texXsize);

}

short gdl::Texture::TYsize() {

	return(texYsize);

}


int gdl::MakeValuePOT(int value) {

	int i;

	for (i=0; (1<<i)<value; i++);
	return(1<<i);

}

void gdl::wii::SetAutoFlush(bool flush) {

	gdl::wii::AutoFlush = flush;

}

void gdl::wii::SetTexAllocMode(bool allocMode) {

	gdl::wii::TexAllocMode = allocMode;

}

#endif
