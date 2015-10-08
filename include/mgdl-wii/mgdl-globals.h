/*! \file mgdl-globals.h
 *	\brief Global variables module header
 */

#ifndef _GDL_WII_GLOBALS_H
#define _GDL_WII_GLOBALS_H


namespace gdl {


/*!	\addtogroup globalScreenVars Global Screen Variables
 *	\brief Global variables of screen settings.
 *
 *	\details Depending on the video mode and resolution, the screen resolution values may not tell the actual
 *	resolution that the console is running on. One such example is running in widescreen mode where gdl::ScreenXres
 *	would read 854 when the actual resolution is 640 due to the Wii not capable of handling a wide framebuffer and
 *	instead, widescreen is achieved through anamorphic means through the projection matrix.
 *
 *  In 4:3 or low-res mode, gdl::ScreenXres reads the true horizontal resolution.
 *
 *	To get the exact framebuffer resolution regardless of video mode, use the gdl::wii::rmode object.
 *
 *	@{
 */

extern short	ScreenXres;     /*!< Screen X resolution. */
extern short	ScreenYres;     /*!< Screen Y resolution. */
extern short	ScreenCenterX;  /*!< Screen X center position. */
extern short	ScreenCenterY;  /*!< Screen Y center position. */
extern bool		ConsoleActive;  /*!< Console active flag (true if screen is in text console mode). */

//! Delta time between gdl::PrepDisplay() and gdl::Display() calls.
/*!
 *  Meant mostly for time based momentum control to achieve proper frameskipping. The value is a rough estimation of
 *	the time it took between a gdl::PrepDisplay() and gdl::Display() call plus the amount of time it took for the GX
 *	to finish graphics processing. Usually roughly 1.0f when the program is running at 60FPS (NTSC) and roughly 1.2f
 *	when the program is running at 50FPS (PAL).
 *
 */
extern float	Delta;

namespace wii {


	//! Pointer to video and renderer settings (see <a href="http://libogc.devkitpro.org/gx__struct_8h.html#a232a76ec57f12bf0e45abad393c36698"><b>GXRModeObj</b></a> for details).
	/*!
	 *  Modifying this does not really affect the library's graphics routines as it is not directly applied to the
	 *	video encoder when calling most	graphics functions. Used mostly for getting the exact framebuffer resolution.
	 */
    extern GXRModeObj* rmode;


    //! Global projection matrix used by the library's graphics functions.
    /*!
     *  Modifying this does not really affect the library's graphics routines as it is not directly applied to the GX
     *	when calling most graphics functions. Load the projection matrix to the GX with the <a href="http://libogc.devkitpro.org/gx_8h.html#a241a1301f006ed04b7895c051959f64e"><b>GX_LoadProjectionMtx()</b></a> function.
     */
    extern Mtx44 ProjMtx;


    //! Global model matrix object used by the library's graphics functions.
    /*!
     *  Modifying these will also affect the library's graphics routines. Load the model matrix to the GX with the
     *  <a href="http://libogc.devkitpro.org/gx_8h.html#a90349e713128a1fa4fd6048dcab7b5e7"><b>GX_LoadPosMtxImm()</b></a> function.
     */
    extern Mtx ModelMtx;


	//! CPU delta time.
	/*!
	 *  Similar to gdl::Delta but contains the delta time it took between a gdl::PrepDisplay() and gdl::Display() pair
	 *	CPU-wise. It does not read the exact CPU usage but a rough estimation of it useful for profiling/benchmarking.
	 *
	 *	You may want to keep this value to stay less than 1.0f if you want to maintain 60FPS.
	 */
	extern float	CPUdelta;


	//! GPU delta time.
	/*!
	 *  Similar to gdl::Delta but contains the delta time it took for the GPU to finish rendering a frame when
	 *	gdl::Display() is called. It does not read the exact GPU usage but a rough estimation of it useful for
	 *	profiling/benchmarking.
	 *
	 *	You may want to keep this value to stay less than 1.0f if you want to maintain 60FPS.
	 */
	extern float	GPUdelta;


}

/*!	@} */


}


#endif // _GDL_WII_GLOBALS_H
