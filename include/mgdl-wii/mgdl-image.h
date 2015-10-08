/*! \file mgdl-image.h
 *  \brief Image handling module header
 */

#ifndef _GDL_WII_IMAGE_H
#define _GDL_WII_IMAGE_H

#include <png.h>

#include "mgdl-wii/mgdl-texture.h"


namespace gdl {


/*!	\addtogroup imageModule Image Handling Module
 *	\brief Image loading and saving functions and classes.
 *	@{
 */

//! Saves a screenshot.
/*!
 *	\details Saves a screenshot and saves it as a 24-bit PNG image with maximum compression. Because of that,
 *		encoding the image will take about roughly half a second.
 *
 *	\note As of v0.80 and onwards, this function can be called anywhere but the actual screen capture will
 *		take place once gdl::Display() is called. This is so that the screenshot will always be the same as
 *		what is seen on the TV and it also allows taking screenshots while in anti-aliased mode.
 *	\note When taking a screenshot while in anamorphic widescreen mode, the image will not be stretched
 *		horizontally for correct aspect ratio so it had to be manually stretched with an image editing program.
 *
 *	\param[in]	fileName	File name (may contain a path) of the output image file (must end with .png).
 */
void SaveScreen(const char *fileName);


//! Image handling class
/*!
 *	This class is used for loading and drawing images with ease similar to SFML's sf::Image class. It also
 *	contains a gdl::Texture object for storing the actual image data and can be used for custom graphics routines.
 *
 *	\note It is not advisable to use this class for handling several individual sprite images. Instead, Use
 *		the gdl::SpriteSet class.
 */
class Image {

	short xSize,ySize;
	float u,v;
	void *vList;
	void *tList;

	void _PrepareImage(short xRes, short yRes);
	bool _ProcessImage(png_structp png_ptr, u_int minFilt, u_int magFilt, u_int format, bool mipmapped, short maxMips);

public:


	//! gdl::Texture object containing the actual texture data of the image.
	gdl::Texture Texture;


	//! Constructor.
	Image();


	//! Deconstructor.
	virtual	~Image();


	//! Creates an image.
	/*!
	 *	\details This function allocates texture and vertex array buffers for handling and drawing an image. The
	 *		class contains a gdl::Texture object for storing the actual image data and you can use it to alter the
	 *		texture data of the image.
	 *
	 *  \note If the image has already been created before, it will reallocate the texture and vertex buffers of
	 *		the image. However, it is not advisable to do this too frequently.
	 *  \note This function does not need to be called when loading an image file with gdl::Image::LoadImage() as
	 *		that function already creates the image for you.
	 *
	 *	\param[in]	xRes		X size of texture in pixels.
	 *	\param[in]	yRes		Y size of texture in pixels.
	 *	\param[in]	filterMode	Image filter mode (see gdl::TextureFilterModes).
	 *	\param[in]	format		Texture format (see gdl::TextureFormatModes).
	 *
	 *	\returns Non-zero if the image had been allocated successfully otherwise there isn't enough memory to allocate it.
	 */
	bool Create(short xRes, short yRes, u_int filterMode, u_int format = gdl::RGBA8);


	//! Creates a mipmapped image.
	/*!
	 *	\details Similar to gdl::Image::Create() but it creates a mipmapped image which improves rendering quality
	 *		(and in some cases, performance) when the image gets downscaled.
	 *
	 *	\note See gdl::Texture::CreateMipmapped() for details.
	 *  \note If the image has already been created before, it will reallocate the texture and vertex buffers of the image.
	 *		However, it is not advisable to do this too frequently.
     *  \note This function does not need to be called when loading an image file with gdl::Image::LoadImageMipmapped() as
	 *      that function already creates an image for you.
	 *
	 *	\param[in]  xSize			X size of texture in pixels.
	 *	\param[in]  ySize			Y size of texture in pixels.
	 *	\param[in]	minFilt			Far filter mode (see gdl::TextureFilterModes).
	 *	\param[in]	magFilt			Near filter mode (see gdl::TextureFilterModes).
	 *	\param[in]	maxMipmaps      Maximum number of mipmap levels to generate.
	 *	\param[in]	format			Texture format (see gdl::TextureFormatModes).
	 *
	 *	\returns Non-zero if the image had been allocated successfully otherwise there isn't enough memory to create it.
	 */
	bool CreateMipmapped(short xSize, short ySize, u_int minFilt, u_int magFilt, short maxMipmaps, u_int format = gdl::RGBA8);


