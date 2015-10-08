// Global library variables module

#ifndef _GDL_WII_GLOBALS
#define _GDL_WII_GLOBALS

#include <gccore.h>

#include "mgdl-wii/mgdl-config.h"
#include "mgdl-wii/mgdl-types.h"
#include "mgdl-wii/mgdl-texture.h"

namespace gdl {

short	ScreenXres;
short	ScreenYres;
short	ScreenCenterX;
short	ScreenCenterY;
bool	ConsoleActive=false;
float	Delta=1.f;

gdl::ErrorCallback ErrorCB=NULL;

namespace wii {

		GXRModeObj*	rmode;
		void*		gp_fifo        = NULL;
		void*		FB[2]          = {NULL,NULL};
		GXRModeObj*	conRMode		= NULL;
		void*		conFB			= NULL;
		short       ActiveFB        = 0;
		void*		UpscaleBuff    = NULL;
		GXTexObj	UpscaleTexObj;

        bool    LowResolutionMode   = false;
		bool	ProgressiveScanMode = false;
		bool	WidescreenMode      = false;
		bool	AutoFlush           = false;
		bool	TexAllocMode        = gdl::MEM1;

		Mtx44	ProjMtx;
		Mtx		ModelMtx;

		bool	DoExitSequence      = false;
		float	DoExitCount         = 0;
		bool	ExitType            = 0;

		float	CPUdelta			= 1.f;
		float	GPUdelta			= 1.f;

		bool	ScreenCapIssued		= false;
        char*	ScreenCapFileName	= NULL;
        ImageCapFunc* ScreenCapFunc = NULL;

		InputPollFunc* PollKeyboardCallback	= NULL;

        #if GDL_NO_AUDIO == FALSE

        const char* LastMusicFile=NULL;

        float	UserMusicVolume=100.f;
        float	MasterMusicVolume=100.f;
        float	MasterSfxVolume=100.f;

        #endif // GDL_NO_AUDIO

}

}

#endif // _GDL_WII_GLOBALS
