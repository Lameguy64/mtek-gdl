// Sound class module functions

#ifndef _GDL_WII_SOUND
#define _GDL_WII_SOUND

#if GDL_NO_SOUND == FALSE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <asndlib.h>
#include <oggplayer.h>

#include "mgdl-wii/mgdl-config.h"
#include "mgdl-wii/mgdl-globals.h"
#include "mgdl-wii/mgdl-globals-internal.h"
#include "mgdl-wii/mgdl-main.h"
#include "mgdl-wii/mgdl-sound.h"

gdl::Sound::Sound() {

	format=0;
	freq=0;
	sData=NULL;
	sSize=0;

}

gdl::Sound::~Sound() {

	if (sData != NULL)
		vfree(sData);

}

bool gdl::Sound::LoadSound(const char* fileName) {

	// Open up the wave file
	if (gdl::ConsoleActive)
		printf("gdl: Opening sound file %s... ", fileName);


	FILE *fp;

	if (!(fp = fopen(fileName, "r"))) {
		gdl::CallErrorCallback("Sound file %s not found", fileName);
		return(false);
	}


	// Get header chunk
	struct {
		char	id[4];
		int		size;
		char	format[4];
	} WAV_Chunk;

	fread(&WAV_Chunk, 1, sizeof(WAV_Chunk), fp);

	if (memcmp(&WAV_Chunk.id, "RIFF", 4) || memcmp(&WAV_Chunk.format, "WAVE", 4)) {
		gdl::CallErrorCallback("Sound file %s is not a Microsoft WAVE file", fileName);
		fclose(fp);
		return(false);
	}

	gdl::wii::RevBytes(&WAV_Chunk.size, sizeof(int));


	// Get header chunk
	struct {
		char	id[4];
		int		size;
		short	format;
		short	chan;
		int		freq;
		int		brate;
		short	balign;
		short	bps;
	} WAV_Subchunk1;

	fread(&WAV_Subchunk1, 1, sizeof(WAV_Subchunk1), fp);


	// Convert values we need into BE
	gdl::wii::RevBytes(&WAV_Subchunk1.size, sizeof(int));
	gdl::wii::RevBytes(&WAV_Subchunk1.format, sizeof(short));
	gdl::wii::RevBytes(&WAV_Subchunk1.chan, sizeof(short));
	gdl::wii::RevBytes(&WAV_Subchunk1.freq, sizeof(int));
	gdl::wii::RevBytes(&WAV_Subchunk1.bps, sizeof(short));

	if (memcmp(&WAV_Subchunk1.id, "fmt ", 4)) {
		gdl::CallErrorCallback("Unsupported WAV format variant in %s", fileName);
		fclose(fp);
		return(0);
	}


	// Search and get the data chunk
	struct {
		char	id[4];
		int		size;
	} WAV_Subchunk2;

	while(1) {

		fread(&WAV_Subchunk2, 1, sizeof(WAV_Subchunk2), fp);
		gdl::wii::RevBytes(&WAV_Subchunk2.size, sizeof(int));

		if (memcmp(&WAV_Subchunk2.id, "data", 4)) {
			fseek(fp, WAV_Subchunk2.size, SEEK_CUR);
		} else {
			break;
		}

	}


	// If all is valid, allocate memory for storing the wave data
	if (sData != NULL) vfree(sData);	// If already allocated, deallocate it for reallocation
	sData	= valloc(WAV_Subchunk2.size);
	sSize	= WAV_Subchunk2.size;
	freq	= WAV_Subchunk1.freq;


	// Load the sound data, flush it, and then close
	memset(sData, 0x00, sSize);
	fread(sData, 1, sSize, fp);
	fclose(fp);


	// Set audio format
	if (WAV_Subchunk1.chan == 1) {

		// For mono sounds
		switch(WAV_Subchunk1.bps) {
			case 8:

				format = VOICE_MONO_8BIT_U;
				break;

			case 16:

				format = VOICE_MONO_16BIT_LE;
				break;

		}

	} else {

		u_short buff;

		// For stereo sounds
		switch(WAV_Subchunk1.bps) {
			case 8:

				format = VOICE_STEREO_8BIT_U;

				for(int s=0; s<sSize; s+=2) {	// Swap stereo samples
					buff = ((u_char*)sData)[s];
					((u_char*)sData)[s] = ((u_char*)sData)[s+1];
					((u_char*)sData)[s+1] = buff;
				}

				break;

			case 16:

				format = VOICE_STEREO_16BIT_LE;

				for(int s=0; s<sSize/2; s+=2) {	// Swap stereo samples
					buff = ((u_short*)sData)[s];
					((u_short*)sData)[s] = ((u_short*)sData)[s+1];
					((u_short*)sData)[s+1] = buff;
				}

				break;

		}

	}

	DCFlushRange(sData, sSize);


	if (gdl::ConsoleActive)
		printf("Ok!\n");

	return(true);

}

