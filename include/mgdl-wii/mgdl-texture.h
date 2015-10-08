/*! \file mgdl-texture.h
 *	\brief Texture handling module header
 */

#ifndef _GDL_WII_TEXTURE_H
#define _GDL_WII_TEXTURE_H


namespace gdl {


/*! \addtogroup textureModule Texture Handling Module
 *	\brief Class module for handling and converting textures.
 *	@{
 */


 /*! Input color format options
 *
 *	\details Options for the gdl::Texture::ConvertRawImage() function when converting raw image data into a texture.
 *		These are not to be confused with gdl::TextureFormatModes which designates GX texture formats.
 */
enum ColorFormats {
    Gray,		//!< 8-bit grayscale (I8).
    GrayAlpha,	//!< 16-bit grayscale with alpha (I8A8).
    RGB,		//!< 24-bit true-color RGB (R8G8B8).
    RGBA,		//!< 32-bit true-color RGBA (R8G8B8A8).
};

/*! Texture wrap modes
 *
 *	\details Wrapping modes for gdl::Texture::SetWrapMode().
 *
 *	\note Wrapping modes other than gdl::Clamp require a texture resolution that is a power of two to work correctly.
 */
enum TextureWrapModes {
    Clamp       = GX_CLAMP,		//!< Clamped (no wrap).
    Repeat      = GX_REPEAT,	//!< Repeated wrap (only applicable to power of two textures).
    Mirror		= GX_MIRROR,	//!< Mirrored wrap (only applicable to power of two textures).
};

/*! Texture filter modes
 *
 *	\details Filter modes for gdl::Texture and gdl::Image classes for creation and loading functions.
 *
 *	\note Mipmapped filtering modes only work if the texture or image was created with gdl::Texture::CreateMipmapped(),
 *		gdl::Image::CreateMipmapped() and gdl::Image::LoadImageMipmapped() and must be specified to the minFilt parameter
 *		of said functions to work.
 */
enum TextureFilterModes {
    Nearest     = GX_NEAR,			//!< Nearest-neighbor interpolation.
    Linear      = GX_LINEAR,		//!< Linear interpolation.
    NR_MM_NR    = GX_NEAR_MIP_NEAR,	//!< Near-mipmap-Near (texture must be mipmapped).
    LN_MM_NR    = GX_LIN_MIP_NEAR,	//!< Linear-mipmap-Near (texture must be mipmapped).
    NR_MM_LN    = GX_NEAR_MIP_LIN,	//!< Near-mipmap-Linear (texture must be mipmapped).
    LN_MM_LN    = GX_LIN_MIP_LIN,	//!< Linear-mipmap-Linear (texture must be mipmapped).
};

/*! Texture format options
 *
 *	\details Texture formats for gdl::Texture and gdl::Image creation and loading functions. Each texture format takes
 *		up memory differently being gdl::I4 being the smallest but could not handle color at all (monochrome) while
 *		gdl::RGBA8 has the best color definition but it takes up a lot of memory and at times is the slowest to render.
 *
 *	\note To save as much MEM1 memory as possible, pick an appropriate texture format for certain images such as
 *		gdl::RGB5A3 for pixel art with transparency while gdl::RGBA8 is for displaying high quality illustrations of
 *		title screens and the like. It all depends on your project for the most part.
 */
enum TextureFormatModes {
    I4			= GX_TF_I4,		//!< 4-bit intensity (alpha) format (2 pixels per byte).
    I8			= GX_TF_I8,		//!< 8-bit intensity (alpha) format (1 pixel per byte).
    IA4			= GX_TF_IA4,	//!< 8-bit intensity with alpha (monochrome) format (1 byte per pixel).
    IA8			= GX_TF_IA8,	//!< 16-bit intensity with alpha (monochrome) format (2 bytes per pixel).
    RGB565		= GX_TF_RGB565,	//!< 16-bit RGB565 pixel format (2 bytes per pixel).
    RGB5A3		= GX_TF_RGB5A3,	//!< 16-bit RGB5A3 pixel format (RGB5 if pixel is opaque, RGB4A3 if pixel is translucent, 2 bytes per pixel).
    RGBA8		= GX_TF_RGBA8 ,	//!< 32-bit RGBA8 pixel format (4 bytes per pixel).
};

/*! Texture allocation modes
 *
 *	\details Used internally by the gdl::Texture class and is used for gdl::wii::SetTexAllocMode().
 *
 */
enum TextureAllocModes {
    MEM1 = 0,	//!< Allocate texture in MEM1.
    MEM2 = 1	//!< Allocate texture in MEM2.
};


//! Texture handling class
/*!
 *	This class is used for conveniently handling textures for storing 2D graphics data such as images, sprites and
 *	textures. These are used by the library in image and sprite set classes and can be used by itself to ease loading
 *	and converting of textures when developing custom graphics engines.
 */
class Texture {

