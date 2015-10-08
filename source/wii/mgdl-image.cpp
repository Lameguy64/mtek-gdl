// Image handling module

#ifndef _GDL_WII_IMAGE
#define _GDL_WII_IMAGE

#include <malloc.h>
#include <gccore.h>
#include <zlib.h>

#include "mgdl-wii/mgdl-config.h"
#include "mgdl-wii/mgdl-types.h"
#include "mgdl-wii/mgdl-globals.h"
#include "mgdl-wii/mgdl-globals-internal.h"
#include "mgdl-wii/mgdl-main.h"
#include "mgdl-wii/mgdl-image.h"


// Common image/sprite drawing functions (shared to the image and sprite drawing classes)

namespace gdl {

	// Extended put with centering, rotation and scaling
	void _put(short x, short y, short cx, short cy, short px, short py, float angle, float scale, u_int col, short xSize, short ySize, GXTexObj *texObj, void *vList, void *tList) {

		/// Draws an image with rotation and scaling

		float ppx,ppy;

		if (cx == gdl::Centered) {
			ppx = (int)xSize/2;
		} else if (cx == gdl::PCentered) {
			ppx = (float)xSize/2;
		} else if (cx == gdl::RJustify) {
			ppx = xSize;
		} else if (cx == gdl::Pivot) {
			ppx = px;
		} else if (cx == gdl::CPivot) {
			ppx = px+0.5f;
		} else {
			ppx = cx;
		}

		if (cy == gdl::Centered) {
			ppy = (int)ySize/2;
		} else if (cy == gdl::PCentered) {
			ppy = (float)ySize/2;
		} else if (cy == gdl::RJustify) {
			ppy = ySize;
		} else if (cy == gdl::Pivot) {
			ppy = py;
		} else if (cy == gdl::CPivot) {
			ppy = py+0.5f;
		} else {
			ppy = cy;
		}


		// Matrix calculation stuff
		Mtx	tempMatrix;

		if ((angle == 0.f) && (scale == 1.f)) {

			guMtxApplyTrans(gdl::wii::ModelMtx, tempMatrix, x-ppx, y-ppy, 0);

		} else {

			guVector tempRot={0, 0, 1};
			Mtx	tempRotMatrix;

			guMtxApplyTrans(gdl::wii::ModelMtx, tempMatrix, x, y, 0);
			guMtxRotAxisDeg(tempRotMatrix, &tempRot, angle);
			guMtxScaleApply(tempRotMatrix, tempRotMatrix, scale, scale, 1);
			guMtxApplyTrans(tempRotMatrix, tempRotMatrix, -ppx, -ppy, 0);
			guMtxConcat(tempMatrix, tempRotMatrix, tempMatrix);

		}

		GX_LoadPosMtxImm(tempMatrix, GX_PNMTX0);


		// Set color channel 0 for color translation
		GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);

		GXColor	TempCol;
		TempCol.r = RED(col);	TempCol.g = GREEN(col);
		TempCol.b = BLUE(col);	TempCol.a = ALPHA(col);
		GX_SetChanMatColor(GX_COLOR0A0, TempCol);


		// Load the texture object of the image
		GX_LoadTexObj(texObj, GX_TEXMAP0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);


