// Sprite system module functions

#ifndef _GDL_WII_SPRITES
#define _GDL_WII_SPRITES

#include <stdio.h>
#include <malloc.h>

#include "mgdl-wii/mgdl-types.h"
#include "mgdl-wii/mgdl-globals.h"
#include "mgdl-wii/mgdl-globals-internal.h"
#include "mgdl-wii/mgdl-main.h"
#include "mgdl-wii/mgdl-image.h"
#include "mgdl-wii/mgdl-sprites.h"


// External declarations to be able to access the image module's put routines
namespace gdl {

//extern void _put(short x, short y, u_int col, GXTexObj *texObj, void *vList, void *tList);
extern void _put(short x, short y, short cx, short cy, short px, short py, float angle, float scale, u_int col, short xSize, short ySize, GXTexObj *texObj, void *vList, void *tList);
extern void _putS(short x1, short y1, short x2, short y2, u_int col, short xSize, short ySize, GXTexObj *texObj, void *vList, void *tList);

}


// Class functions

gdl::SpriteSet::SpriteSet() {

	sheetList=NULL;
	spriteList=NULL;
	numSprites=0;
	numSheets=0;

}

gdl::SpriteSet::~SpriteSet() {

	// Deallocate sprite lists if allocated
	if (spriteList != NULL) {
		short i;
		for(i=numSprites-1; i>=0; i--) {
			free(spriteList[i].vList);
			free(spriteList[i].tList);
		}
		free(spriteList);
		spriteList=NULL;
	}


	// Deallocate sheet images if allocated
	if (sheetList != NULL) {
		short i;
		for(i=numSheets-1; i>=0; i--) delete sheetList[i];
		free(sheetList);
		sheetList=NULL;
	}

	numSprites=0;
	numSheets=0;

}