	bool texAlloc;
	bool texMipmapped;
	short texMipLevels;
	short texXsize;
	short texYsize;
	short texFmt;
	u_int texSize;
	void *texData;
	GXTexObj texObj;

	void _ConvertTexture(short xres, short yres, short stride, void *inBuff, void *outBuff);
	bool _ConvertMipmap(short xres, short yres, void *inBuff);

public:


	//! Constructor.
	Texture();


	//! Deconstructor.
	virtual	~Texture();


	//! Creates a texture image.
	/*!
	 *	\details Creates a memory buffer for storing texture data.
	 *
	 *	\note The size of the texture must be a multiple of 4 in both dimensions and must not be greater than 1024
	 *		pixels as that is the maximum texture resolution the GX can handle.
	 *	\note If the texture is already created, it is simply reallocated but it is not recommended to keep
	 *		recreating the texture to prevent too much memory fragmentation.
	 *
	 *	\param[in]	xSize		X size of texture in pixels.
	 *	\param[in]	ySize		Y size of texture in pixels.
	 *	\param[in]	filterMode	Filtering mode (see gdl::TextureFilterModes).
	 *	\param[in]	format		Texture format (see gdl::TextureFormatModes).
	 *
	 *	\returns Non-zero if the texture had been created successfully otherwise there is not enough free memory to create the texture buffer.
	 */
	bool Create(short xSize, short ySize, u_int filterMode, u_int format);


	//! Creates a mipmapped texture image.
	/*!
	 *	\details Unlike gdl::Texture::Create(), this function creates a mipmapped texture buffer which improves
	 *		rendering quality (and in some cases performance) when the texture is drawn smaller than its original
	 *		resolution. It is recommended to use mipmapped textures for large 3D geometry or images	that get scaled
	 *		up or down immensely and you don't want it to look all jagged and pixelated when it gets small enough.
	 *
	 *	\note Unlike non-mipmapped textures which can be of any size as long as its a multiple of 4x4 pixels,
	 *		mipmapped textures however must be a power of two to work correctly.
	 *  \note Mipmapped textures take up roughly 33% more memory (with 9 mipmap levels) than that of a non mipmapped
	 *		texture so you must determine the number of mipmaps necessary to generate such as if the image doesn't get
	 *		downscaled so much, a single mipmap level is good enough for it. Using texture formats that are of a lower
	 *		color depth such as gdl::RGB565 or gdl::RGB5A3 will help as well with a slight loss of color definition.
	 *	\note The smallest mipmap resolution the library will allow is 4x4 pixels (same size as a texture block) so a
	 *		maximum of 9 mipmaps can be generated from a 1024x1024 texture image. If the texture is too small to reach
	 *		the specified number of maximum mipmaps, it will simply generate less mipmaps than that specified.
	 *  \note By default, mipmaped textures are set to have an anisotropy level of 4 and edge LOD computation enabled
	 *		for the best (but most intensive) mipmapping quality. You can change these using GX commands and the
	 *		gdl::Texture::TexObj() function to retrieve the GXTexObj struct of the texture.
	 *
	 *	\param[in]	xSize		X size of texture in pixels.
	 *	\param[in]	ySize		Y size of texture in pixels.
	 *	\param[in]  minFilt		Filtering mode when texture is drawn less than the original resolution (see gdl::TextureFilterModes).
	 *	\param[in]	magFilt		Filtering mode when texture is drawn higher than the original resolution (see gdl::TextureFilterModes).
	 *	\param[in]	maxMipmaps  Number of mipmaps to generate.
	 *	\param[in]	format		Texture format (see gdl::TextureFormatModes).
	 *
	 *	\returns Non-zero if the texture had been allocated successfully otherwise there is not enough free memory to
	 *		create the texture.
	 */
	bool CreateMipmapped(short xSize, short ySize, u_int minFilt, u_int magFilt, short maxMipmaps, u_int format);


