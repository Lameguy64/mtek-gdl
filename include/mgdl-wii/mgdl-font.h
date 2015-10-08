/*!	\file mgdl-font.h
 *	\brief Font system module header
 */
#ifndef _GDL_WII_FONT_H
#define _GDL_WII_FONT_H


namespace gdl {


/*!	\addtogroup fontModule Font Module
 *	\brief Class module for loading and drawing text fonts.
 *	@{
 */

//! Fixed font handling class
/*!
 *	Used for handling simple fixed-width fonts from a font sheet. A font sheet is essentially just an image with 256
 *	characters arranged in a 16x16 tile grid. The gdl::Font class however supports variable-width characters but is
 *	currently limited to supporting only DirectQB font files.
 */
class FFont {

	GXTexObj	*charTexObj;
	short		cw,ch;
	void		*vList;
	void		*tList;

public:


	//! Class constructor.
	FFont();


	//! Class deconstructor.
	virtual	~FFont();


	//! Binds the font handler class to an gdl::Image object of a font sheet.
	/*!
	 *	\details A font sheet consists of a simple array of 256 characters arranged in a 16x16 character grid. It is
	 *	good enough for simple fonts (such as those found in retro inspired games) but will become problematic with
	 *	dynamic fonts that have variable-width characters.
	 *
	 *	\param[in]	image	gdl::Image object of a font sheet.
	 *	\param[in]	charw	Width of characters in pixels.
	 *	\param[in]	charh	Height of characters in pixels.
	 */
	void BindSheet(gdl::Image& image, short charw, short charh);


	//! Draw text.
	/*!
	 *	\details Draws a string of text using the currently binded font sheet. Because it only accepts a pre-made
	 *	string of characters, it is not as flexible as printf() and gdl::FFont::Printf().
	 *
	 *	\param[in]	*text	Text to draw.
	 *	\param[in]	x		X coordinate of text (can be gdl::Centered to center text).
	 *	\param[in]	y		Y coordinate of text.
	 *	\param[in]	scale	Scale factor (1.0 is original size).
	 *	\param[in]	col		Color value (see gdl::Color::ColorValues or \ref colorMacros).
	 */
	void DrawText(const char *text, short x, short y, float scale, u32 col);


	//! Draw text with a printf-like syntax.
	/*!
	 *	\details Similar to gdl::FFont::DrawText() but is much more flexible as it accepts printf-style specifiers
	 *	and formatting parameters.
	 *
	 *	\note The overall length of the string must be no longer than 255 characters.
	 *
	 *	\param[in]	x		X coordinate of text (can be gdl::Centered to center text).
	 *	\param[in]	y		Y coordinate of text.
	 *	\param[in]	scale	Scale factor (1.0 is original size).
	 *	\param[in]	col		Color of text (see gdl::Color::ColorValues or \ref colorMacros).
	 *	\param[in]	*format	Text to draw supporting printf specifiers and formatting parameters.
	 *	\param[in]	...		Print arguments.
	 */
	void Printf(short x, short y, float scale, u32 col, const char* format, ... );


};


//! Font handling class
/*!
 *	Unlike gdl::FFont, this font class supports variable-width characters but is currently limited to supporting
 *	DirectQB format fonts.
 */
class Font {

	char charWidth[256];
	char charHeight;

	void *vList;
	void *tList;

	gdl::Texture charSheet;

public:


	//! Class constructor.
	Font();


	//! Class deconstructor.
	virtual ~Font();


	//! Loads a font file.
	/*!
	 *	\details Currently, this function only supports DirectQB format font files. Support for TTF files may
	 *	most likely never be implemented. Tools for converting TTF fonts to a sheet-style font format may
	 *	most likely be implemented in the future.
	 *
	 *	\param[in]	*fileName	File name of the font file to load.
	 *
	 *	\returns Non-zero if the font file was successfully loaded, otherwise an error occurred.
	 */
	bool LoadFont(const char *fileName);


	//! Loads a font from a memory buffer.
	/*!
	 *	\param[in]	*fontData	Pointer to font file data in memory.
	 *
	 *	\returns Non-zero if the font file was successfully loaded, otherwise an error occurred.
	 */
	 bool LoadFontMem(u_char *fontData);


	//! Calculates the length of a string in pixels relative to the loaded font.
	/*!
	 *	\param[in]	*text	String to calculate the length of.
	 *
	 *	\returns Length of the string in pixels, zero if no font was loaded or the string is empty.
	 */
	int CalcStrLen(const char *text);

	//! Overload alternative to CalcStrLen().
	/*!
	 *	\param[in]	*text	String to calculate the length of.
	 *	\param[in]	len		Number of characters to calculate the length of.
	 *
	 *	\returns Length of the string in pixels, zero if no font was loaded or the string is blank.
	 */
	int CalcStrLen(const char *text, int len);


	//! Draw text.
	/*!
	 *	\details Draws a string of text using the loaded font. Because it only accepts a pre-made string of
	 *	characters, it is not as flexible as printf() and gdl::Font::Printf()
	 *
	 *	\param[in]	*text	Text to draw.
	 *	\param[in]	x		X coordinate of text (can be gdl::Centered to center text).
	 *	\param[in]	y		Y coordinate of text.
	 *	\param[in]	scale	Scale factor (1.0 is original size).
	 *	\param[in]	col		Color of text (see gdl::Color::ColorValues or \ref colorMacros).
	 */
	void DrawText(const char *text, short x, short y, float scale, u_int col);


	//! Draw text with a printf-like syntax.
	/*!
	 *	\details Similar to gdl::Font::DrawText() but is much more flexible as it accepts printf-style specifiers
	 *	and formatting parameters.
	 *
	 *	\note The overall length of the string must be no longer than 256 characters.
	 *
	 *	\param[in]	x		X coordinate of text (can be gdl::Centered to center text).
	 *	\param[in]	y		Y coordinate of text.
	 *	\param[in]	scale	Scale factor (1.0 is original size).
	 *	\param[in]	col		Color of text (see gdl::Color::ColorValues or \ref colorMacros).
	 *	\param[in]	*format	Text to draw supporting printf specifiers and formatting parameters.
	 *	\param[in]	...		Print arguments.
	 */
	void Printf(short x, short y, float scale, u32 col, const char* format, ... );


};

/*!	@} */


}


#endif // _GDL_WII_FONT_H