		// Set vertex descriptors, bind to the vertex arrays, and then draw
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
		GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX8);

		GX_SetArray(GX_VA_POS, vList, 2*sizeof(s16));
		GX_SetArray(GX_VA_TEX0,	tList, 2*sizeof(f32));


		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

			GX_Position1x8(0);
			GX_TexCoord1x8(0);

			GX_Position1x8(1);
			GX_TexCoord1x8(1);

			GX_Position1x8(2);
			GX_TexCoord1x8(2);

			GX_Position1x8(3);
			GX_TexCoord1x8(3);

		GX_End();


		// Reset model matrix and color channel
		GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);
		GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);

	}

	// Stretched put
	void _putS(short x1, short y1, short x2, short y2, u_int col, short xSize, short ySize, GXTexObj *texObj, void *vList, void *tList) {

		// Draws an image as a stretchable rectangle

		short sx=(x2-x1)+1;
		short sy=(y2-y1)+1;


		// Matrix stuff
		Mtx TempMatrix;
		guMtxCopy(gdl::wii::ModelMtx, TempMatrix);
		guMtxScaleApply(TempMatrix, TempMatrix, (f32)sx/xSize, (f32)sy/ySize, 0);
		guMtxTransApply(TempMatrix, TempMatrix, x1, y1, 0);

		GX_LoadPosMtxImm(TempMatrix, GX_PNMTX0);


		// Load texture and change TEV oprerator
		GX_LoadTexObj(texObj, GX_TEXMAP0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);


		// Set color channel 0 for color translation
		GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);

		GXColor	TempCol;
		TempCol.r = RED(col);	TempCol.g = GREEN(col);
		TempCol.b = BLUE(col);	TempCol.a = ALPHA(col);
		GX_SetChanMatColor(GX_COLOR0A0, TempCol);


		// Set vertex descriptors, bind to the vertex arrays, and then draw
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
		GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX8);

		GX_SetArray(GX_VA_POS, vList, 2*sizeof(s16));
		GX_SetArray(GX_VA_TEX0,	tList, 2*sizeof(f32));


		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

			GX_Position1x8(0);
			GX_TexCoord1x8(0);

			GX_Position1x8(1);
			GX_TexCoord1x8(1);

			GX_Position1x8(2);
			GX_TexCoord1x8(2);

			GX_Position1x8(3);
			GX_TexCoord1x8(3);

		GX_End();


		// Reset model matrix and color channel
		GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);
		GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);

	}

	// Internal screen capture function for gdl::Display()
	static bool _saveScreen(const char* fileName) {

		#define RGBV(r,g,b,a) ( (u32)( ( ((u32)(r))   ) |  \
								((((u32)(g)) &0xFF) << 8) |  \
								((((u32)(b)) &0xFF) <<16) ) )

		png_structp	png_ptr=NULL;
		png_infop	info_ptr=NULL;
		short		sWidth;
		short		sHeight;

		if (gdl::ConsoleActive)
			printf("gdl: Attempting to save png... ");

		sWidth = gdl::wii::rmode->fbWidth;
		sHeight = gdl::wii::rmode->xfbHeight;


		// Create file
		FILE *fp = fopen(fileName, "wb");

		if (!fp) {
			gdl::CallErrorCallback("Cannot create png file: %s", fileName);
			return(false);
		}


		// Initialize stuff
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if (!png_ptr) {
			gdl::CallErrorCallback("Cannot create png struct for creating %s", fileName);
			png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
			return(false);

		}

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			gdl::CallErrorCallback("Cannot create png info struct for creating %s", fileName);
			png_destroy_write_struct(&png_ptr, &info_ptr);
			return(false);
		}

		int jmpRet = setjmp(png_jmpbuf(png_ptr));
		if (jmpRet) {
			gdl::CallErrorCallback("setjmp() call error %d", jmpRet);
			png_destroy_write_struct(&png_ptr, &info_ptr);
			return(false);
		}

		png_init_io(png_ptr, fp);


		// Write header
		jmpRet = setjmp(png_jmpbuf(png_ptr));
		if (jmpRet) {
			gdl::CallErrorCallback("setjmp() call error %d", jmpRet);
			png_destroy_write_struct(&png_ptr, &info_ptr);
			return(false);
		}

		png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

		png_set_IHDR(png_ptr, info_ptr, sWidth, sHeight,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		png_write_info(png_ptr, info_ptr);


		// Now save the image
		jmpRet = setjmp(png_jmpbuf(png_ptr));
		if (jmpRet) {
			gdl::CallErrorCallback("setjmp() call error %d", jmpRet);
			png_destroy_write_struct(&png_ptr, &info_ptr);
			return(false);
		}

		u_char*	rowBuff=(u_char*)malloc(3*sWidth);

		if (gdl::wii::rmode->aa == false) {

			GXColor pixel;

			for(short y=0; y<sHeight; y++) {

				for(short x=0; x<sWidth; x++) {

					GX_PeekARGB(x, y, &pixel);
					rowBuff[(3*x)  ] = pixel.r;
					rowBuff[(3*x)+1] = pixel.g;
					rowBuff[(3*x)+2] = pixel.b;

				}

				png_write_row(png_ptr, (png_const_bytep)rowBuff);

			}

		} else {

			char* fbPix = (char*)MEM_K1_TO_K0(gdl::wii::FB[gdl::wii::ActiveFB]);

			memset(rowBuff, 0x00, 3*sWidth);

			for(short y=0; y<sHeight; y++) {

				for(short x=0; x<sWidth; x+=2) {

					int pix = 2*(x+(sWidth*y));

					u_char y0 = fbPix[pix];
					u_char y1 = fbPix[pix+2];
					u_char u  = fbPix[pix+1];
					u_char v  = fbPix[pix+3];

					/*
					rowBuff[(3*x)  ] = y0;
					rowBuff[(3*x)+1] = y0;
					rowBuff[(3*x)+2] = y0;

					rowBuff[(3*(x+1))  ] = y1;
					rowBuff[(3*(x+1))+1] = y1;
					rowBuff[(3*(x+1))+2] = y1;
					*/

					short C = y0 - 16;
					short D = u - 128;
					short E = v - 128;

					short r = (298*C+409*E+128)/256;
					if (r < 0) r = 0;
					if (r > 255) r = 255;
					short g = (298*C-100*D-208*E+128)/256;
					if (g < 0) g = 0;
					if (g > 255) g = 255;
					short b = (298*C+516*D+128)/256;
					if (b < 0) b = 0;
					if (b > 255) b = 255;

					rowBuff[(3*x)  ] = r;
					rowBuff[(3*x)+1] = g;
					rowBuff[(3*x)+2] = b;

					C = y1 - 16;

					r = (298*C+409*E+128)/256;
					if (r < 0) r = 0;
					if (r > 255) r = 255;
					g = (298*C-100*D-208*E+128)/256;
					if (g < 0) g = 0;
					if (g > 255) g = 255;
					b = (298*C+516*D+128)/256;
					if (b < 0) b = 0;
					if (b > 255) b = 255;

					rowBuff[(3*(x+1))  ] = r;
					rowBuff[(3*(x+1))+1] = g;
					rowBuff[(3*(x+1))+2] = b;

				}

				png_write_row(png_ptr, (png_const_bytep)rowBuff);

			}

		}


		jmpRet = setjmp(png_jmpbuf(png_ptr));
		if (jmpRet) {
			gdl::CallErrorCallback("setjmp() call error %d", jmpRet);
			png_destroy_write_struct(&png_ptr, &info_ptr);
			return(false);
		}

		png_write_end(png_ptr, NULL);
		png_destroy_write_struct(&png_ptr, &info_ptr);

		fclose(fp);


		if (gdl::ConsoleActive)
			printf("Ok!\n");

		return(true);

	}

}


