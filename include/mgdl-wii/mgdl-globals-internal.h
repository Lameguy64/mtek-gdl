/*! \file mgdl-globals-internal.h
 *	\brief Internal global variables header
 *
 *	\details These variables are not really necessary to be accessed or modified outside the library but you can include this header
 *      into your projects if you want to be able to access them.
 */

#ifndef _GDL_WII_GLOBALS_INT_H
#define _GDL_WII_GLOBALS_INT_H

#include "mgdl-wii/mgdl-config.h"
#include "mgdl-wii/mgdl-types.h"


namespace gdl {

//! Error messaging callback pointer
extern gdl::ErrorCallback ErrorCB;

namespace wii {

    //! Pointer to GP FIFO buffer where GP commands are stored and sent to the GP.
    extern void *gp_fifo;

    //! K1 pointers to the display framebuffers, pixel format is in YUV2 format.
    extern void *FB[2];

    //! Console screen framebuffer
    extern void *conFB;

    //! Console rmode settings
    extern GXRModeObj *conRMode;
    //! Pointer for upscale texture buffer for upscaling 240p to a 480p framebuffer (since progressive scan only supports 480/576 vertical resolution).
    extern void *UpscaleBuff;
    //! Texture object for the upscale buffer.
    extern GXTexObj UpscaleTexObj;
    //! Active framebuffer page index.
    extern short ActiveFB;

    //! Automatic texture flush flag (for gdl::Texture::PokePixel()).
    extern bool AutoFlush;
    //! Allocation mode for texture and image classes.
    extern bool TexAllocMode;

    //! Exit sequence flag (for built-in power off/reset handling).
    extern bool DoExitSequence;
    //! Exit sequence counter (for power off/reset).
    extern float DoExitCount;
    //! Exit type (0: Power off, 1: Soft Reset back to Homebrew Channel).
    extern bool ExitType;

	//! Low-resolution mode flag
    extern bool LowResolutionMode;
    //! Progressive scan mode flag
    extern bool ProgressiveMode;
    //! Widescreen mode flag
    extern bool WidescreenMode;

	//! Screen capture issue flag (set by the Image Handling Module)
    extern bool ScreenCapIssued;
    //! Screen capture file name (set by the Image Handling Module)
	extern char* ScreenCapFileName;
	//! Screen capture function pointer (set by the Image Handling Module)
	extern ImageCapFunc* ScreenCapFunc;

	//! Keyboard poll function callback (set by the keyboard portion of the Input Module)
	extern InputPollFunc* PollKeyboardCallback;

    // Sound system variables
    #if GDL_NO_AUDIO == FALSE

    //! Pointer to string of last music file played
    extern const char *LastMusicFile;

    //! User music volume
    extern float UserMusicVolume;

    //! Master music volume
    extern float MasterMusicVolume;

    //! Master sounds volume
    extern float MasterSfxVolume;

    #endif // GDL_NO_AUDIO

}

}


#endif // _GDL_WII_GLOBALS_INT_H
