/*! \file mgdl-main.h
 *	\brief Main library module header
 */

/*!	\mainpage
 *
 *	\version	0.75
 *	\author		Jobert "Lameguy" Villamor (Lameguy64/TheCodingBrony)
 *	\copyright	GNU General Public License
 *
 *	\attention This is a work-in-progress "sneak-peek" version of this library! Avoid using functions that are not documented
 *	yet as they have not yet been finalized.
 *
 *	\section introSectionn Introduction
 *  MTek-GDL Wii is an open-source game development library designed to make homebrew game and application development for the
 *	Nintendo Wii a lot easier to do for both beginner and intermediate programmers. This library supports GX accelerated 2D
 *	graphics routines, sound playback, ogg vorbis streaming and image loading and texture conversion routines.
 *
 *  Despite this library mostly supporting 2D graphics only, the way how this library is written allows one to be able to use
 *	MTek-GDL as a base for developing custom 2D or 3D graphics engines provided that they know how to write their own GX
 *	graphics routines and knowing enough how to make a decent game engine in general.
 *
 *	\section changeLog Changes since v0.66:
 *	- Fixed a bug in oggplayer library where oggplayer.h didn't include ivorbisfile.h which is needed for a certain function
 *	  declaration in the header.
 *
 *	- Removed PutX() functions in both gdl::Image and gdl::SpriteSet classes in place of the improved gdl::Image::Put() and
 *	  gdl::SpriteSet::Put() functions which now has optional parameters for pivot point, rotation angle and scale factor.
 *
 *	- Changed syntax of gdl::InitSystem() function significantly.
 *
 *	- Added multisample anti-aliasing support but only works in hi-res mode.
 *
 *	- Optimized gdl::Display() routine which now performs a EFB to XFB copy prior to GX_DrawDone() to prevent potential
 *	  frame dropping when the framerate drops.
 *
 *	- Removed letterboxed aspect ratio mode, it was pretty buggy and problematic and is much better to implement it yourself
 *	  in the long run.
 *
 *	- Fixed argument order of gdl::Image::LoadImageMipmapped() to match with gdl::Image::CreateMipmapped().
 *
 *	- gdl::SaveScreen() can now take screenshots in anti-aliased modes. The function also now works by setting a
 *	  screen capture request to the main library which is then processed by gdl::Display() when called so that the
 *	  finished scene will always be captured correctly.
 *
 *	- Added gdl::wii::CPUdelta and gdl::wii::GPUdelta global variables for profiling/benchmarking purposes.
 *
 *	- Made gdl::wii::LowResolutionMode, gdl::wii::ProgressiveMode and gdl::wii::WidescreenMode into
 *	  gdl::wii::IsLowResMode(), gdl::wii::IsProgressive() and gdl::wii::IsWidescreen() functions respectively.
 *
 *	- Made gdl::SpriteSet::SpriteInfo() return a const pointer to prevent accidental modification.
 *
 *	- Fixed a bunch of typos and spelling mistakes in the documentation.
 *
 */


#ifndef _GDL_WII_MAIN_H
#define _GDL_WII_MAIN_H


