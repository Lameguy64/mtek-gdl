// Main library module

#ifndef _GDL_WII_MAIN
#define _GDL_WII_MAIN

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <strings.h>
#include <math.h>

#include <png.h>
#include <zlib.h>

#include <gccore.h>

#include <mgdl-wii/mgdl-config.h>

#if GDL_NO_SOUND == FALSE
#include <asndlib.h>
#endif

#include "mgdl-wii/mgdl-types.h"
#include "mgdl-wii/mgdl-globals.h"
#include "mgdl-wii/mgdl-globals-internal.h"
#include "mgdl-wii/mgdl-main.h"
#include "mgdl-wii/mgdl-basic.h"


namespace gdl {

	static void _powerCallback() {

		// Power button exit (shutdown)

		if (gdl::wii::DoExitSequence == false) {

			gdl::wii::DoExitSequence = true;
			gdl::wii::ExitType = 0;

		}

	}

	static void _resetCallback() {

		// Reset button exit (exit to menu)

		if (gdl::wii::DoExitSequence == false) {

			gdl::wii::DoExitSequence = true;
			gdl::wii::ExitType = 1;

		}

	}

	static u64 _timer_getCpuCycles() {

		// Function based from libprofile by Ian Callaghan (ian@unrom.com)

		u32 tb_upper;
		u32 tb_upper1;
		u32 tb_lower;
		u64 time;

		do {

			__asm__ __volatile__ ("mftbu %0" : "=r"(tb_upper));
			__asm__ __volatile__ ("mftb %0" : "=r"(tb_lower));
			__asm__ __volatile__ ("mftbu %0" : "=r"(tb_upper1));

		} while (tb_upper != tb_upper1);

		time = (((u64)tb_upper)<<32)|tb_lower;

		return time;

	}

	static void _defaultErrorCallback(const char *message) {

		gdl::ConsoleMode();
		printf("ERROR: %s\n", message);

	}

	static u64 _timer_lastCycles;
	static u64 _timer_CPUcycles;
	static u64 _timer_GPUcycles;

}


void gdl::CallErrorCallback(const char *message, ... ) {

    if (gdl::ErrorCB == NULL)
		return;

	char	buff[256];
	va_list args;

	va_start(args, message);
	vsprintf(buff, message, args);
	va_end(args);

	gdl::ErrorCB(buff);

}