bool gdl::SpriteSet::LoadSprites(const char *fileName, const char *sheetsdir, u_int filtermode, u_int format) {

	// Open up the file
	FILE *fp;

	if (gdl::ConsoleActive) printf("gdl: Loading sprites in file %s...", fileName);

	if (!(fp = fopen(fileName, "r"))) {
		gdl::CallErrorCallback("Sprite map file %s not found.", fileName);
		return(false);
	}


	// Check the header
	struct {
		char	id[3];
		char	ver;
		short	numSheets;
		short	numSprites;
	} TSM_header;

	fread(&TSM_header, 1, sizeof(TSM_header), fp);
	gdl::wii::RevBytes(&TSM_header.numSheets, sizeof(short));
	gdl::wii::RevBytes(&TSM_header.numSprites, sizeof(short));

	if (strncmp(TSM_header.id, "TSM", 3)) {
		gdl::CallErrorCallback("%s not a TSM sprite map file.", fileName);
		fclose(fp);
		return(false);
	}

	if (TSM_header.ver != 0) {
		gdl::CallErrorCallback("Format version of %s is not supported.", fileName);
		fclose(fp);
		return(false);
	}

	if (gdl::ConsoleActive)
		printf("Ok!\n");


	// Set important private variables and then allocate sprite and sheet lists
	numSprites	= TSM_header.numSprites;
	numSheets	= TSM_header.numSheets;
	spriteList	= (Sprite*)calloc(TSM_header.numSprites, sizeof(Sprite));
	sheetList	= (Image**)calloc(TSM_header.numSheets, sizeof(Image*));


	// Load the sprite sheet images
	if (gdl::ConsoleActive) printf("gdl: Now attempting to load %d sprite sheet(s).\n", TSM_header.numSheets);

	short	i;
	char	namelen,namebuff[64];
	char	pathbuff[256];

	for(i=0; i<TSM_header.numSheets; i++) {

		fread(&namelen, 1, 1, fp);
		fread(namebuff, 1, namelen, fp);
		namebuff[(short)namelen] = 0x00;

		strcpy(pathbuff, sheetsdir);
		strcat(pathbuff, namebuff);

		sheetList[i] = new Image;
		if (!(sheetList[i]->LoadImage(pathbuff, filtermode, format))) {

			for (short d=i-1; d>=0; d--) delete sheetList[d];
			for (short d=i-1; d>=0; d--) {
				delete spriteList[d].vList;
				delete spriteList[d].tList;
			}

			free(spriteList);	spriteList=NULL;
			free(sheetList);	sheetList=NULL;
			return(false);

		}

	}


	// Get the sprite coords
	struct {
		short	sheetnum;
		short	tx1,ty1;
		short	tx2,ty2;
		short	px,py;
	} TSM_entry;

	for(i=0; i<TSM_header.numSprites; i++) {

		// Read an entry and then flip its bytes around to make it BE
		fread(&TSM_entry, 1, sizeof(TSM_entry), fp);
		gdl::wii::RevBytes(&TSM_entry.sheetnum, sizeof(short));
		gdl::wii::RevBytes(&TSM_entry.tx1, sizeof(short));
		gdl::wii::RevBytes(&TSM_entry.ty1, sizeof(short));
		gdl::wii::RevBytes(&TSM_entry.tx2, sizeof(short));
		gdl::wii::RevBytes(&TSM_entry.ty2, sizeof(short));
		gdl::wii::RevBytes(&TSM_entry.px, sizeof(short));
		gdl::wii::RevBytes(&TSM_entry.py, sizeof(short));

		// Calculate sprite width and height and set the sprite's center coords
		spriteList[i].w = (TSM_entry.tx2 - TSM_entry.tx1) + 1;
		spriteList[i].h = (TSM_entry.ty2 - TSM_entry.ty1) + 1;
		spriteList[i].cx = TSM_entry.px;
		spriteList[i].cy = TSM_entry.py;

		// Generate vertex list
		spriteList[i].vList = (wii::VERT2s16*)memalign(32, sizeof(wii::VERT2s16)*4);
		spriteList[i].vList[0].x = 0;					spriteList[i].vList[0].y = 0;
		spriteList[i].vList[1].x = spriteList[i].w;	spriteList[i].vList[1].y = 0;
		spriteList[i].vList[2].x = spriteList[i].w;	spriteList[i].vList[2].y = spriteList[i].h;
		spriteList[i].vList[3].x = 0;					spriteList[i].vList[3].y = spriteList[i].h;
		DCFlushRange(spriteList[i].vList, sizeof(wii::VERT2s16)*4);

		// Generate texcoord list
		TSM_entry.tx2++;
		TSM_entry.ty2++;
		spriteList[i].tList = (wii::TEX2f32*)memalign(32, sizeof(wii::TEX2f32)*4);
		spriteList[i].tList[0].u = ((float)TSM_entry.tx1+0.01f)	/sheetList[TSM_entry.sheetnum-1]->Texture.TXsize();
		spriteList[i].tList[0].v = ((float)TSM_entry.ty1+0.01f)	/sheetList[TSM_entry.sheetnum-1]->Texture.TYsize();
		spriteList[i].tList[1].u = (float)TSM_entry.tx2			/sheetList[TSM_entry.sheetnum-1]->Texture.TXsize();
		spriteList[i].tList[1].v = ((float)TSM_entry.ty1+0.01f)	/sheetList[TSM_entry.sheetnum-1]->Texture.TYsize();
		spriteList[i].tList[2].u = (float)TSM_entry.tx2			/sheetList[TSM_entry.sheetnum-1]->Texture.TXsize();
		spriteList[i].tList[2].v = (float)TSM_entry.ty2			/sheetList[TSM_entry.sheetnum-1]->Texture.TYsize();
		spriteList[i].tList[3].u = ((float)TSM_entry.tx1+0.01f)	/sheetList[TSM_entry.sheetnum-1]->Texture.TXsize();
		spriteList[i].tList[3].v = (float)TSM_entry.ty2			/sheetList[TSM_entry.sheetnum-1]->Texture.TYsize();
		DCFlushRange(spriteList[i].tList, sizeof(wii::VERT2s16)*4);

		// Set appropriate texture object
		spriteList[i].texObj = sheetList[TSM_entry.sheetnum-1]->Texture.TexObj();

	}


	// Finish off the rest
	fclose(fp);

	if (gdl::ConsoleActive)
		printf("gdl: Sprite sheets loaded successfully.\n");

	return(true);

}

short gdl::SpriteSet::NumSprites() {

	return(numSprites);

}

const gdl::Sprite *gdl::SpriteSet::SpriteInfo(short index) {

	return(&spriteList[index]);

}

void gdl::SpriteSet::Put(short x, short y, int index, u_int col, short cx, short cy, float scale, float angle) {

	_put(x, y, cx, cy, spriteList[index].cx, spriteList[index].cy, angle, scale, col,
		spriteList[index].w, spriteList[index].h, spriteList[index].texObj, spriteList[index].vList, spriteList[index].tList);

}

void gdl::SpriteSet::PutS(short x1, short y1, short x2, short y2, short index, u_int col) {

	_putS(x1, y1, x2, y2, col, spriteList[index].w, spriteList[index].h,
		spriteList[index].texObj, spriteList[index].vList, spriteList[index].tList);

}


#endif // _GDL_WII_SPRITES