namespace gdl {


/*! \addtogroup mainModule Main Library Module
 *	\brief Functions for setting up the system, debug environments and some screen and video related functions.
 *	@{
 */


/*! Video system options
 *
 *	\details Video system mode options for the gdl::InitSystem() function.
 *
 *	\note It is recommended to use gdl::ModeAuto by default so that the library will choose an appropriate video system
 *		according to the console's settings and region. If you're using a specific video system mode, make sure that your
 *		TV supports it (though most modern TVs pretty much support all modes).
 *	\note S-Video output strangely does not work in PAL mode (there's no workaround as it has something to do with
 *		the console's video encoder).
 *
 */
enum InitVideoMode {
	ModeAuto = 0,		//!< Auto selects a video mode according to the system's video settings.
	ModeNTSC,			//!< Initialize in NTSC video mode (480 lines, 60Hz).
	ModePAL,			//!< Initialize in PAL video mode (524 lines, 50Hz).
	ModeMPAL,			//!< Initialize in MPAL/PAL60 video mode (similar to NTSC: 480 lines, 60Hz).
};

/*! Aspect ratio options
 *
 *  \details Aspect ratio mode options for the gdl::InitSystem() function.
 *
 *	\note In hi-res mode, widescreen is emulated through anamorphic means (2D/3D projection matrix is wide but actual
 *		screen resolution isn't). In low-res mode however, the actual screen resolution is proper wide.
 *
 */
enum InitAspectMode {
	AspectAuto = 0,		//!< Aspect ratio selected automatically (based on console settings).
	Aspect4x3,			//!< Initialize in 4:3 aspect ratio mode (640x480/320x240 in NTSC or MPAL mode, 640x528/320x264 in PAL mode).
	Aspect16x9,			//!< Initialize in 16:9 widescreen aspect ratio mode (854x480/432x240 in NTSC or MPAL mode, 854x528/432x264 in PAL mode).
};

/*! Screen resolution options
 *
 *	\details Screen resolution mode options for the gdl::InitSystem() function.
 */
enum InitResolution {
	HiRes = 0,			//!< Standard hi-res mode (640x480 for NTSC and MPAL, 640x528 for PAL).
	LowRes,				//!< OldSchool low-res mode (320x240/432x240) for NTSC and MPAL, 320x264/432x264 for PAL).
};

/*! Initialization flag options
 *
 *  \details Special initialization flags for the for the gdl::InitSystem() function.
 */
enum InitFlags {
    DontMask	= 1<<0,	//!< Disable screen masking which is used to hide garbage in the FrameBuffer during initialization.
    ForceNonInt	= 1<<1,	//!< Force non-interlaced mode for low resolution modes regardless whether or not a component cable is used.
    AntiAliased	= 1<<2,	//!< Enable anti-aliasing mode (ignored when resolution is set to gdl::LowRes).
};


//! Initializes the system.
/*!
 *	\details This function must be called before using most library functions (usually graphics drawing and sound related). What this
 *      function does is that it initializes the system's video, graphics, and audio subsystems as well as automatically setting up
 *      anamorphic widescreen and progressive scan based on the system's configuration by default.
 *
 *	\note Even though this function initializes most of the important console subsystems, it does not initialize the file and Wiimote
 *      subsystems but they are pretty easy to initialize them yourself anyway (see the included Wii examples).
 *	\note When low-resolution mode is set, the video mode is set to non-interlaced (like a VC title) when the console is set to Standard
 *		480i mode. If a component video cable is used but the console is not set to progressive scan mode, interlacing is enabled by default for
 *		compatibility with HDTVs that may not support non-interlaced video through component. If progressive scan mode is set, low-resolution
 *		is emulated in which the library allocates a full 480/528 line framebuffer but drawing is allowed only at the top-half of it. When
 *		gdl::Display() is called, the top half is expanded vertically to fill the entire framebuffer.
 *	\note When anti-aliased mode is set, the actual screen resolution is reduced vertically by (y/2)-4 to make way for multi-sample
 *		anti-aliasing in the EFB and it also makes pixels of textures look like they were rendered at half the vertical resolution as
 *		MSAA is only done on the edges of polygons. Color depth and z-buffer are also reduced to 16-bit as well (RGB565 Z16 to be exact).
 *
 *	\param[in]	videoMode	Video system mode (see gdl::InitVideoMode)
 *	\param[in]	aspectMode	Aspect ratio mode (see gdl::InitAspectMode)
 *	\param[in]	resolution	Display resolution mode (see gdl::InitResolution)
 *	\param[in]	flags		Init flags (see gdl::InitFlags)
 *
 */
void InitSystem(gdl::InitVideoMode videoMode = gdl::ModeAuto, gdl::InitAspectMode aspectMode = gdl::AspectAuto, gdl::InitResolution resolution = gdl::HiRes, u_int flags = 0);


//! Sets the screen to console mode.
/*!
 *	\details This function sets the screen into a simple TTY console for printing out debug messages through printf(). Use the global
 *      variable gdl::ConsoleActive to tell whether or not console mode is set or not. Since v0.65 and onwards of this library, this
 *		function will allocate its own framebuffer and rmode settings so that the console screen will always be hi-res even when low-res
 *		mode is set but calling gdl::Display() will revert it back to the mode you specified.
 *
 *	\note This mode is cleared automatically once gdl::Display() is called. However, console text is preserved.
 */
void ConsoleMode();


//! Sets the resolution of the 2D projection matrix.
/*!
 *	\details This function changes the resolution of the 2D projection matrix useful if you want to override the default screen matrix
 *      resolution when gdl::Set2Dmode() is called.
 *
 *	\note Changing the 2D projection matrix resolution does not actually change the current screen resolution, just the 2D projection
 *      matrix and thus, it is not advisable to use this function in low-resolution mode if you want to maintain pixel perfect rendering.
 *
 *	\param[in]	xres	X resolution of 2D projection matrix.
 *	\param[in]	yres	Y resolution of 2D projection matrix.
 */
void Set2DMatrixRes(short xres, short yres);


//! Sets the current projection matrix into an orthographic 2D projection matrix.
/*!
 *	\details This is the default projection mode set whenever gdl::PrepDisplay() is called. Use this projection mode for drawing 2D
 *      graphics. You can still do 3D graphics in this mode but it'll be rendered orthographically (no depth).
 *
 *	\note Calling this function also resets the current model matrix and disables depth testing.
 *
 *	\param[in]	farClip	Depth clip range (also mirrored for the negative clip range).
 */
void Set2DMode(float farClip = 100.f);


//! Sets the current projection matrix into a 3D projection matrix.
/*!
 *	\details This projection mode is most ideal for rendering 3D graphics.
 *
 *	\note Calling this function also resets the current model matrix and enables depth testing. However, it does not set vertex
 *		format descriptors for rendering 3D geometry so you'll have to set your own. Whilst setting up your own vertex format descriptor,
 *		do not use GX_VTXFMT0 as that is reserved for the library's 2D graphics functions.
 *
 *	\param[in]	farClip	Far distance clip range.
 */
void Set3DMode(float farClip = 1000.f);


//! Sets the clear color.
/*!
 *	\details Sets the color used when the screen gets cleared.
 *
 *	\note This function does not affect the background color in console mode.
 *
 *	\param[in]	red	Red component value.
 *	\param[in]	grn	Green component value.
 *	\param[in]	blu	Blue component value.
 *	\param[in]	alp	Alpha component value.
 */
void SetClearColor(u_char red, u_char grn, u_char blu, u_char alp);


//! Sets the clip box.
/*!
 *	\details Sets the position of the clip box. It uses the scissor box functions of the GX to achieve clipping.
 *
 *	\note If the box range is outside the screen, it is automatically clipped. Also, the clip box is reset whenever
 *		gdl::PrepDisplay() is called.
 *  \note This function also affects 3D rendering which means graphics will only be drawn within the specified
 *      clip box range.
 *
 *	\param[in]	x1,y1	2D screen coordinate of the top-left position of the box.
 *	\param[in]	x2,y2	2D screen coordinate of the bottom-right position of the box.
 */
void SetClipBox(short x1, short y1, short x2, short y2);


//! Resets the clip box.
void ResetClipBox();


//! Prepares the display for drawing graphics.
/*!
 *	\details Always call this function before calling any of the graphics functions. This function also begins calculation
 *      for the gdl::Delta variable based on the amount of time it took for a gdl::PrepDisplay() and gdl::Display() pair to
 *		complete.
 *
 *  \note This function resets the projection matrix to 2D, model matrix and the clip box.
 */
void PrepDisplay();


//! Display graphics.
/*!
 *	\details Finishes GX rendering, waits for it to finish processing, and then switches framebuffers after a VSync,
 *	    displaying the rendered graphics. It also calculates the delta value for gdl::Delta based on the amount of CPU cycles
 *      passed since gdl::PrepDisplay() was called.
 *
 *	\note Always call this function after drawing any graphics to display it.
 *  \note In order for this function to properly calculate a value for gdl::Delta if you take into account frame skipping,
 *      intensive routines must be written within a gdl::PrepDisplay() and gdl::Display() pair.
 */
void Display();


//! Sets an error callback function.
/*!
 *	\details Sets a function which will be called when an error occurs in the library. To properly define an error callback
 *		function, specify parameters myFunc(const char *message) where *message points to a string of the error message.
 *
 *	\param[in]	func    Function to set as the error handler.
 *
 *	\returns Pointer to previous error callback function.
 */
gdl::ErrorCallback SetErrorCallback(gdl::ErrorCallback func);


//! Calls the currently set error callback function
/*!
 *	\details This function calls the currently set error handler function usually displaying the error message. The default
 *		error callback function will set the screen to high-res mode and then prints out the specified error message.
 *
 *	\note The overall message length must be no longer than 255 characters.
 *
 *	\param[in]	message	String of print message (supports printf() style formatting codes).
 *	\param[in]	...		Print arguments.
 */
void CallErrorCallback(const char *message, ... );


namespace wii {