void gdl::InitSystem(gdl::InitVideoMode videoMode, gdl::InitAspectMode aspectMode, gdl::InitResolution resolution, u_int flags) {


	// Check if the system is configured for Widescreen and set an appropriate resolution accordingly
	if (resolution == gdl::LowRes) {

		// Low-resolution mode
		gdl::wii::LowResolutionMode = true;

		if (aspectMode == gdl::Aspect4x3) {

			gdl::ScreenXres = 320;
			gdl::wii::WidescreenMode = false;

		} else {

			if ((CONF_GetAspectRatio() == CONF_ASPECT_16_9) || (aspectMode == gdl::Aspect16x9)) {

				gdl::ScreenXres = 432;
				gdl::wii::WidescreenMode = true;

			} else {

				gdl::ScreenXres = 320;
				gdl::wii::WidescreenMode = false;

			}

		}

	} else {

		// Standard-resolution mode
		if (aspectMode == gdl::Aspect4x3) {

			gdl::ScreenXres = 640;
			gdl::wii::WidescreenMode = false;

		} else {

			if ((CONF_GetAspectRatio() == CONF_ASPECT_16_9) || (aspectMode == gdl::Aspect16x9)) {

				gdl::ScreenXres = 854;
				gdl::wii::WidescreenMode = true;

			} else {

				gdl::ScreenXres = 640;
				gdl::wii::WidescreenMode = false;

			}

		}

	}


	// Get horizontal screen offset setting
	s8	hoffset;
	CONF_GetDisplayOffsetH(&hoffset);


	// Initialize the video system
	VIDEO_Init();


	// Get preferred video mode (or use a preset if overridden)
	if (videoMode == gdl::ModeNTSC) {

		gdl::wii::rmode = &TVNtsc480Int;

	} else if (videoMode == gdl::ModePAL) {

		gdl::wii::rmode = &TVPal528Int;

	} else if (videoMode == gdl::ModeMPAL) {

		gdl::wii::rmode = &TVMpal480IntDf;

	} else {

		gdl::wii::rmode = VIDEO_GetPreferredMode(NULL);

	}

	// Copy preferred rmode settings for console mode
	gdl::wii::conRMode = (GXRModeObj*)malloc(sizeof(GXRModeObj));
    memcpy(gdl::wii::conRMode, gdl::wii::rmode, sizeof(GXRModeObj));


	// Set parameters for low-res mode
	if (gdl::wii::LowResolutionMode) {

		gdl::wii::rmode->fbWidth = gdl::ScreenXres;
		gdl::ScreenYres			 = gdl::wii::rmode->efbHeight/2;

		if (flags & gdl::ForceNonInt) {

			gdl::wii::rmode->viTVMode = VI_TVMODE(gdl::wii::rmode->viTVMode>>2, VI_NON_INTERLACE);

		} else {

			if (VIDEO_HaveComponentCable() == false) {

				// If standard composite cables are used, use non-interlaced mode
				gdl::wii::rmode->viTVMode = VI_TVMODE(gdl::wii::rmode->viTVMode>>2, VI_NON_INTERLACE);

			} else {

				// If component cables are used but console is not in progressive scan mode
				if ((gdl::wii::rmode->viTVMode & 3) != VI_PROGRESSIVE) {

					// Set interlaced mode for compatibility with some HDTVs
					gdl::wii::rmode->viTVMode = VI_TVMODE(gdl::wii::rmode->viTVMode>>2, VI_INTERLACE);

				} else {

					// If in progressive mode, create a texture buffer for framebuffer upscaling
					gdl::wii::UpscaleBuff = memalign(32, (gdl::ScreenXres*gdl::ScreenYres)*4);
					DCFlushRange(gdl::wii::UpscaleBuff, (gdl::ScreenXres*gdl::ScreenYres)*4);

					GX_InitTexObj(
						&gdl::wii::UpscaleTexObj, gdl::wii::UpscaleBuff,
						gdl::ScreenXres, gdl::ScreenYres,
						GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE
					);

					GX_InitTexObjFilterMode(&gdl::wii::UpscaleTexObj, GX_NEAR, GX_NEAR);


				}

			}

		}

		// Set video parameters for non-progressive low-res mode
		if ((gdl::wii::rmode->viTVMode&3) != VI_PROGRESSIVE) {

			// Set single-field FrameBuffer mode to get non-interlaced mode working
			gdl::wii::rmode->xfbMode =VI_XFBMODE_SF;

			// Halve the embedded and external FrameBuffer heights
			gdl::wii::rmode->efbHeight	/=2;
			gdl::wii::rmode->xfbHeight	/=2;

		}

	} else {

		gdl::ScreenYres = gdl::wii::rmode->efbHeight;

		if (flags & gdl::AntiAliased) {

			// Sample pattern required for multi-sample anti-aliasing
			u_char samplePattern[12][2] = {
				{4,2},{8,6},{4,10},  // pix 0, 3 sample points, 1/12 units, 4 bits each
				{4,2},{8,6},{4,10},  // pix 1
				{8,2},{4,6},{8,10},  // pix 2
				{8,2},{4,6},{8,10}   // pix 3
			};

			// Vertical filter pattern, 1/64 units, 6 bits each
			u_char vfilterPattern[7] = {
				4,         // line n-1
				8,         // line n-1
				12,        // line n
				16,        // line n
				12,        // line n
				8,         // line n+1
				4          // line n+1
			};

			gdl::wii::rmode->efbHeight = (gdl::wii::rmode->efbHeight/2)-4;
			// Copy sample pattern to RMode
			memcpy(gdl::wii::rmode->sample_pattern, samplePattern, 12*2);
			// Copy vertical filter pattern to RMode
			memcpy(gdl::wii::rmode->vfilter, vfilterPattern, 7);
			gdl::wii::rmode->aa = GX_TRUE;

		}

	}


	// Allocate the FrameBuffers
	gdl::wii::FB[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(gdl::wii::rmode));
	gdl::wii::FB[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(gdl::wii::rmode));

	// Clear them to keep things clear
	VIDEO_ClearFrameBuffer(gdl::wii::rmode, gdl::wii::FB[0], 0x00);
	VIDEO_ClearFrameBuffer(gdl::wii::rmode, gdl::wii::FB[1], 0x00);


	// Set video parameters specific for certain video configurations
	if (gdl::wii::WidescreenMode) {

		if (gdl::wii::LowResolutionMode) {
			gdl::wii::rmode->viWidth += 48;
			gdl::wii::rmode->viXOrigin = 17+hoffset;
		} else {
			gdl::wii::rmode->viWidth += 46;
			gdl::wii::rmode->viXOrigin = 18+hoffset;
		}

	} else {

		gdl::wii::rmode->viXOrigin = 42+hoffset;

	}

	if (gdl::wii::rmode->viWidth > 720)
		gdl::wii::rmode->viWidth = 720;


	// Apply video settings and set the first FrameBuffer to the video encoder
	VIDEO_Configure(gdl::wii::rmode);
	VIDEO_SetNextFramebuffer(gdl::wii::FB[gdl::wii::ActiveFB]);

	if (!(flags & gdl::DontMask))
		VIDEO_SetBlack(true);
	else
		VIDEO_SetBlack(false);

	VIDEO_Flush();
	VIDEO_WaitVSync();
	if (gdl::wii::rmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();


	// Allocate for the GP fifo buffer
	gdl::wii::gp_fifo = memalign(32, GDL_GP_FIFO_SIZE);
	memset(gdl::wii::gp_fifo, 0, GDL_GP_FIFO_SIZE);


	// Initialize the FIFO buffers
	GX_Init(gdl::wii::gp_fifo, GDL_GP_FIFO_SIZE);


	// Set the clear color and z buffer values
	GX_SetCopyClear((GXColor){ 0, 0, 0, 0 }, GX_MAX_Z24);


	// Set the viewport and scissor box
	GX_SetViewport(0, 0, gdl::wii::rmode->fbWidth, gdl::wii::rmode->efbHeight, 0, 1);
	GX_SetScissor(0, 0, gdl::wii::rmode->fbWidth, gdl::wii::rmode->efbHeight);


	// Set a bunch more stuff
	if (gdl::wii::rmode->aa)
        GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);	// For anti-aliased mode
	else
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);	// For non anti-aliased mode

	// Calculate Y scale factor when copying pixels from the EFB to the XFB
	float yscale = GX_GetYScaleFactor(gdl::wii::rmode->efbHeight, gdl::wii::rmode->xfbHeight);
	short xfbHeight = GX_SetDispCopyYScale(yscale);

	GX_SetDispCopySrc(0, 0, gdl::wii::rmode->fbWidth, gdl::wii::rmode->efbHeight);
	GX_SetDispCopyDst(gdl::wii::rmode->fbWidth, xfbHeight);

	GX_SetCopyFilter(gdl::wii::rmode->aa, gdl::wii::rmode->sample_pattern, GX_FALSE, gdl::wii::rmode->vfilter);
	GX_SetFieldMode(gdl::wii::rmode->field_rendering, ((gdl::wii::rmode->viHeight == 2*gdl::wii::rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	GX_SetDither(GX_FALSE);
	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(gdl::wii::FB[gdl::wii::ActiveFB], GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);


	// Set to the default 2D orthogonal projection matrix
	guOrtho(gdl::wii::ProjMtx, -0.325f, (f32)gdl::ScreenYres - 0.325f, -0.325f, (f32)gdl::ScreenXres - 0.325f, 0, 16);
	GX_LoadProjectionMtx(gdl::wii::ProjMtx, GX_ORTHOGRAPHIC);


	// Clear the vertex descriptor
	GX_ClearVtxDesc();


	// Set the first vertex format slot
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS,	GX_POS_XY,	GX_S16,	0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);


	// Set color chanels, texgens and some texture parameters
	GX_SetNumChans(1);
	GX_SetNumTexGens(1);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);


	// Set color blending mode and Z clipping mode
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);


	// Fix that makes lines draw properly in low-res mode
	if (resolution == gdl::LowRes)
		GX_SetLineWidth(8, 0);


	// Initialize the ASND sound subsystems
	#if GDL_NO_SOUND == FALSE
	ASND_Init();
	#endif


	gdl::ScreenCenterX = gdl::ScreenXres/2;
	gdl::ScreenCenterY = gdl::ScreenYres/2;


	// Set power-off and reset callbacks
	SYS_SetPowerCallback(_powerCallback);
	SYS_SetResetCallback(_resetCallback);


	// Set default error handling callback
	gdl::SetErrorCallback(_defaultErrorCallback);

}