// Internal structs

typedef struct {
	const u_char	*buffer;
	png_uint_32		currentPos;
} _gdl_image_png_MemReaderState;


// Internal functions

void _gdl_image_ReadMemFile(png_structp png_ptr, png_bytep data, png_uint_32 length) {

	_gdl_image_png_MemReaderState *f = (_gdl_image_png_MemReaderState *)png_get_io_ptr(png_ptr);

	memcpy(data, f->buffer + f->currentPos, length);
	f->currentPos += length;

}

png_structp _gdl_image_GetFilePngContext(FILE* fp) {

	// Check if file is really a PNG image
	char header[8];

	fread(header, 1, 8, fp);

	if (png_sig_cmp((const u_char*)header, 0, 8)) {
		gdl::CallErrorCallback("Image file is not a valid png or tpl file");
		return(NULL);
	}

	// Initialize stuff for PNG loading
	png_structp	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		gdl::CallErrorCallback("Could not create png context for image file");
		return(NULL);
	}

	// Set up IO for libpng
	png_init_io(png_ptr, fp);

	return(png_ptr);

}

png_structp _gdl_image_GetMemPngContext(void *imagePtr) {

	// Check if memory address points to a PNG image
	if (png_sig_cmp((const u_char*)imagePtr, 0, 8)) {
		gdl::CallErrorCallback("Memory address does not contain a valid png file");
		return(NULL);
	}

	// Prepare the reader callback routine
	_gdl_image_png_MemReaderState readState;
	readState.buffer	 = (u_char*)imagePtr;
	readState.currentPos = 8;

	png_structp	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
		gdl::CallErrorCallback("Could not create png context");
		return(NULL);
    }

	// Setup reader callback then process the file
    png_set_read_fn(png_ptr, &readState, (png_rw_ptr)_gdl_image_ReadMemFile);

    return(png_ptr);

}


