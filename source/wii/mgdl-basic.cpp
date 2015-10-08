// Basic graphics functions (see mgdl-wii.h or library docs for info)

#ifndef _GDL_WII_BASIC
#define _GDL_WII_BASIC

#include <gccore.h>

#include "mgdl-wii/mgdl-types.h"
#include "mgdl-wii/mgdl-globals.h"
#include "mgdl-wii/mgdl-basic.h"


void gdl::DrawPoint(short x, short y, u_int col) {

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	GX_Begin(GX_POINTS, GX_VTXFMT0, 1);

		GX_Position2s16(x, y);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));

	GX_End();

}

void gdl::DrawLine(short x1, short y1, short x2, short y2, u_int col) {

	// Draws a line
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	GX_Begin(GX_LINES, GX_VTXFMT0, 2);

		GX_Position2s16(x1, y1);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));
		GX_Position2s16(x2, y2);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));

	GX_End();

}

void gdl::DrawBox(short x1, short y1, short x2, short y2, u_int col) {

	short tx1,tx2;
	short ty1,ty2;

	if (x1 > x2) {
		tx1 = x2; tx2 = x1;
	} else {
		tx1 = x1; tx2 = x2;
	}

	if (y1 > y2) {
		ty1 = y2; ty2 = y1;
	} else {
		ty1 = y1; ty2 = y2;
	}

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	GX_Begin(GX_LINES, GX_VTXFMT0, 8);

		GX_Position2s16(tx1+1, ty1);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));
		GX_Position2s16(tx2, ty1);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));

		GX_Position2s16(tx2, ty1);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));
		GX_Position2s16(tx2, ty2+1);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));

		GX_Position2s16(tx2, ty2);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));
		GX_Position2s16(tx1, ty2);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));

		GX_Position2s16(tx1, ty2);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));
		GX_Position2s16(tx1, ty1);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));

	GX_End();

}

void gdl::DrawBoxF(short x1, short y1, short x2, short y2, u_int col) {

	short tx1,tx2;
	short ty1,ty2;

	if (x1 > x2) {
		tx1 = x2; tx2 = x1+1;
	} else {
		tx1 = x1; tx2 = x2+1;
	}

	if (y1 > y2) {
		ty1 = y2; ty2 = y1+1;
	} else {
		ty1 = y1; ty2 = y2+1;
	}

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

		GX_Position2s16(tx1, ty1);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));

		GX_Position2s16(tx2, ty1);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));

		GX_Position2s16(tx2, ty2);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));

		GX_Position2s16(tx1, ty2);
		GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));

	GX_End();

}

void gdl::DrawBoxFG(short x1, short y1, short x2, short y2, u_int tl_col, u_int tr_col, u_int br_col, u_int bl_col) {

	short tx1,tx2;
	short ty1,ty2;

	if (x1 > x2) {
		tx1 = x2; tx2 = x1+1;
	} else {
		tx1 = x1; tx2 = x2+1;
	}

	if (y1 > y2) {
		ty1 = y2; ty2 = y1+1;
	} else {
		ty1 = y1; ty2 = y2+1;
	}

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

		GX_Position2s16(tx1, ty1);
		GX_Color4u8(RED(tl_col), GREEN(tl_col), BLUE(tl_col), ALPHA(tl_col));

		GX_Position2s16(tx2, ty1);
		GX_Color4u8(RED(tr_col), GREEN(tr_col), BLUE(tr_col), ALPHA(tr_col));

		GX_Position2s16(tx2, ty2);
		GX_Color4u8(RED(br_col), GREEN(br_col), BLUE(br_col), ALPHA(br_col));

		GX_Position2s16(tx1, ty2);
		GX_Color4u8(RED(bl_col), GREEN(bl_col), BLUE(bl_col), ALPHA(bl_col));

	GX_End();

}

void gdl::DrawBoxG(short x1, short y1, short x2, short y2, u_int tl_col, u_int tr_col, u_int br_col, u_int bl_col) {

	short tx1,tx2;
	short ty1,ty2;

	if (x1 > x2) {
		tx1 = x2; tx2 = x1;
	} else {
		tx1 = x1; tx2 = x2;
	}

	if (y1 > y2) {
		ty1 = y2; ty2 = y1;
	} else {
		ty1 = y1; ty2 = y2;
	}

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	GX_Begin(GX_LINES, GX_VTXFMT0, 8);

		GX_Position2s16(tx1+1, ty1);
		GX_Color4u8(RED(tl_col), GREEN(tl_col), BLUE(tl_col), ALPHA(tl_col));
		GX_Position2s16(tx2, ty1);
		GX_Color4u8(RED(tr_col), GREEN(tr_col), BLUE(tr_col), ALPHA(tr_col));

		GX_Position2s16(tx2, ty1);
		GX_Color4u8(RED(tr_col), GREEN(tr_col), BLUE(tr_col), ALPHA(tr_col));
		GX_Position2s16(tx2, ty2+1);
		GX_Color4u8(RED(br_col), GREEN(br_col), BLUE(br_col), ALPHA(br_col));

		GX_Position2s16(tx2, ty2);
		GX_Color4u8(RED(br_col), GREEN(br_col), BLUE(br_col), ALPHA(br_col));
		GX_Position2s16(tx1, ty2);
		GX_Color4u8(RED(bl_col), GREEN(bl_col), BLUE(bl_col), ALPHA(bl_col));

		GX_Position2s16(tx1, ty2);
		GX_Color4u8(RED(bl_col), GREEN(bl_col), BLUE(bl_col), ALPHA(bl_col));
		GX_Position2s16(tx1, ty1);
		GX_Color4u8(RED(tl_col), GREEN(tl_col), BLUE(tl_col), ALPHA(tl_col));

	GX_End();

}

void gdl::DrawLineG(short x1, short y1, short x2, short y2, u_int col_a, u_int col_b) {

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	GX_Begin(GX_LINES, GX_VTXFMT0, 2);

		GX_Position2s16(x1, y1);
		GX_Color4u8(RED(col_a), GREEN(col_a), BLUE(col_a), ALPHA(col_a));
		GX_Position2s16(x2, y2);
		GX_Color4u8(RED(col_b), GREEN(col_b), BLUE(col_b), ALPHA(col_b));

	GX_End();

}


#endif // _GDL_WII_BASIC
