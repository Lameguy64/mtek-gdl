/*! \file mgdl-types.h
 *	\brief Library types, macros, defines and enums header
 */

#ifndef _GDL_WII_TYPES_H
#define _GDL_WII_TYPES_H

#include <gccore.h>


#ifndef GDL_PLATFORM_WII
//!< Macro for checking what port of the library is used
#define GDL_PLATFORM_WII
#endif


#ifndef PI
#define PI      3.141592653589793f	/*!< PI value. */
#endif

#ifndef ROTPI
#define ROTPI   PI/180				/*!< PI value for degree to radian conversion. */
#endif


/*! \addtogroup colorMacros Color Handling Macros
 *	\brief Macros for handling 32-bit color values.
 *	@{
 */

#define RED(c)		(((c)>>24)&0xFF)	/*!< Gets the red component intensity from a 32-bit color value.
										 *	 \param[in] c 32-bit RGBA color value
										 *	 \return Red component of value
										 */

#define GREEN(c)	(((c)>>16)&0xFF)	/*!< Gets the green component intensity from a 32-bit color value.
										 *	 \param[in] c 32-bit RGBA color value
										 *	 \return Green component of value
										 */

#define BLUE(c) 	(((c)>>8)&0xFF)		/*!< Gets the blue component intensity from a 32-bit color value
										 *	 \param[in] c 32-bit RGBA color value
										 *	 \return Blue component of value
										 */

#define ALPHA(c)	((c)		&0xFF)	/*!< Gets the alpha component intensity from a 32-bit color value.
										 *	 \param[in] c 32-bit RGBA color value
										 *	 \return Alpha component of value
										 */

#define RGBA(r,g,b,a)	((u32)((((u32)(r))<<24) | \
						((((u32)(g))&0xFF)<<16) | \
						((((u32)(b))&0xFF)<<8) | \
						(((u32)(a))&0xFF)))
										/*!< Constructs a 32-bit RGBA color value.
										 *	 \param[in] r Red component
										 *	 \param[in] g Green component
										 *	 \param[in] b Blue component
										 *	 \param[in] a Alpha component
										 *	 \return 32-bit color value
										 */

#define IA(i,a) (i&0xff)|((a&0xff)<<8)	/*!< Constructs a 16-bit intensity with alpha value.
										 *	 \param[in] i Intensity component
										 *	 \param[in] a Alpha component
										 *	 \return 16-bit intensity with alpha value
										 */

/*! @} */


//! Library namespace
namespace gdl {



/*! Alignment modes
 *
 *	\details Alignment modes for gdl::Font, gdl::FFont, gdl::Image and gdl::SpriteSet drawing functions.
 *
 *	\note For gdl::Font and gdl::FFont functions, only gdl::Centered is applicable to said classes.
 */
enum AlignmentModes {
    Centered	= 0x7ff0,	//!< Centered.
    PCentered	= 0x7ff1,	//!< Pixel centered (most noticable when image is zoomed in).
    Pivot       = 0x7ff2,	//!< Alligned according to its pivot coordinate (only applicable to gdl::SpriteSet functions).
    CPivot      = 0x7ff3,	//!< Alligned according to its pivot coordinate and pixel centered (only applicable to gdl::SpriteSet functions).
    RJustify	= 0x7ff4	//!< Right justified (also bottom justified when used for the Y pivot axis).
};


//! Color namespace
namespace Color {

	/*!	Color values
	 *
	 *	\details Preset colors for prototyping and convenience reasons.
	 *
	 *	\note For a better method of selecting colors, use the RGBA() macro instead.
	 */
    enum ColorValues { // RRGGBBAA
        Black		= 0x000000ff,	//!< Black.
        White		= 0xffffffff,	//!< White.
        Red			= 0xff0000ff,	//!< Red.
        Green		= 0x00ff00ff,	//!< Green.
        Blue		= 0x0000ffff,	//!< Blue.
        Yellow		= 0xffff00ff,	//!< Yellow.
        Cyan		= 0x00ffffff,	//!< Cyan.
        Magenta		= 0xff00ffff,	//!< Magenta.
        LightRed	= 0xff7f7fff,	//!< Light red.
        LightGreen	= 0x7fff7fff,	//!< Light green.
        LightBlue	= 0x7f7fffff,	//!< Light blue.
    };

}


//! For the error callback
typedef void (*ErrorCallback)(const char *message);

//! For the internal screen image capture function pointer
typedef bool (ImageCapFunc)(const char *fileName);

typedef void (InputPollFunc)(void);


//! Wii specific functions, structs and global variables.
/*!
 *	\note Be wary of this if you plan to make your game or engine cross-platform compatible as these objects/variables
 *	    are not available in the other ports. Use the GDL_PLATFORM_WII macro to detect which port of the library
 *	    was used to compile your project.
 */
namespace wii {


    //! 2D vertex coordinate.
    /*!
     *	A generic 2D short vector struct used to handle 2D vertices in this library.
     */
    typedef struct {
        s16 x;
        s16 y;
    } VERT2s16;


    //! UV texture coordinate.
    /*!
     *	Generic floating-point texture coordinate struct used to handle texcoords in this library.
     */
    typedef struct {
        f32	u;
        f32 v;
    } TEX2f32;


    //! 3D vertex coordinate.
    /*!
     *	Generic 3D float vector object not used by the library for now... Kept for convenience reasons in 3D prototyping.
     */
    typedef struct {
        f32 x;
        f32 y;
        f32 z;
    } VERT3f32;


};


}


#endif // _GDL_WII_TYPES_H