void gdl::ConsoleMode() {

	if (gdl::ConsoleActive)
		return;

	if (gdl::wii::conFB == NULL) {

		gdl::wii::conFB = MEM_K0_TO_K1(SYS_AllocateFramebuffer(gdl::wii::conRMode));
		VIDEO_ClearFrameBuffer(gdl::wii::conRMode, gdl::wii::conFB, 0x00);

		console_init(gdl::wii::conFB, 0, 0, gdl::wii::conRMode->fbWidth, gdl::wii::conRMode->xfbHeight,
					 gdl::wii::conRMode->fbWidth*VI_DISPLAY_PIX_SZ);

		printf("\x1b[1;0H");
		printf("MTek-GDL Wii v%s - Console Screen\n", GDL_VERSION);
		printf("----------------\n");

	}

	VIDEO_Configure(gdl::wii::conRMode);
	VIDEO_SetNextFramebuffer(gdl::wii::conFB);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if (gdl::wii::rmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();

	gdl::ConsoleActive=true;

}

void gdl::Set2DMatrixRes(short xres, short yres) {

	gdl::ScreenXres = xres;
	gdl::ScreenYres = yres;
	gdl::ScreenCenterX = xres/2;
	gdl::ScreenCenterY = yres/2;

	guOrtho(gdl::wii::ProjMtx, -0.325f, (f32)yres - 0.325f, -0.325f, (f32)xres - 0.325f, 0, 16);
	GX_LoadProjectionMtx(gdl::wii::ProjMtx, GX_ORTHOGRAPHIC);

}

void gdl::Set2DMode(float farClip) {

	guOrtho(gdl::wii::ProjMtx, -0.325f, (f32)gdl::ScreenYres - 0.325f, -0.325f, (f32)gdl::ScreenXres - 0.325f, -farClip, farClip);
	GX_LoadProjectionMtx(gdl::wii::ProjMtx, GX_ORTHOGRAPHIC);

	guMtxIdentity(gdl::wii::ModelMtx);
	GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);
	GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);

	GX_ClearVtxDesc();

}