    //! Reverses the byte order of a variable.
    /*!
     *	\details Reverses the byte order of 16-bit and 32-bit variables useful when handling little-endian binary data.
     *
     *	\param[in,out]	*var    Pointer to variable to reverse byte order.
     *  \param[in]		size    Size of variable in bytes (for convenience, use sizeof()).
     */
    void RevBytes(void *var, int size);


    //! Checks whether or not a Component Video cable is used.
    /*!
     *  \details This function will return a non-zero value if a Component Cable is connected to the console. This is
     *      useful for projects that uses the low-resolution video mode mode and you want to implement a non-interlaced
     *      mode test when your program is first run on a console not set to Progressive Scan to test whether or not the
     *      user's TV supports non-interlaced video through Component video.
     *
     *  \note When implementing a non-interlaced mode test dialog, you must prompt the user to either wait for a certain
     *      period of time (10 seconds is ideal) or press a certain button to revert back to interlaced mode if the user's
     *      TV does not support non-interlaced mode through Component video. However, the video test is not necessary if the
     *      console is running in Progressive Scan mode.
     */
    bool IsComponentCableUsed();


    /*! Checks whether or not the console is set to display in widescreen.
     *
     *	\note Call this function only after gdl::InitSystem() is already called.
     *
     *  \returns Non-zero if the system is running in a low display resolution otherwise its running in standard resolution.
     */
	bool IsWidescreen();