// Internal class functions

void gdl::Image::_PrepareImage(short xRes, short yRes) {

	// Calculate texture coords (for non multiple of 4 images)
	float u = (float)xRes/Texture.TXsize();
	float v = (float)yRes/Texture.TYsize();

	// Generate vertex arrays for fast image drawing
	// (as were going to take advantage of the vertex cache)
	if (vList == NULL) vList = memalign(32, sizeof(s16)*8);
	((s16*)vList)[0] = 0;		((s16*)vList)[1] = 0;
	((s16*)vList)[2] = xRes;	((s16*)vList)[3] = 0;
	((s16*)vList)[4] = xRes;	((s16*)vList)[5] = yRes;
	((s16*)vList)[6] = 0;		((s16*)vList)[7] = yRes;
	DCFlushRange(vList, sizeof(s16)*8);

	if (tList == NULL) tList = memalign(32, sizeof(f32)*8);
	((f32*)tList)[0] = 0;	((f32*)tList)[1] = 0;
	((f32*)tList)[2] = u;	((f32*)tList)[3] = 0;
	((f32*)tList)[4] = u;	((f32*)tList)[5] = v;
	((f32*)tList)[6] = 0;	((f32*)tList)[7] = v;
	DCFlushRange(tList, sizeof(f32)*8);

	xSize = xRes;
	ySize = yRes;

}