void gdl::Sound::Delete() {

	if (sData != NULL)
		vfree(sData);

	sData = NULL;

}

void gdl::Sound::Play(float pitch, float volume) {

	// Simple play function

	if (sData == NULL)
		return;

	short tslot = ASND_GetFirstUnusedVoice();
	if (tslot == SND_INVALID)
		return;

	ASND_Pause(0);
	ASND_SetVoice(tslot, format, freq*pitch, 0,
		sData, sSize, volume*((float)gdl::wii::MasterSfxVolume/100.f), volume*((float)gdl::wii::MasterSfxVolume/100.f), NULL);

}

void gdl::Sound::Play2D(float pitch, float volume, float x, float y) {

	if (sData == NULL)
		return;

	short tslot = ASND_GetFirstUnusedVoice();
	if (tslot == SND_INVALID)
		return;

	// Calculate volumes according to the X position of the sound
	float lvol=1,rvol=1;
	if (x < 0) {
		lvol = 1.f;
		rvol = 1.f-((float)abs(x)/(gdl::ScreenXres*0.7f));
	} else if (x > 0) {
		lvol = 1.f-((float)x/(gdl::ScreenXres*0.7f));
		rvol = 1.f;
	}

	ASND_Pause(0);
	ASND_SetVoice(tslot, format, freq*pitch, 0,
		sData, sSize, (volume*((float)gdl::wii::MasterSfxVolume/100.f))*lvol, (volume*((float)gdl::wii::MasterSfxVolume/100.f))*rvol, NULL);

}


// Sound system functions

void gdl::SetMasterVolumes(float musicVol, float soundVol) {

    gdl::wii::MasterMusicVolume  = musicVol;
    gdl::wii::MasterSfxVolume    = soundVol;

	SetVolumeOgg(255*((gdl::wii::UserMusicVolume*(gdl::wii::MasterMusicVolume/100.f))/100.f));

}


// Music functions

void gdl::SetMusicVolume(float volume) {

	gdl::wii::UserMusicVolume = volume;
    SetVolumeOgg(255*((gdl::wii::UserMusicVolume*(gdl::wii::MasterMusicVolume/100.f))/100.f));

}

bool gdl::PlayMusic(const char* fileName, bool loop) {

    if (fileName != NULL) {

		if (PlayOgg(fileName, 0, loop))
			return(false);

		SetVolumeOgg(255*((gdl::wii::UserMusicVolume*(gdl::wii::MasterMusicVolume/100.f))/100.f));
		gdl::wii::LastMusicFile = fileName;	// Save for possible later use

    } else {

		if ((StatusOgg() == OGG_STATUS_ERR) || (StatusOgg() == OGG_STATUS_EOF)) {

			if (PlayOgg(gdl::wii::LastMusicFile, 0, loop))
				return(false);

		} else {

			PauseOgg(0);

		}

	}

	return(true);

}

void gdl::PauseMusic() {

    PauseOgg(1);

}

void gdl::StopMusic() {

    StopOgg();

}

#endif


#endif // _GDL_WII_SOUND
