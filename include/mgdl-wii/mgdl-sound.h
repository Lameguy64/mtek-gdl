/*!	\file mgdl-sound.h
 *	\brief Sound module header
 */
#ifndef _GDL_WII_SOUND_H
#define _GDL_WII_SOUND_H

#include <gccore.h>

#include "mgdl-wii/mgdl-config.h"


#if GDL_NO_SOUND == FALSE

namespace gdl {


/*!	\addtogroup soundModule Sound System Module
 *	\brief Functions and classes for loading and playing back sound and music files
 *	@{
 */


//! Sound handling class
/*!
 *	\details Class object for loading and playing back sound effects with pitch control and stereo panning.
 */
class Sound {

	short	format;
	u_short	freq;
	void	*sData;
	int		sSize;

public:


	//! Constructor.
	Sound();


	//! Deconstructor.
	virtual ~Sound();


	//! Loads a sound file.
	/*!
	 *	\details This function loads a sound file which can then be played with Play() or Play2D().
	 *
	 *	\note This port of the library only supports uncompressed Microsoft WAV sound files with a sample
	 *		format of 8 or 16-bit and a sample frequency of up to 48KHz, Mono or Stereo.
	 *
	 *	\param[in]	*fileName	File name of sound file to load.
	 *
	 *	\returns Non-zero if the sound file was successfully loaded, otherwise an error occured.
	 */
	bool LoadSound(const char* fileName);


	//! Deletes the sound data stored in the object
	/*!
	 *	\details Deallocates the memory buffer containing the loaded sound data.
	 *
	 *  \note It is not necessary to call this function when the object gets deconstructed as the deconstructor already does
	 *      it for you.
	 */
	void Delete();


	//! Plays a sound.
	/*!
	 *  \details Plays a sound with pitch control and its own volume level.
	 *
	 *  \note If no sound file is loaded, nothing will be played.
	 *
	 *	\param[in]	pitch	Pitch of sound (1.0 is normal pitch).
	 *	\param[in]	volume	Volume of sound (100 is full volume).
	 */
	void Play(float pitch, float volume);


	//! Plays a sound with 2D stereo panning.
	/*!
	 *	\param[in]	pitch	Pitch of sound (1.0 is normal pitch).
	 *	\param[in]	volume	Volume of sound (100 is full volume).
	 *	\param[in]	x		X position of sound (relative to screen).
	 *	\param[in]	y		Y position of sound (this parameter does nothing for now).
	 */
	void Play2D(float pitch, float volume, float x, float y);


};


//! Sets master sound volumes.
/*!
 *  \details Changes the master volumes of music playback and sound effects.
 *
 *	\note Changing the music volume will affect the currently playing music. Already playing sound effects however
 *		will not be affected but the next sounds will be.
 *
 *	\param[in]	musicVol	Master music volume.
 *	\param[in]	soundVol	Sound effects volume.
 */
void SetMasterVolumes(float musicVol, float soundVol);


//! Sets the music volume.
/*!
 *	\note Changing the music volume will affect the currently playing music on-the-fly.
 *
 *	\param[in]	volume	Music volume.
 */
void SetMusicVolume(float volume);


//!	Plays a music file.
/*!
 *	\details Plays a music file in the background. Unlike the gdl::Sound class which loads the sound data entirely into
 *	    memory, this one streams audio data off of a music file which is ideal for music tracks as they tend to be lengthy
 *      and take up a bit of memory.
 *
 *	\note To resume a stopped or paused music, use this function but specify NULL as the file name.
 *	\note This port of the library only supports mono or stereo OGG Vorbis audio files. Looping OGG files (logg) and mp3s are
 *	    not supported.
 *
 *	\param[in]	fileName	Name of music file to play.
 *	\param[in]	loop		Loop flag (music will be looped if true).
 */
bool PlayMusic(const char* fileName, bool loop);


//! Pauses the currently playing music.
/*!
 *  \details Pauses the current playing music. To resume playback, use gdl::PlayMusic() but specify NULL as the file name.
 */
void PauseMusic();


//! Stops the currently playing music.
void StopMusic();

/*! @} */


}

#endif


#endif // _GDL_WII_SOUND_H
