#include "main.h"
#include "helix/mp3dec.h"
#include "../../common/soundcommon.h"
#include "avi_headers.h"

#ifndef __SOUND7_H__
#define __SOUND7_H__

#define SNDMIXER_BUFFER_SIZE (4 * MAX_NCHAN * MAX_NGRAN * MAX_NSAMP)

typedef struct
{
	// State
	sndState curState;
	sndMixerState mixState;
		
	// Buffer info
	u32 bufSize;
		
	// Sample info
	u32 smpPos, smpRate;
	s32 curTimer, lstTimer;
	int numChannels;
		
	// AVI buffer
	u8* aviBufStart;
	int aviBufSize;
	int aviRemain;
	// Current chunk within the buffer
	u8* mp3ChunkAddr;
	int	mp3ChunkSize;
	// Data within the chunk
	u8* mp3BufAddr;
    int mp3BufRemain;

	MP3FrameInfo mp3FrameInfo;
	HMP3Decoder hMP3Decoder;
		
	s16	*mixBufferL;
	s16	*mixBufferR;
	s16 *musicBuf;
		
	int nMusicBuf, nMusicBufStart;
	int totalUsed;
} tMixerInfo;

// functions
void SoundLoopStep(void);
void SoundSetTimer(int period);
void SoundStopPlayback();
void SoundVolume(u32 volume);
void SoundMix(int smpCount);
void SoundMixCallback(s16* streamL, s16* streamR, u32 smpCount);
void SoundStartMP3(u8* aviBuffer, int aviBufLen, int aviBufPos, int aviRemain);
void SoundPlayMP3(void);
void SoundPauseMP3(void);
void deinterleaveStereo (s16* streamL, s16* streamR, const u32* inBuffer, int smpCount);


#endif