void gdl::Set3DMode(float farClip) {

	guPerspective(gdl::wii::ProjMtx, tan(90.0f*PI/360.0f)*atan((f32)gdl::ScreenYres/gdl::ScreenXres)*360.0f/PI, (f32)gdl::ScreenXres/gdl::ScreenYres, 0.1f, farClip);
	GX_LoadProjectionMtx(gdl::wii::ProjMtx, GX_PERSPECTIVE);

	guMtxIdentity(gdl::wii::ModelMtx);
	GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

	GX_ClearVtxDesc();

}

void gdl::SetClearColor(u_char red, u_char grn, u_char blu, u_char alp) {

	GXColor clearCol = { red, grn, blu, alp };
	GX_SetCopyClear(clearCol, GX_MAX_Z24);

}

void gdl::SetClipBox(short x1, short y1, short x2, short y2) {

	short sx1=x1,sy1=y1;
    short sx2=x2,sy2=y2;

	if (sx1<0) sx1=0;
	if (sx1>gdl::ScreenXres-1) sx1=gdl::ScreenXres-1;
	if (sy1<0) sy1=0;
	if (sy1>gdl::ScreenYres-1) sy1=gdl::ScreenYres-1;

	if (sx2<0) sx2=0;
	if (sx2>gdl::ScreenXres-1) sx2=gdl::ScreenXres-1;
	if (sy2<0) sy2=0;
	if (sy2>gdl::ScreenYres-1) sy2=gdl::ScreenYres-1;

	short clipXdim = (sx2-sx1)+1;
    short clipYdim = (sy2-sy1)+1;

	sx1 *= ((float)gdl::wii::rmode->fbWidth+0.6f)/(float)gdl::ScreenXres;
	sx2 *= ((float)gdl::wii::rmode->efbHeight)/(float)gdl::ScreenYres;

	clipXdim *= (float)gdl::wii::rmode->fbWidth/((float)gdl::ScreenXres-0.05f);
	clipYdim *= (float)gdl::wii::rmode->efbHeight/(float)gdl::ScreenYres;

	GX_SetScissor(sx1, sy1, clipXdim, clipYdim);

}

void gdl::ResetClipBox() {

	GX_SetScissor(0, 0, gdl::wii::rmode->fbWidth, gdl::wii::rmode->efbHeight);

}

