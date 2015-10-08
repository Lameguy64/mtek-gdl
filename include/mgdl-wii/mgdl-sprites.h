/*! \file mgdl-sprites.h
 *  \brief Sprite system module header
 */

#ifndef _GDL_WII_SPRITES_H
#define _GDL_WII_SPRITES_H


namespace gdl {


/*!	\addtogroup spriteModule Sprite System Module
 *	\brief Class module for loading and drawing sprite sets.
 *	@{
 */

//! Sprite entry structure
typedef struct {
    short			w;			//!< Width of sprite in pixels
    short 			h;			//!< Height of sprite in pixels
    short			cx;			//!< X pivot offset of sprite
    short			cy;			//!< Y pivot offset of sprite
    wii::VERT2s16	*vList;		//!< Vertex array pointer
    wii::TEX2f32	*tList;		//!< Texcoord array pointer
    GXTexObj		*texObj;	//!< Pointer to associated texture object
} Sprite;


//! Sprite set handling class
/*!
 *	Instead of using several gdl::Image objects for handling sprite images, it is recommended to use this gdl::SpriteSet
 *	class as it allows you to store sprites as sprite sheets which is more efficient than having multiple texture
 *	objects for each individual sprite. It also allows you to reference each sprite by index making sprite
 *	animations flexible to do.
 */
class SpriteSet {

    Sprite	*spriteList;
    Image	**sheetList;
    short	numSprites;
    short	numSheets;

public:


    //! Constructor.
    SpriteSet();


    //! Deconstructor.
    /*!
     *	\details If a sprite set and its associated sheets have already been loaded, all of it is automatically
     *		freed once the object has been deconstructed.
     */
    virtual ~SpriteSet();


    //! Loads a sprite set file.
    /*!
     *	\details Loads a TR-Engine format sprite map file and its associated sprite sheets (sprite sheets must
	 *		be in PNG format in this port of the library).
     *
     *	\param[in]	*fileName	File name of TSM file to load.
     *	\param[in]	*sheetsDir	Path to where the associated sprite sheets are stored.
     *	\param[in]	filterMode	Filtering mode (see gdl::TextureFilterModes).
     *	\param[in]	format		Texture format to load sprite sheets as (see gdl::TextureFormatModes).
     *
     *	\return Non-zero if the sprite set and its associated sprite sheets were loaded successfully. Otherwise
     *		an error occurred (detailed error is printed in console mode).
     */
    bool	LoadSprites(const char *fileName, const char *sheetsDir, u_int filterMode, u_int format);


    //! Returns the number of sprites in the currently loaded sprite set.
    /*!
     *	\return The number of sprites in the sprite set, 0 if a sprite set is not loaded.
     */
    short	NumSprites();


    //! Returns a pointer to a sprite entry object.
    /*!
     *	\param[in]	index	Sprite index.
     *
     *	\return Pointer to a sprite entry object.
     */
    const Sprite *SpriteInfo(short index);


	//! Draws a sprite onto the screen.
	/*!
	 *	\details Draws the image with color translation and optional rotation and scaling. This function
	 *		must only be called between a gdl::PrepDisplay() and gdl::Display() pair. If no sprite set is
	 *		loaded in this class, nothing is drawn.
	 *
	 *	\param[in]	x		X coordinate of image.
	 *	\param[in]	y		Y coordinate of image.
	 *	\param[in]	index	Index of sprite to draw.
	 *	\param[in]	col		Color of image (use gdl::Color::White or full white or RGBA() for normal colors).
	 *	\param[in]	cx		X offset of image pivot (you can also use one of the gdl::AlignmentModes).
	 *	\param[in]	cy		Y offset of image pivot (you can also use one of the gdl::AlignmentModes).
	 *	\param[in]	scale	Scale of image (1.0f is original size).
	 *	\param[in]	angle	Rotation angle in degrees.
	 */
	void	Put(short x, short y, int index, u_int col = gdl::Color::White, short cx = 0, short cy = 0, float scale = 1.f, float angle = 0.f);



    //! Draws a sprite as a stretchable rectangle.
    /*!
     *	\details Draws the image as a fully stretchable rectangle with optional color translation. This function
	 *		must only be called between a gdl::PrepDisplay() and gdl::Display() pair and if no image is loaded in
	 *		this class, nothing is drawn.
	 *
     *	\param[in]	x1		Upper-left X coordinate of image.
     *	\param[in]	y1		Upper-left Y coordinate of image.
     *	\param[in]	x2		Lower-right X coordinate of image.
     *	\param[in]	y2		Lower-right Y coordinate of image.
     *	\param[in]	index	Index of sprite to draw.
     *	\param[in]	col		Color of sprite (use gdl::Color::White for normal colors).
     */
    void	PutS(short x1, short y1, short x2, short y2, short index, u_int col = gdl::Color::White);


};

/*!	@} */


}

#endif // _GDL_WII_SPRITES_H