bool gdl::Image::_ProcessImage(png_structp png_ptr, u_int minFilt, u_int magFilt, u_int format, bool mipmapped, short maxMips) {

	png_infop	info_ptr;
	png_byte	color_type;
	png_byte	bit_depth;
	png_byte	intr_type;
	short		width,height;
	int			jmpval;


	// Create PNG info context
	info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr) {
		gdl::CallErrorCallback("Could not create info struct when loading image");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return(false);
	}


	// Get function jump data for libpng
	jmpval = setjmp(png_jmpbuf(png_ptr));
	if (jmpval) {
		gdl::CallErrorCallback("setjmp() call error %d", jmpval);
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return(false);

	}


	// Get image information and test for compatibility
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);

	width		= png_get_image_width(png_ptr, info_ptr);
	height		= png_get_image_height(png_ptr, info_ptr);
	color_type	= png_get_color_type(png_ptr, info_ptr);
	bit_depth	= png_get_bit_depth(png_ptr, info_ptr);
	intr_type	= png_get_interlace_type(png_ptr, info_ptr);

	if (intr_type != PNG_INTERLACE_NONE) {
		gdl::CallErrorCallback("Interlaced image files are not supported");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return(false);
	}

	if ((width > 1024) || (height > 1024)) {
		gdl::CallErrorCallback("Resolution of image file too high");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return(false);
	}

	if (bit_depth == 16)
		png_set_strip_16(png_ptr);

	if (color_type == PNG_COLOR_TYPE_PALETTE) {

		png_set_palette_to_rgb(png_ptr);
		color_type = PNG_COLOR_TYPE_RGB;

		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {

			png_set_tRNS_to_alpha(png_ptr);
			color_type = PNG_COLOR_TYPE_RGBA;

		}

	}

	png_read_update_info(png_ptr, info_ptr);


	// Prepare for image loading
	jmpval = setjmp(png_jmpbuf(png_ptr));
	if (jmpval) {
		gdl::CallErrorCallback("setjmp() call error %d", jmpval);
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return(false);
	}


	// Create image
	if (mipmapped) {

		if (!CreateMipmapped(width, height, minFilt, magFilt, maxMips, format)) {
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			return(false);
		}

	} else {

		if (!Create(width, height, magFilt, format)) {
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			return(false);
		}

	}


	// Now load and convert the image data into the specified texture format
	void		*tempBuff=NULL;
	u_char		*tempRow;
	png_bytep	rows[height];

	switch(color_type) {
	case PNG_COLOR_TYPE_GRAY:	// For 8-bit grayscale images

		tempBuff = malloc2(width*height);
		for(short i=0; i<height; i++) {
			rows[i] = ((u_char*)tempBuff)+(width*i);
		}

		png_read_rows(png_ptr, rows, NULL, height);

		Texture.ConvertRawImage(width, height, tempBuff, gdl::Gray);

		break;

	case PNG_COLOR_TYPE_GRAY_ALPHA:	// For grayscale with alpha images

		tempBuff = malloc2(2*(width*height));
		for(short i=0; i<height; i++) {
			rows[i] = ((u_char*)tempBuff)+(width*i);
		}

		png_read_rows(png_ptr, rows, NULL, height);

		Texture.ConvertRawImage(width, height, tempBuff, gdl::GrayAlpha);

		break;

	case PNG_COLOR_TYPE_RGB:	// For 24-bit RGB images

		tempBuff = malloc2((4*width)*height);

		for(short i=0; i<height; i++) {
			rows[i] = ((u_char*)tempBuff)+(4*(width*i));
		}

		png_read_rows(png_ptr, rows, NULL, height);

		tempRow = (u_char*)malloc(3*width);
		for(short i=0; i<height; i++) {
			memcpy(tempRow, rows[i], 3*width);
			for(short x=0; x<width; x++) {
				rows[i][(4*x)  ] = tempRow[(3*x)  ];
				rows[i][(4*x)+1] = tempRow[(3*x)+1];
				rows[i][(4*x)+2] = tempRow[(3*x)+2];
				rows[i][(4*x)+3] = 0xff;
			}
		}

		free(tempRow);

		Texture.ConvertRawImage(width, height, tempBuff, gdl::RGBA);

		break;

	case PNG_COLOR_TYPE_RGBA:	// For 32-bit RGBA images

		tempBuff = malloc2((4*width)*height);
		for(short i=0; i<height; i++) {
			rows[i] = ((u_char*)tempBuff)+(4*(width*i));
		}

		png_read_rows(png_ptr, rows, NULL, height);

		Texture.ConvertRawImage(width, height, tempBuff, gdl::RGBA);

		break;

	default:	// Unsupported color types

		gdl::CallErrorCallback("Png color type %d is not supported", color_type);

		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return(false);

	}


	// Deallocate temporary buffer
	if (tempBuff != NULL)
		free2(tempBuff);


	// Delete the contexes and exit
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	xSize = width;
	ySize = height;

	return(true);

}


// Class functions

gdl::Image::Image() {

	u=0;
	v=0;
	vList=NULL;
	tList=NULL;

}

gdl::Image::~Image() {

	if (vList != NULL)
		free(vList);

	if (tList != NULL)
		free(tList);

}

bool gdl::Image::Create(short xRes, short yRes, u_int filterMode, u_int format) {

	if (!Texture.Create(4*((xRes+3)/4), 4*((yRes+3)/4), filterMode, format))
		return(false);

	_PrepareImage(xRes, yRes);

	return(true);

}

bool gdl::Image::CreateMipmapped(short xRes, short yRes, u_int minFilt, u_int magFilt, short maxMipmaps, u_int format) {

	if (!gdl::Image::Texture.CreateMipmapped(4*((xRes+3)/4), 4*((yRes+3)/4), minFilt, magFilt, maxMipmaps, format))
		return(false);

	_PrepareImage(xRes, yRes);

	return(true);

}

