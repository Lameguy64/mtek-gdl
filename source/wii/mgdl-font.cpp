// Font system module

#ifndef _GDL_WII_FONT
#define _GDL_WII_FONT

#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <gccore.h>

#include "mgdl-wii/mgdl-types.h"
#include "mgdl-wii/mgdl-globals.h"
#include "mgdl-wii/mgdl-globals-internal.h"
#include "mgdl-wii/mgdl-main.h"
#include "mgdl-wii/mgdl-image.h"
#include "mgdl-wii/mgdl-font.h"


// Fixed-sized font class functions

gdl::FFont::FFont() {

	cw=0;
	ch=0;
	vList=NULL;
	tList=NULL;

}

gdl::FFont::~FFont() {

	if (vList != NULL)
		free(vList);

	if (tList != NULL)
		free(tList);

}

void gdl::FFont::BindSheet(gdl::Image& image, short charw, short charh) {

	short	cx,cy;
	short	tx,ty;
	int		tc;

	charTexObj = image.Texture.TexObj();
	cw = charw;
	ch = charh;


	if (gdl::ConsoleActive)
		printf("gdl: Binding a character font sheet... ");


	if (vList == NULL)
		vList = memalign(32, sizeof(gdl::wii::VERT2s16)*4);

	((gdl::wii::VERT2s16*)vList)[0].x = 0;		((gdl::wii::VERT2s16*)vList)[0].y = 0;
	((gdl::wii::VERT2s16*)vList)[1].x = charw;	((gdl::wii::VERT2s16*)vList)[1].y = 0;
	((gdl::wii::VERT2s16*)vList)[2].x = charw;	((gdl::wii::VERT2s16*)vList)[2].y = charh;
	((gdl::wii::VERT2s16*)vList)[3].x = 0;		((gdl::wii::VERT2s16*)vList)[3].y = charh;

	DCFlushRange(vList, sizeof(gdl::wii::VERT2s16)*4);


	if (tList == NULL)
		tList = memalign(32, (sizeof(gdl::wii::TEX2f32)*4)*256);

	for(cy=0; cy<16; cy++) {
		for(cx=0; cx<16; cx++) {

			tx = charw*cx;
			ty = charh*cy;
			tc = 4*(cx+(16*cy));

			// Upper-left
			((gdl::wii::TEX2f32*)tList)[tc].u	= ((f32)tx+0.01f)/image.Texture.TXsize();
			((gdl::wii::TEX2f32*)tList)[tc].v	= ((f32)ty+0.01f)/image.Texture.TYsize();

			// Upper-right
			((gdl::wii::TEX2f32*)tList)[tc+1].u	= ((f32)(tx+charw)-0.4f)/image.Texture.TXsize();
			((gdl::wii::TEX2f32*)tList)[tc+1].v	= ((f32)ty+0.01f)/image.Texture.TYsize();

			// Lower-left
			((gdl::wii::TEX2f32*)tList)[tc+2].u	= ((f32)(tx+charw)-0.4f)/image.Texture.TXsize();
			((gdl::wii::TEX2f32*)tList)[tc+2].v	= (f32)(ty+charh)/image.Texture.TYsize();

			// Lower-right
			((gdl::wii::TEX2f32*)tList)[tc+3].u	= ((f32)tx+0.01f)/image.Texture.TXsize();
			((gdl::wii::TEX2f32*)tList)[tc+3].v	= (f32)(ty+charh)/image.Texture.TYsize();

		}
	}

	DCFlushRange(tList, (sizeof(gdl::wii::TEX2f32)*4)*256);


	if (gdl::ConsoleActive)
		printf("Ok!\n");

}