	//! Loads a TPL texture file generated by gxtexconv.
	/*!
	 *	\details This function loads a texture from a TPL texture file. TPL actually stands for Texture PaLette as
	 *		the file format can contain multiple textures at once. However, this function only supports single texture
	 *		TPL files currently. If the texture has mipmaps, trilinear mipmap filtering is set.
	 *
	 *	\note The texture does not need to be created first when loading a TPL file.
	 *	\note TPL files are best used for handling textures in special formats which are not supported by the library
	 *		such as DXT1 compressed textures. However, Color Index textures are not supported.
	 *
	 *	\param[in]	fileName	File name of TPL texture file to load.
	 *
	 *	\returns Non-zero if the texture was successfully loaded otherwise an error occured.
	 */
	bool LoadTexture(const char* fileName);


	//! Converts raw image data into a texture.
	/*!
	 *	\details Converts raw image data into an appropriate texture format specified by gdl::Texture::Create() or
	 *		gdl::Texture::CreateMipmapped(). If the texture is created with gdl::Texture::CreateMipmapped(), the function
	 *		will generate mipmaps internally.
	 *
	 *	\note If rawXres and rawYres are lower than the created size of the texture, it is automatically padded out with
	 *		black (or transparent if the texture format has an alpha channel except gdl::RGB5A3).
	 *	\note The texture must already be created with gdl::Texture::Create() or gdl::Texture::CreateMipmapped() first.
	 *  \note If the texture is mipmapped, the function would take a bit more time to finish conversion as it had to
	 *		generate mipmaps.
	 *
	 *	\param[in]	rawXres		X size of raw image data in pixels.
	 *	\param[in]	rawYres		Y size of raw image data in pixels.
	 *	\param[in]	*rawBuff	Pointer to raw image data.
	 *	\param[in]	srcFormat	Format of raw image data (see gdl::ColorFormats).
	 */
	bool ConvertRawImage(short rawXres, short rawYres, void *rawBuff, short srcFormat);


	//! Copies pixel data from the active framebuffer.
	/*!
	 *	\details This function copies pixels from the active framebuffer to be used as a texture which is useful for doing
	 *  	render-to-texture based effects. Take note that the range at which you can capture depends on the current video
	 *		mode that the system is running on and is best to take into account of the actual framebuffer resolution using
	 *		the variables gdl::wii::rmode->fbWidth and gdl::wii::rmode->efbHeight.
	 *
	 *	\note The texture must already be created with gdl::Texture::Create().
	 *	\note This function is GPU intensive which means calling this function too many times will cause a big performance
	 *		drop.
	 *	\note The copy region offset and size is specified in framebuffer pixels which means that the specified resolution
	 *		is always relative to the actual resolution from gdl::wii::rmode and is never affected by the current
	 *		projection matrix.
	 *
	 *	\param[in]	x			X offset of screen to copy from in framebuffer pixels.
	 *	\param[in]	y			Y offset of screen to copy from in framebuffer pixels.
	 *	\param[in]	width		Width of copy region in framebuffer pixels.
	 *	\param[in]	height		Height of copy region in  framebuffer pixels.
	 *	\param[in]	clearScreen	If true, the framebuffer is cleared after copying.
	 */
	void CopyFromScreen(short x, short y, short width, short height, u_char clearScreen);


	//! Deletes the texture (but not the object itself).
	/*!
	 *	\details This function deallocates the texture data block associated to it and resets various parameters of the
	 *		texture to 0.
	 *
	 *	\note It is unnecessary to call this function before deconstructing the object as the texture is automatically
	 *		freed upon deconstruction... It is implemented for certain purposes.
	 */
	void Delete();


	//! Flushes the texture's data block.
	/*!
	 *	\details If gdl::SetAutoFlush() is set to false, you must call this function whenever you modify the contents
	 *		of the texture data block otherwise, the changes will not take effect or it will initially appear as corrupted
	 *		blocks.
	 */
	void Flush();


	//! Sets filter modes to a texture.
	/*!
	 *	\note magFilt can only be either gdl::Nearest or gdl::Linear.
	 *
	 *	\param[in]	minFilt	Filter mode when texture is < 1.0 of original size (see gdl::TextureFilterModes for more filter modes).
	 *	\param[in]	magFilt	Filter mode when texture is >= 1.0 of original size.
	 */
	void SetFilterMode(u_int minFilt, u_int magFilt);