bool gdl::Image::LoadImage(const char *fileName, u_int filterMode, u_int format) {

	if (gdl::ConsoleActive)
		printf("gdl: Loading image %s... ", fileName);


	// Alternate file loader if file is a TPL texture file
	char *extOffs = strchr(fileName, '.');

	if (extOffs) {

		if (strcmp(extOffs+1, "tpl") == 0) {

			Texture.LoadTexture(fileName);

			// Generate vertex arrays for fast image drawing
			// (as were going to take advantage of the vertex cache)
			if (vList == NULL) vList = memalign(32, sizeof(s16)*8);
			((s16*)vList)[0] = 0;					((s16*)vList)[1] = 0;
			((s16*)vList)[2] = Texture.TXsize();	((s16*)vList)[3] = 0;
			((s16*)vList)[4] = Texture.TXsize();	((s16*)vList)[5] = Texture.TYsize();
			((s16*)vList)[6] = 0;					((s16*)vList)[7] = Texture.TYsize();
			DCFlushRange(vList, sizeof(s16)*8);

			if (tList == NULL) tList = memalign(32, sizeof(f32)*8);
			((f32*)tList)[0] = 0;	((f32*)tList)[1] = 0;
			((f32*)tList)[2] = 1;	((f32*)tList)[3] = 0;
			((f32*)tList)[4] = 1;	((f32*)tList)[5] = 1;
			((f32*)tList)[6] = 0;	((f32*)tList)[7] = 1;
			DCFlushRange(tList, sizeof(f32)*8);

			return(true);

		}

	}


	// Open file
	FILE *fp;
	if (!(fp = fopen(fileName, "rb"))) {
		gdl::CallErrorCallback("Image file %s not found", fileName);
		fclose(fp);
		return(false);
	}


	// Decode and convert the image
	png_structp png_ptr;
	if (!(png_ptr = _gdl_image_GetFilePngContext(fp))) {
		fclose(fp);
		return(false);
	}


	if (!gdl::Image::_ProcessImage(png_ptr, 0, filterMode, format, false, 0)) {
		fclose(fp);
		return(false);
	}


	if (gdl::ConsoleActive)
		printf("Ok.\n");


	fclose(fp);
	return(true);

}

bool gdl::Image::LoadImageMipmapped(const char *fileName, u_int minFilt, u_int magFilt, short maxMipmaps, u_int format) {

	if (gdl::ConsoleActive)
		printf("gdl: Loading image %s with mipmap... ", fileName);


	FILE *fp;
	if (!(fp = fopen(fileName, "rb"))) {
		gdl::CallErrorCallback("Image file %s not found", fileName);
		fclose(fp);
		return(false);
	}


	png_structp png_ptr;
	if (!(png_ptr = _gdl_image_GetFilePngContext(fp))) {
		fclose(fp);
		return(false);
	}


	if (!gdl::Image::_ProcessImage(png_ptr, minFilt, magFilt, format, true, maxMipmaps)) {
		fclose(fp);
		return(false);
	}

	if (gdl::ConsoleActive)
		printf("Ok.\n");

	fclose(fp);
	return(true);

}

bool gdl::Image::LoadImageMem(void *imagePtr, u_int filterMode, u_int format) {

	if (gdl::ConsoleActive)
		printf("gdl: Loading image %p... ", imagePtr);

	png_structp png_ptr;
	if (!(png_ptr = _gdl_image_GetMemPngContext(imagePtr)))
		return(false);

	if (!gdl::Image::_ProcessImage(png_ptr, 0, filterMode, format, false, 0))
		return(false);

	if (gdl::ConsoleActive)
		printf("Ok.\n");

	return(true);

}

short gdl::Image::Xsize() {

    return(xSize);

}

short gdl::Image::Ysize() {

    return(ySize);

}

void gdl::Image::Put(short x, short y, u_int col, short cx, short cy, float scale, float angle) {

	_put(x, y, cx, cy, 0, 0, angle, scale, col, xSize, ySize,
				Texture.TexObj(), vList, tList);

}

void gdl::Image::PutS(short x1, short y1, short x2, short y2, u_int col) {

	_putS(x1, y1, x2, y2, col, xSize, ySize,
				Texture.TexObj(), vList, tList);

}


// Extra functions

void gdl::SaveScreen(const char* fileName) {

	if (gdl::wii::ScreenCapIssued)
		return;

	gdl::wii::ScreenCapFunc = _saveScreen;
    gdl::wii::ScreenCapIssued = true;
    gdl::wii::ScreenCapFileName = strdup(fileName);

}


#endif // _GDL_WII_IMAGE