	//! Loads an image.
	/*!
	 *	\details Loads an image file, internally creates texture and image buffers and then converts the image data into a
	 *		texture. This version of the library only supports PNG image files that are in grayscale, 8-bit palletized, RGB,
	 *		and RGBA color formats as well as TPL texture files containing a single texture. There will be no support for
	 *		interlaced PNG images.
	 *
	 *      It is advisable that you use an appropriate texture format if you wish to save as much MEM1 memory as possible
	 *		such as using gdl::RGB5A3 for images that have simple transparency (like pixel art sprites) and gdl::RGB565 for
	 *		images that have no alpha channel. Using such texture formats may result in slight color quality loss however so
	 *		use it in situations where color definition is not so important.
	 *
	 *	\note When a TPL texture file is being loaded, the filterMode and format parameters are ignored.
	 *	\note This function will allocate temporary buffers used for converting the image data into an appropriate texture
	 *		format. These buffers are allocated in MEM2 using malloc2() to reduce memory fragmentation in MEM1 when loading
	 *		a lot of individual images with varying resolutions.
	 *
	 *	\param[in]	fileName	File name of image file to load.
	 *	\param[in]	filterMode	Image filter mode (see gdl::TextureFilterModes).
	 *	\param[in]	format		Texture format (see gdl::TextureFormatModes).
	 *
	 *	\returns Non-zero if the image was successfully loaded otherwise an error occurred.
	 */
	bool LoadImage(const char *fileName, u_int filterMode, u_int format = gdl::RGBA8);


    //! Loads an image with mipmap generation.
    /*!
     *  \details Similar to gdl::Image::LoadImage() but it also generates mipmaps of the image.
     *
     *      With mipmapped images, aliasing issues will be eliminated when you scale down an image from its original
     *		resolution. Since mipmapped images take up a bit more memory than a non-mipmapped image, use it wisely for
     *		graphics that scale down tremendously or textures of environments.
     *
     *  \note Unlike gdl::Image::LoadImage(), this function will take slightly longer to load an image and takes up more
     *		MEM2 memory temporarily due to on-the-fly mipmap generation.
     *  \note See gdl::Texture::CreateMipmapped() for details about mipmaps.
     *
     *  \param[in]  fileName    File name of image to load.
     *  \param[in]  minFilt     Filter mode when image is drawn smaller than original size (see gdl::TextureFilterModes).
     *  \param[in]  magFilt     Filter mode when image is drawn bigger than original size (see gdl::TextureFilterModes).
     *  \param[in]  maxMipmaps  The maximum number of mipmaps to generate (9 is the maximum value).
     *	\param[in]	format		Texture format (see gdl::TextureFormatModes).
     *
     *  \returns Non-zero if the image and mipmaps were successfully loaded and generated, otherwise an error occured
     *      (detailed error is printed in console mode).
     */
	bool LoadImageMipmapped(const char* fileName, u_int minFilt, u_int magFilt, short maxMipmaps, u_int format = gdl::RGBA8);


	//! Loads an image from a memory block.
	/*!
	 *  \details Similar to gdl::LoadImage() but this function loads an image file from a memory address. This is most
	 *		useful when making standalone DOL executables where image data is embedded into the program.
	 *
	 *	\note Unlike gdl::LoadImage() which supports TPL texture files, this function only supports PNG images.
	 *
	 *  \param[in]	imagePtr	Pointer to a memory block containing an image file.
	 *	\param[in]	filterMode	Filtering mode (see gdl::TextureFilterModes).
	 *	\param[in]	format		Texture format (see gdl::TextureFormatModes).
	 *
	 *	\returns Non-zero if the image was successfully loaded, otherwise an error occured (detailed error is printed
	 *		in console mode).
	 */
	bool LoadImageMem(void *imagePtr, u_int filterMode, u_int format);


	//! Returns the X size of the image in pixels.
	/*!
	 *	\returns X size of image in pixels.
	 */
	short Xsize();


	//! Returns the Y size of the image in pixels.
	/*!
	 *	\returns Y size of image in pixels.
	 */
	short Ysize();


	//! Draws the image onto the screen with more options.
	/*!
	 *	\details Draws the image with color translation and optional rotation and scaling. This function
	 *		must only be called between a gdl::PrepDisplay() and gdl::Display() pair and if no image is loaded in
	 *		this class, nothing is drawn.
	 *
	 *	\param[in]	x		X coordinate of image.
	 *	\param[in]	y		Y coordinate of image.
	 *	\param[in]	col		Color of image (use gdl::Color::White or full white or RGBA() for normal colors).
	 *	\param[in]	cx		X offset of image pivot (you can also use one of the gdl::AlignmentModes).
	 *	\param[in]	cy		Y offset of image pivot (you can also use one of the gdl::AlignmentModes).
	 *	\param[in]	scale	Scale of image (1.0f is original size).
	 *	\param[in]	angle	Rotation angle in degrees.
	 */
	void Put(short x, short y, u_int col = gdl::Color::White, short cx = 0, short cy = 0, float scale = 1.f, float angle = 0.f);


	//! Draws an image as a stretchable rectangle.
	/*!
	 *	\details Draws the image as a fully stretchable rectangle with optional color translation. This function
	 *		must only be called between a gdl::PrepDisplay() and gdl::Display() pair and if no image is loaded in
	 *		this class, nothing is drawn.
	 *
	 *	\param[in]	x1	Upper-left X coordinate of image.
	 *	\param[in]	y1	Upper-left Y coordinate of image.
	 *	\param[in]	x2	Lower-right X coordinate of image.
	 *	\param[in]	y2	Lower-right Y coordinate of image.
	 *	\param[in]	col	Color of image (use gdl::Color::White or full white with RGBA() for normal colors).
	 */
	void PutS(short x1, short y1, short x2, short y2, u_int col = gdl::Color::White);


};

/*!	@} */


}


#endif // _GDL_WII_IMAGE_H