void gdl::PrepDisplay() {

	if (gdl::wii::PollKeyboardCallback != NULL)
		gdl::wii::PollKeyboardCallback();

	// Get the current CPU cycles passed for delta calculation when gdl::Display() is called
	_timer_lastCycles = _timer_getCpuCycles();

	GX_SetViewport(0, 0, gdl::wii::rmode->fbWidth, gdl::wii::rmode->efbHeight, 0, 1);
	GX_SetScissor(0, 0, gdl::wii::rmode->fbWidth, gdl::wii::rmode->efbHeight);

	GX_InvVtxCache();
	GX_InvalidateTexAll();

	guMtxIdentity(gdl::wii::ModelMtx);
	GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);
	GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);

	gdl::Set2DMode(100);

}

void gdl::Display() {

	// Fade-out sequence for power/reset button handling
	if (wii::DoExitSequence) {

		wii::DoExitCount += 1.f*gdl::Delta;
		GX_SetScissor(0, 0, gdl::wii::rmode->fbWidth, gdl::wii::rmode->efbHeight);

		if (ceil(wii::DoExitCount)<30)
			gdl::DrawBoxF(0, 0, gdl::ScreenXres-1, gdl::ScreenYres-1, RGBA(0, 0, 0, 255*(ceil(wii::DoExitCount)/30.f)));
		else
			gdl::DrawBoxF(0, 0, gdl::ScreenXres-1, gdl::ScreenYres-1, RGBA(0, 0, 0, 255));

	}


	// Copy framebuffer to upscale buffer (if upscale buffer is not NULL)
	if (gdl::wii::UpscaleBuff != NULL) {

		GX_SetTexCopySrc(0, 0, gdl::ScreenXres, gdl::ScreenYres);
		GX_SetTexCopyDst(gdl::ScreenXres, gdl::ScreenYres, GX_TF_RGBA8, GX_FALSE);
		GX_CopyTex(gdl::wii::UpscaleBuff, GX_TRUE);

		// Set appropriate parameters for upscaling the framebuffer
		GX_SetViewport(0, 0, gdl::wii::rmode->fbWidth, gdl::wii::rmode->efbHeight, 0, 1);
		GX_SetScissor(0, 0, gdl::wii::rmode->fbWidth, gdl::wii::rmode->efbHeight);

		guOrtho(gdl::wii::ProjMtx, -0.325f, (f32)gdl::wii::rmode->efbHeight - 0.325f, -0.325f, (f32)gdl::wii::rmode->fbWidth - 0.325f, -100, 100);
		GX_LoadProjectionMtx(gdl::wii::ProjMtx, GX_ORTHOGRAPHIC);

		guMtxIdentity(gdl::wii::ModelMtx);
		GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);

		// Draw the upscaled framebuffer
		GX_LoadTexObj(&gdl::wii::UpscaleTexObj, GX_TEXMAP0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

			GX_Position2s16(0, 0);
			GX_Color4u8(255, 255, 255, 255);
			GX_TexCoord2f32(0.f, 0.f);

			GX_Position2s16(gdl::wii::rmode->fbWidth, 0);
			GX_Color4u8(255, 255, 255, 255);
			GX_TexCoord2f32(1.f, 0.f);

			GX_Position2s16(gdl::wii::rmode->fbWidth, gdl::wii::rmode->efbHeight);
			GX_Color4u8(255, 255, 255, 255);
			GX_TexCoord2f32(1.f, 1.f);

			GX_Position2s16(0, gdl::wii::rmode->efbHeight);
			GX_Color4u8(255, 255, 255, 255);
			GX_TexCoord2f32(0.f, 1.f);

		GX_End();

	}


	// If not in anti-aliased mode, finish drawing and take screenshot from EFB
	if ((gdl::wii::ScreenCapIssued) && (!gdl::wii::rmode->aa)) {

		GX_DrawDone();
		if (gdl::wii::ScreenCapFunc != NULL)
			gdl::wii::ScreenCapFunc(gdl::wii::ScreenCapFileName);

		gdl::wii::ScreenCapIssued = false;
		free(gdl::wii::ScreenCapFileName);

		_timer_lastCycles = _timer_getCpuCycles();

	}


	// Complete all rendering
	gdl::wii::ActiveFB ^= 1;
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(gdl::wii::FB[gdl::wii::ActiveFB], GX_TRUE);

	_timer_CPUcycles = _timer_getCpuCycles();

	GX_DrawDone();

	_timer_GPUcycles = _timer_getCpuCycles();


	// If in anti-aliased mode, capture from the external framebuffer
	if ((gdl::wii::ScreenCapIssued) && (gdl::wii::rmode->aa)) {

		if (gdl::wii::ScreenCapFunc != NULL)
			gdl::wii::ScreenCapFunc(gdl::wii::ScreenCapFileName);

		gdl::wii::ScreenCapIssued = false;
		free(gdl::wii::ScreenCapFileName);

		_timer_lastCycles = _timer_getCpuCycles();

	}


	if (gdl::ConsoleActive) {
		VIDEO_Configure(gdl::wii::rmode);
		gdl::ConsoleActive = false;
	}

	// Switch framebuffers
	VIDEO_SetNextFramebuffer(gdl::wii::FB[gdl::wii::ActiveFB]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();


	// Power-off or exit to menu handling
	if (wii::DoExitSequence) {

		if (ceil(wii::DoExitCount) >= 30) {

			if (wii::ExitType == 0) {	// Power-off

				VIDEO_SetBlack(TRUE);
				VIDEO_Flush();
				VIDEO_WaitVSync();
				VIDEO_WaitVSync();
				SYS_ResetSystem(SYS_POWEROFF, 0, 0);

			} else { // Soft-reset (exit to loader)

				VIDEO_SetBlack(TRUE);
				VIDEO_Flush();
				VIDEO_WaitVSync();
				VIDEO_WaitVSync();
				exit(0);

			}

		}


	}


	// Calculate delta based on the amount of cycles that have passed since the start of rendering when gdl::PrepDisplay() got called
	u64 timer_cycles = (u_int)((_timer_getCpuCycles()-_timer_lastCycles))/100;
	gdl::Delta = ((float)timer_cycles)/10110.f;

	timer_cycles = (u_int)(_timer_CPUcycles-_timer_lastCycles)/100;
	gdl::wii::CPUdelta = ((float)timer_cycles)/10110.f;

	timer_cycles = (u_int)((_timer_GPUcycles-_timer_lastCycles))/100;
	gdl::wii::GPUdelta = (((float)timer_cycles)/10110.f)-gdl::wii::CPUdelta;

}

gdl::ErrorCallback gdl::SetErrorCallback(gdl::ErrorCallback func) {

	gdl::ErrorCallback oldFunc = gdl::ErrorCB;

	gdl::ErrorCB = func;

	return(oldFunc);

}

void gdl::wii::Exit() {

	if (gdl::wii::DoExitSequence == false) {

		gdl::wii::DoExitSequence = true;
		gdl::wii::ExitType = 1;

	}

}

void gdl::wii::RevBytes(void *var, int size) {

	u8 temp[size];

	memcpy(temp, var, size);
	for(short i=0; i<size; i++) {
		((u8*)var)[i] = temp[(size-1)-i];
	}

}

bool gdl::wii::IsComponentCableUsed() {

    return(VIDEO_HaveComponentCable());

}

bool gdl::wii::IsWidescreen() {

	return(gdl::wii::LowResolutionMode);

}

bool gdl::wii::IsProgressive() {

	return(gdl::wii::ProgressiveMode);

}

bool gdl::wii::IsLowResMode() {

	return(gdl::wii::LowResolutionMode);

}

void gdl::wii::SetNonInterlaced(bool nonInterlaced) {

    // Make this function only work in low-resolution, non-progressive mode
    if ((gdl::wii::LowResolutionMode == false) || (gdl::wii::ProgressiveMode))
        return;


    // Set video mode register accordingly
    if (nonInterlaced)
        gdl::wii::rmode->viTVMode=VI_TVMODE(gdl::wii::rmode->viTVMode>>2, VI_NON_INTERLACE);
    else
        gdl::wii::rmode->viTVMode=VI_TVMODE(gdl::wii::rmode->viTVMode>>2, VI_INTERLACE);


    // Apply new video settings
    VIDEO_Configure(gdl::wii::rmode);
    VIDEO_Flush();
	VIDEO_WaitVSync();
	if (gdl::wii::rmode->viTVMode&VI_NON_INTERLACE)
        VIDEO_WaitVSync();

}


#endif // _GDL_WII_MAIN
