/*! \file mgdl-basic.h
 *	\brief Basic graphics module header
 */

#ifndef _GDL_WII_BASIC_H
#define _GDL_WII_BASIC_H


namespace gdl {


/*! \addtogroup basicGfxModule Basic Graphics Module
 *	\brief Basic graphics module for simple graphics drawing or prototyping purposes.
 *
 *	\details Because these graphics functions send vertex data directly to the GX, it is best not to call these routines
 *	too redundantly otherwise, a slowdown will occur.
 *
 *	These functions are mostly meant for prototyping purposes.
 *
 *	@{
 */


//! Draws a point or a single pixel.
/*!
 *	\details You can adjust the point size with <a href="http://libogc.devkitpro.org/gx_8h.html#a3a1632e5897acafc1255b2d93236ddcb"><b>GX_SetPointSize()</b></a>
 *		but its going to be tricky since the function does not exactly take the size in pixels units.
 *
 *	\param[in]	x	X coordinate of point.
 *	\param[in]	y	Y coordinate of point.
 *	\param[in]	col	Color of point (see gdl::Color::ColorValues or \ref colorMacros).
 */
void	DrawPoint(short x, short y, u_int col);


//! Draws a line.
/*!
 *	\details You can adjust the point size with <a href="http://libogc.devkitpro.org/gx_8h.html#a18e5556c0f12c84a00b631f24edb6f99"><b>GX_SetLineWidth()</b></a>
 *		but its going to be tricky since the function doesn't really take sizes in pixels exactly.
 *
 *	\param[in]	x1	X coordinate of start point of line.
 *	\param[in]	y1	Y coordinate of start point of line.
 *	\param[in]	x2	X coordinate of end point of line.
 *	\param[in]	y2	Y coordinate of end point of line.
 *	\param[in]	col	Color of point (see gdl::Color::ColorValues or \ref colorMacros).
 */
void	DrawLine(short x1, short y1, short x2, short y2, u_int col);


//! Draws a box.
/*!
 *	\param[in]	x1	X coordinate of upper-left point of box.
 *	\param[in]	y1	Y coordinate of upper-left point of box.
 *	\param[in]	x2	X coordinate of lower-right point of box.
 *	\param[in]	y2	Y coordinate of lower-right point of box.
 *	\param[in]	col	Color of point (see gdl::Color::ColorValues or \ref colorMacros).
 */
void	DrawBox(short x1, short y1, short x2, short y2, u_int col);


//! Draws a filled box.
/*!
 *	\param[in]	x1	X coordinate of upper-left point of box.
 *	\param[in]	y1	Y coordinate of upper-left point of box.
 *	\param[in]	x2	X coordinate of lower-right point of box.
 *	\param[in]	y2	Y coordinate of lower-right point of box.
 *	\param[in]	col	Color of point (see gdl::Color::ColorValues or \ref colorMacros).
 */
void	DrawBoxF(short x1, short y1, short x2, short y2, u_int col);


//! Draws a gouraud-shaded line.
/*!
 *	\param[in]	x1		X coordinate of start point of line.
 *	\param[in]	y1		Y coordinate of start point of line.
 *	\param[in]	x2		X coordinate of end point of line.
 *	\param[in]	y2		Y coordinate of end point of line.
 *	\param[in]	col_a	Color of start point of line (see gdl::Color::ColorValues or \ref colorMacros).
 *	\param[in]	col_b	Color of end point of line.
 */
void	DrawLineG(short x1, short y1, short x2, short y2, u_int col_a, u_int col_b);


//! Draws a gouraud-shaded box.
/*!
 *	\param[in]	x1		X coordinate of upper-left point of box.
 *	\param[in]	y1		Y coordinate of upper-left point of box.
 *	\param[in]	x2		X coordinate of lower-right point of box.
 *	\param[in]	y2		Y coordinate of lower-right point of box.
 *	\param[in]	tl_col	Color of top-left point of box (see gdl::Color::ColorValues or \ref colorMacros).
 *	\param[in]	tr_col	Color of top-right point of box.
 *	\param[in]	br_col	Color of bottom-right point of box.
 *	\param[in]	bl_col	Color of bottom-left point of box.
 */
void	DrawBoxG(short x1, short y1, short x2, short y2, u_int tl_col, u_int tr_col, u_int br_col, u_int bl_col);


//! Draws a filled gouraud-shaded box.
/*!
 *	\param[in]	x1		X coordinate of upper-left point of box.
 *	\param[in]	y1		Y coordinate of upper-left point of box.
 *	\param[in]	x2		X coordinate of lower-right point of box.
 *	\param[in]	y2		Y coordinate of lower-right point of box.
 *	\param[in]	tl_col	Color of top-left point of box (see gdl::Color::ColorValues or \ref colorMacros).
 *	\param[in]	tr_col	Color of top-right point of box.
 *	\param[in]	br_col	Color of bottom-right point of box.
 *	\param[in]	bl_col	Color of bottom-left point of box.
 */
void	DrawBoxFG(short x1, short y1, short x2, short y2, u_int tl_col, u_int tr_col, u_int br_col, u_int bl_col);

/*! @} */


}

#endif // _MGDL_WII_BASIC_H