    /*! Checks whether or not the console is set to display in progressive scan mode.
     *
	 *	\note Call this function only after gdl::InitSystem() is already called.
	 *
     *  \returns Non-zero if the system is running in progressive scan mode otherwise it is running in interlaced or
     *		non-interlaced (low-res) mode.
     */
	bool IsProgressive();


    /*! Checks whether or not the console is set in a low display resolution.
     *
     *	\note Call this function only after gdl::InitSystem() is already called.
     *
     *  \returns Non-zero if the system is running in widescreen otherwise its in a standard 4:3 aspect ratio.
     */
	bool IsLowResMode();


    //! Sets the video output to either be interlaced or non-interlaced.
    /*!
     *  \details This function switches between a non-interlaced video more or an interlaced video mode on-the-fly
     *		useful for implementing a non-interlaced compatibility test of the user's TV as not all TVs support
     *		non-interlaced video through component.
     *
     *	\note This function will only work in low-resolution, non-progressive mode since non-interlaced mode is most
     *		ideal for it. If the console is set in progressive scan, the screen is upscaled internally to 480p.
     *
     *  \param[in] nonInterlaced    If true, the video output will become non-interlaced otherwise the video output will be made interlaced.
     */
    void SetNonInterlaced(bool nonInterlaced);


    //! Makes the library perform a program exit with a fade-out
    /*!
     *	\details This function will work only during a gdl::PrepDisplay() and gdl::Display() loop. It pretty much does
     *		the same thing that would happen if you press reset on the console.
     */
    void Exit();


}

/*! @} */


}


#endif // _GDL_WII_MAIN_H