void gdl::FFont::DrawText(const char *text, short x, short y, float scale, u32 col) {

	// Draws text using the current font set by gdl_SetCurrentFont()

	float	tx;
	int		c,tc;
	Mtx		TempMatrix;
	GXColor	TempCol;


	if (vList == NULL)
		return;


	if ((x == gdl::Centered) || (x == gdl::PCentered)) {
		tx = gdl::ScreenCenterX - ((((cw+1)*strlen(text))*scale)/2)+(scale/2);
	} else {
		tx = x;
	}

	TempCol.r = RED(col);
	TempCol.g = GREEN(col);
	TempCol.b = BLUE(col);
	TempCol.a = ALPHA(col);


	GX_LoadTexObj(charTexObj, GX_TEXMAP0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
	GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);

	GX_SetArray(GX_VA_POS, vList, 2*sizeof(s16));
	GX_SetArray(GX_VA_TEX0, tList, 2*sizeof(f32));


	guMtxCopy(gdl::wii::ModelMtx, TempMatrix);
	guMtxApplyTrans(TempMatrix, TempMatrix, tx, y, 0);
	guMtxApplyScale(TempMatrix, TempMatrix, scale, scale, 0);
	GX_LoadPosMtxImm(TempMatrix, GX_PNMTX0);

	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GX_SetChanMatColor(GX_COLOR0A0, TempCol);

	for(c=0; text[c] != 0x00; c++) {

		tc = 4*((u_char*)text)[c];

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

			GX_Position1x8(0);
			GX_TexCoord1x16(tc);

			GX_Position1x8(1);
			GX_TexCoord1x16(tc+1);

			GX_Position1x8(2);
			GX_TexCoord1x16(tc+2);

			GX_Position1x8(3);
			GX_TexCoord1x16(tc+3);

		GX_End();

		guMtxApplyTrans(TempMatrix, TempMatrix, cw+1, 0, 0);
		GX_LoadPosMtxImm(TempMatrix, GX_PNMTX0);

	}

	GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);

}

void gdl::FFont::Printf(short x, short y, float scale, u32 col, const char* format, ... ) {

	va_list args;
	char	buff[256];

	va_start(args, format);
	vsprintf(buff, format, args);
	va_end(args);

	DrawText(buff, x, y, scale, col);

}


// Variable-sized font class routines

gdl::Font::Font() {

	vList = NULL;
	tList = NULL;
	memset(charWidth, 0x00, 256);

}

gdl::Font::~Font() {

	if (vList != NULL)
		free(vList);

	if (tList != NULL)
		free(tList);

}

bool gdl::Font::LoadFont(const char *fileName) {

	if (gdl::ConsoleActive)
		printf("gdl: Loading font file... ");


	// Load font file
	FILE *fp;
	if (!(fp = fopen(fileName, "rb"))) {
		gdl::CallErrorCallback("Font file %s not found", fileName);
		return(false);
	}

	u_char fontData[2305];

	if (fread(fontData, 1, 2305, fp) != 2305) {
		gdl::CallErrorCallback("Font file %s might be invalid or corrupt", fileName);
		return(false);
	}

	fclose(fp);


	// Parse the font data
    if (!gdl::Font::LoadFontMem(fontData)) {
		gdl::CallErrorCallback("Font file %s might be invalid or corrupt", fileName);
		return(false);
    }

	if (gdl::ConsoleActive)
		printf("Ok!\n");

	return(true);

}

bool gdl::Font::LoadFontMem(u_char *fontData) {

	memcpy(gdl::Font::charWidth, &fontData[2048], 256);
	gdl::Font::charHeight = fontData[2304];

	// Create vertex arrays for drawing the characters
	if (gdl::Font::vList == NULL)
		gdl::Font::vList = memalign(32, (sizeof(gdl::wii::VERT2s16)*4)*256);

	if (gdl::Font::tList == NULL)
		gdl::Font::tList = memalign(32, (sizeof(gdl::wii::TEX2f32)*4)*256);

	gdl::wii::VERT2s16	*verts = (gdl::wii::VERT2s16*)gdl::Font::vList;
	gdl::wii::TEX2f32	*tcoords = (gdl::wii::TEX2f32*)gdl::Font::tList;

	for(int c=0; c<256; c++) {

		int cc=4*c;
		short tx=8*(c%16);
		short ty=8*(c/16);

		verts[cc].x		= 0;						verts[cc].y		= 0;
		verts[cc+1].x	= gdl::Font::charWidth[c];	verts[cc+1].y	= 0;
		verts[cc+2].x	= gdl::Font::charWidth[c];	verts[cc+2].y	= gdl::Font::charHeight;
		verts[cc+3].x	= 0;						verts[cc+3].y	= gdl::Font::charHeight;

		tcoords[cc].u	= ((float)tx/128);
		tcoords[cc].v	= ((float)ty/128);
		tcoords[cc+1].u	= ((float)(tx+gdl::Font::charWidth[c])/128);
		tcoords[cc+1].v	= ((float)ty/128);
		tcoords[cc+2].u	= ((float)(tx+gdl::Font::charWidth[c])/128);
		tcoords[cc+2].v	= ((float)(ty+gdl::Font::charHeight)/128);
		tcoords[cc+3].u	= ((float)tx/128);
		tcoords[cc+3].v	= ((float)(ty+gdl::Font::charHeight)/128);

	}

	DCFlushRange(gdl::Font::vList, (sizeof(gdl::wii::VERT2s16)*4)*256);
	DCFlushRange(gdl::Font::tList, (sizeof(gdl::wii::TEX2f32)*4)*256);


	// Convert font data into a character sheet
	gdl::Font::charSheet.Create(128, 128, gdl::Nearest, gdl::I4);

	u_char charSheet[128][128];
	for(short cy=0; cy<16; cy++) {
		for(short cx=0; cx<16; cx++) {

			for(short py=0; py<8; py++) {
				u_char col = fontData[(8*(cx+(16*cy)))+py];
				for(short px=0; px<8; px++) {


					if (col & (1<<(7-px)))
						charSheet[(8*cy)+py][(8*cx)+px] = 255;
					else
						charSheet[(8*cy)+py][(8*cx)+px] = 0;

				}
			}

		}
	}

	gdl::Font::charSheet.ConvertRawImage(128, 128, (void*)charSheet, gdl::Gray);

	return(true);

}