	//! Sets the wrapping strategy of the texture.
	/*!
	 *	\param[in]	wrap_s	Texture wrapping strategy in the S (or U) direction (see gdl::TextureWrapModes).
	 *	\param[in]	wrap_t	Texture wrapping strategy in the T (or V) direction (see gdl::TextureWrapModes).
	 */
	void SetWrapMode(u_int wrap_s, u_int wrap_t);


	//! Pokes a pixel into the texture.
	/*!
	 *	\details Pokes a pixel into a texture useful for procedurally generating textures easily.
	 *
	 *	\note This function is only effective on non-mipmapped and non-specialty format textures. Also, pixels drawn
	 *		outside the texture are simply clipped.
	 *	\note If gdl::SetAutoFlush() is set to true (false by default), this function will perform a lot slower when
	 *		doing a lot of texture changes. For best performance, set gdl::SetAutoFlush() to false and call gdl::Texture::Flush()
	 *		after doing such changes.
	 *	\note If you poke a pixel to an I4/I8 format texture, the color value must only be a range from 0 to 255. If
	 *		the texture is an IA4/IA8, use the IA() color macro.
	 *
	 *	\param[in]	x	X offset of pixel to draw to.
	 *	\param[in]	y	Y offset of pixel to draw to.
	 *	\param[in]	col	Color of pixel (use the RGBA() or IA() macros for this).
	 */
	void PokePixel(short x, short y, u_int col);


	//! Returns the pointer to the texture data block.
	/*!
	 *	\note If you modify the contents of the memory block, you must flush it either with gdl::Texture::Flush() or
	 *		DCFlushRange().
	 *	\note The pointer is always aligned in 32 byte increments.
	 *
	 *	\returns Pointer to texture data (NULL if texture has not yet been created).
	 */
	void *TexAddr();


	//! Returns the size of the texture data in bytes.
	/*!
	 *	\returns Size of texture data in bytes (0 if texture is not yet created).
	 */
	u_int TexSize();


	//! Returns the texture format index of the texture.
	/*!
	 *	\returns Texture format (see gdl::TextureFormatModes).
	 */
	short TexFmt();


	//! Returns a pointer to the GXTexObj object of the texture.
	/*!
	 *	\details Useful if you want to set the texture with <a href="http://libogc.devkitpro.org/gx_8h.html#ad6388b0e4a0f2ffb5daa16a8851fa567"><b>GX_LoadTexObj()</b></a>
	 *		to be used as your current texture object.
	 *
	 *	\returns Pointer to a GXTexObj object, NULL if texture is not yet created.
	 */
	GXTexObj *TexObj();


	//! Returns the X size of the texture in pixels.
	/*!
	 *	\returns X size of texture in pixels, 0 if texture is not yet created.
	 */
	short TXsize();


	//! Returns the Y size of the texture in pixels.
	/*!
	 *	\returns Y size of texture in pixels, 0 if texture is not yet created.
	 */
	short TYsize();


};


//! Pads out a value to make it a power-of-two.
/*!
 *	\param[in]	value	Value to pad into a power-of-two value.
 *
 *	\returns Power-of-two result.
 */
int MakeValuePOT(int value);

/*!	@} */

namespace wii {

	/*! \addtogroup textureModule
	 *	@{
	 */

    //! Texture flusing mode.
    /*!
     *	\details Sets whether or not to flush the texture whenever gdl::Texture::PokePixel() is used. For performance
     *      reasons, it is best to turn it off when doing a lot of texture modifications and then flush it with
     *      gdl::Texture::Flush() right after.
     *
     *	\param[in]	flush	false to disable auto-flushing, true to enable.
     */
    void SetAutoFlush(bool flush);

    //! Sets which memory area the next texture will be allocated in.
    /*!
     *	\details Sets where the next texture to be created will be allocated on either MEM1 or MEM2.
     *
     *	\note Using MEM2 to store texture data will result to a HUGE (I mean 20-50% performance loss) speed penalty
     *      especially with large textures since MEM2 is completely separate from the graphics package so only use
     *      MEM2 for texture storage when absolutely necessary.
     *
     *	\param[in]	allocMode	Allocation mode (see gdl::TextureAllocModes).
     */
    void SetTexAllocMode(bool allocMode);

    /*!	@} */

}

}



#endif // _GDL_WII_TEXTURE_H