int gdl::Font::CalcStrLen(const char *text) {

	int slen=0;
	for(short c=0; c<(short)strlen(text); c++) {
		slen += charWidth[(int)text[c]];
	}

	return(slen-1);

}

int gdl::Font::CalcStrLen(const char *text, int len) {

	int slen=0;
	for(short c=0; c<len; c++) {
		slen += charWidth[(int)text[c]];
	}

	return(slen-1);

}

void gdl::Font::DrawText(const char *text, short x, short y, float scale, u_int col) {

	float	tx;
	GXColor textCol;
	Mtx		tempMatrix;


	if (vList == NULL)
		return;

	if (tList == NULL)
		return;


	if ((x == gdl::Centered) || (x == gdl::PCentered)) {
		tx = gdl::ScreenCenterX-((gdl::Font::CalcStrLen(text)*scale)/2)+(scale/2);
	} else {
		tx = x;
	}


	textCol.r = RED(col);
	textCol.g = GREEN(col);
	textCol.b = BLUE(col);
	textCol.a = ALPHA(col);

	GX_LoadTexObj(gdl::Font::charSheet.TexObj(), GX_TEXMAP0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
	GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);

	GX_SetArray(GX_VA_POS, gdl::Font::vList, 2*sizeof(s16));
	GX_SetArray(GX_VA_TEX0, gdl::Font::tList, 2*sizeof(f32));

	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GX_SetChanMatColor(GX_COLOR0A0, textCol);


	guMtxCopy(gdl::wii::ModelMtx, tempMatrix);
	guMtxApplyTrans(tempMatrix, tempMatrix, tx, y, 0);
	guMtxApplyScale(tempMatrix, tempMatrix, scale, scale, 0);
	GX_LoadPosMtxImm(tempMatrix, GX_PNMTX0);


	for(int i=0; text[i]!=0x00; i++) {

		int tc=4*((u_char)text[i]);

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

			GX_Position1x16(tc);
			GX_TexCoord1x16(tc);

			GX_Position1x16(tc+1);
			GX_TexCoord1x16(tc+1);

			GX_Position1x16(tc+2);
			GX_TexCoord1x16(tc+2);

			GX_Position1x16(tc+3);
			GX_TexCoord1x16(tc+3);

		GX_End();

		guMtxApplyTrans(tempMatrix, tempMatrix, gdl::Font::charWidth[(u_char)text[i]], 0, 0);
		GX_LoadPosMtxImm(tempMatrix, GX_PNMTX0);

	}

	GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);

}

void gdl::Font::Printf(short x, short y, float scale, u32 col, const char* format, ... ) {

	va_list args;
	char	buff[256];

	va_start(args, format);
	vsprintf(buff, format, args);
	va_end(args);

	gdl::Font::DrawText(buff, x, y, scale, col);

}


#endif // _GDL_WII_FONT
