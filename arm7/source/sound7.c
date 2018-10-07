#include <nds/timers.h>
#include "errors.h"
#include "sound7.h"
#include "utils.h"

volatile tMixerInfo mixerInfo = {0};

#define DECODE_AHEAD_LENGTH (mixerInfo.bufSize >> 1)

static void SoundNextAviChunk (void) {
	AviChunkHeader aviChunkHeader;

	// Scan forward in AVI buffer for next audio data chunk
	do {
		mixerInfo.mp3ChunkAddr += mixerInfo.mp3ChunkSize;
		mixerInfo.aviRemain -= mixerInfo.mp3ChunkSize;
		ipcSend_AmountUsed(mixerInfo.mp3ChunkSize);
		if (mixerInfo.mp3ChunkAddr >= mixerInfo.aviBufStart + mixerInfo.aviBufSize) {
			mixerInfo.mp3ChunkAddr -= mixerInfo.aviBufSize;
		}
		memcpy (&aviChunkHeader, mixerInfo.mp3ChunkAddr, sizeof (AviChunkHeader));
		// Lists contain chunks, but we're only interested in the chunks inside, so ignore list headers
		if (aviChunkHeader.fourcc == LIST_ID) {
			aviChunkHeader.size = sizeof(AviListHeader)-sizeof(AviChunkHeader);
		}
		if (aviChunkHeader.size > mixerInfo.aviBufSize)
		{
			ipcSend_Error(ERROR_MP3_CHUNK_SIZE);
		}
		mixerInfo.mp3ChunkSize = ((aviChunkHeader.size + 1) & ~1) + sizeof(AviChunkHeader);
		mixerInfo.mp3BufAddr = mixerInfo.mp3ChunkAddr + sizeof(AviChunkHeader);
		mixerInfo.mp3BufRemain = aviChunkHeader.size;
	} while ((aviChunkHeader.fourcc != CHUNK_audio || aviChunkHeader.size == 0) && mixerInfo.aviRemain > 0);

	if (aviChunkHeader.fourcc != CHUNK_audio)
	{
		mixerInfo.mp3BufRemain = 0;
	}
}

static void SoundUsedAmount (int amount) {
	mixerInfo.totalUsed += amount;
	mixerInfo.mp3BufRemain -= amount;
	mixerInfo.mp3BufAddr += amount;
	if (mixerInfo.mp3BufAddr >= mixerInfo.aviBufStart + mixerInfo.aviBufSize) {
		mixerInfo.mp3BufAddr -= mixerInfo.aviBufSize;
	}
	if (mixerInfo.mp3BufRemain <= 0) {
		SoundNextAviChunk();
	}
}

static int SoundMixWorker(s16* streamL, s16* streamR, int smpCount, bool ignoreErrors)
{
	MP3FrameInfo mp3FrameInfo;
	int minSamples, usedBytes;
	int outSample = 0;

	u8* aid_readPtr;
	int aid_bytesLeft;
	int error = 0;

	// Fill buffered data into stream
	minSamples = min(mixerInfo.nMusicBuf, smpCount);
	if(minSamples > 0) {
		// Copy data from musicBuf to stream
		smpCount -= minSamples;

		if (mixerInfo.numChannels == 2) {
			deinterleaveStereo (streamL + outSample, streamR + outSample, ((u32*)mixerInfo.musicBuf) + mixerInfo.nMusicBufStart, minSamples);
		} else {
			memcpy(streamL + outSample, mixerInfo.musicBuf + mixerInfo.nMusicBufStart, minSamples * sizeof(s16));
			memcpy(streamR + outSample, mixerInfo.musicBuf + mixerInfo.nMusicBufStart, minSamples * sizeof(s16));
		}

		outSample += minSamples;

		mixerInfo.nMusicBufStart += minSamples;
		mixerInfo.nMusicBuf -= minSamples;

		if(mixerInfo.nMusicBuf <= 0) {
			mixerInfo.nMusicBufStart = 0;
			mixerInfo.nMusicBuf = 0;
		}
	}

	// Still more data needed?
	while(smpCount > 0) {
		// More data is needed. Decode an MP3 frame to outBuf
		// Find start of next MP3 frame - assume EOF if no sync found
		usedBytes = MP3FindSyncWord(mixerInfo.mp3BufAddr, mixerInfo.mp3BufRemain);
		if(usedBytes < 0) {
			if (mixerInfo.aviRemain <= 0)
			{
				// Reached the end of the AVI file, so can't decode any more.
				// Finish playing what has been decoded then stop.
				SoundState_Finishing();
			}
			else
			{
				SoundState_Stop();
				ipcSend_Error((ERROR_MP3_END_OF_FILE << 24) | -usedBytes);
			}
			break;
		}
		SoundUsedAmount(usedBytes);

		aid_readPtr = mixerInfo.mp3BufAddr;
		aid_bytesLeft = mixerInfo.mp3BufRemain;

		// Decode one MP3 frame to the buffer
		error = MP3Decode(mixerInfo.hMP3Decoder, &aid_readPtr, &aid_bytesLeft, mixerInfo.musicBuf, 0);
		if (error >= 0) {
			// Copy needed data to the stream
			MP3GetLastFrameInfo(mixerInfo.hMP3Decoder, &mp3FrameInfo);
			mixerInfo.numChannels = mp3FrameInfo.nChans;		// Stereo or mono?
			if (mixerInfo.numChannels == 2) {
				mp3FrameInfo.outputSamps /= 2;
			}
			minSamples = min(mp3FrameInfo.outputSamps, smpCount);
			smpCount -= minSamples;

			// Set the rest of the decoded data to be used for the next frame
			mixerInfo.nMusicBufStart = minSamples;
			mixerInfo.nMusicBuf = mp3FrameInfo.outputSamps - minSamples;

			if (mixerInfo.numChannels == 2) {
				deinterleaveStereo (streamL + outSample, streamR + outSample, (u32*)mixerInfo.musicBuf, minSamples);
			} else {
				memcpy(streamL + outSample, mixerInfo.musicBuf, minSamples * sizeof(s16));
				memcpy(streamR + outSample, mixerInfo.musicBuf, minSamples * sizeof(s16));
			}

			outSample += minSamples;
		} else if (!ignoreErrors) {
			/* Error decoding, something wrong with the bit stream?
			 * This will be ignored when seeking the MP3, in case we are in
			 * at a bad position in the middle of the bit stream. */
			SoundState_Stop();
			ipcSend_Error((ERROR_MP3_DECODE << 24) | -error);
			break;
		}

		usedBytes = aid_readPtr - mixerInfo.mp3BufAddr;
		SoundUsedAmount(usedBytes);
	}

	return outSample;
}

// Manages the filling of the ring buffer.
static int SoundMix(int smpCount)
{
	int decodedSamples = 0;
	// Decode data to the ring buffer
	if (mixerInfo.bufSize - mixerInfo.smpPos <= smpCount) {
		decodedSamples = SoundMixWorker(
			&mixerInfo.mixBufferL[mixerInfo.smpPos],
			&mixerInfo.mixBufferR[mixerInfo.smpPos],
			mixerInfo.bufSize - mixerInfo.smpPos,
			false);
		if (decodedSamples > 0)
		{
			decodedSamples += SoundMixWorker(
				&mixerInfo.mixBufferL[0],
				&mixerInfo.mixBufferR[0],
				smpCount - decodedSamples,
				false);
		}
	} else {
		decodedSamples = SoundMixWorker(
			&mixerInfo.mixBufferL[mixerInfo.smpPos],
			&mixerInfo.mixBufferR[mixerInfo.smpPos],
			smpCount, false);
	}

	return decodedSamples;
}

static void SoundSetTimer(int period)
{
	/* Here, TIMER0 counts at the same rate as the SCHANNEL_TIMER registers, and
	 * TIMER1 counts the number of times TIMER0 overflows, that is, the number
	 * of sound samples that have passed. */
	if(period) {
		TIMER1_DATA = 0;
		TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE | TIMER_DIV_1;

		/* TIMER0 runs at twice the frequency as SCHANNEL_TIMERs, so the period
		 * must be doubled. */
		TIMER0_DATA = 0x10000 - (period << 1);
		TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1;
	} else {
		TIMER0_DATA = 0;
		TIMER0_CR = 0;

		TIMER1_DATA = 0;
		TIMER1_CR = 0;
	}
}

/* Atomically change state only if it hasn't been set via IPC (FIFO). */
static void SetMixState(sndMixerState oldState, sndMixerState newState) {
	u16 oldIME = REG_IME;
	REG_IME = 0;

	if (mixerInfo.mixState == oldState)
	{
		mixerInfo.mixState = newState;
	}

	REG_IME = oldIME;
}

void SoundInit()
{
	// Enable Sound
	REG_SOUNDCNT = SOUND_ENABLE;
	REG_MASTER_VOLUME = 127;
	REG_SOUNDBIAS = 0x200;

	// Allocate buffers
	mixerInfo.mixBufferL = malloc (sizeof(s16) * SNDMIXER_BUFFER_SIZE);
	mixerInfo.mixBufferR = malloc (sizeof(s16) * SNDMIXER_BUFFER_SIZE);
	mixerInfo.musicBuf = malloc(sizeof(s16) * SNDMIXER_BUFFER_SIZE);

	// Init sound
	SoundSetTimer(0);

	// Adjust settings
	mixerInfo.bufSize = SNDMIXER_BUFFER_SIZE;
}

void SoundState_Start(u8* aviBuffer, int aviBufLen, int aviBufPos, int aviRemain)
{
	mixerInfo.aviBufStart = aviBuffer;
	mixerInfo.aviBufSize = aviBufLen;
	mixerInfo.mp3ChunkAddr = aviBuffer + aviBufPos;
	mixerInfo.aviRemain = aviRemain;

	mixerInfo.mixState = SNDMIXER_START;
}

void SoundState_Play(void) {
	// Start streaming
	mixerInfo.mixState = SNDMIXER_PLAY;
}

void SoundState_Pause(void) {
	mixerInfo.mixState = SNDMIXER_PAUSE;
}

void SoundState_Finishing(void)
{
	mixerInfo.mixState = SNDMIXER_FINISHING;
}

void SoundState_Stop(void)
{
	mixerInfo.mixState = SNDMIXER_STOP;
}

void SoundState_Volume(u32 volume) {
    if (volume > 127) volume = 127;
    SCHANNEL_CR(0) =  (SCHANNEL_CR(0) & ~SOUND_VOL(127)) | SOUND_VOL(volume);
    SCHANNEL_CR(1) =  (SCHANNEL_CR(1) & ~SOUND_VOL(127)) | SOUND_VOL(volume);
}

void SoundLoopStep()
{
	sndMixerState oldState = mixerInfo.mixState;

	switch(oldState) {
		case SNDMIXER_UNINITIALISED:
		case SNDMIXER_IDLE:
			// Nothing to do
			break;
		case SNDMIXER_START: {
			AviChunkHeader aviChunkHeader;
			MP3FrameInfo mp3FrameInfo;

			// (Re-)Init the helix mp3 decoder
			if (mixerInfo.hMP3Decoder)
			{
				MP3FreeDecoder(mixerInfo.hMP3Decoder);
			}
			mixerInfo.hMP3Decoder = MP3InitDecoder();

			// Uses new settings set by SoundState_Start
			memcpy (&aviChunkHeader, mixerInfo.mp3ChunkAddr, sizeof (AviChunkHeader));
			mixerInfo.mp3ChunkSize = ((aviChunkHeader.size + 1) & ~1) + sizeof(AviChunkHeader);

			mixerInfo.totalUsed = 0;

			if (aviChunkHeader.fourcc == LIST_ID) {
				mixerInfo.mp3ChunkSize = sizeof(AviListHeader);
				SoundNextAviChunk ();
			} else if (aviChunkHeader.fourcc == CHUNK_audio) {
				mixerInfo.mp3BufAddr = mixerInfo.mp3ChunkAddr + sizeof(AviChunkHeader);
				mixerInfo.mp3BufRemain = aviChunkHeader.size;
			} else {
				SoundNextAviChunk ();
			}

			mixerInfo.smpPos = 0;
			mixerInfo.lstTimer = 0;
			mixerInfo.nMusicBuf = 0;
			mixerInfo.nMusicBufStart = 0;
			mixerInfo.numChannels = 1;

			// Decode 1 / 2 of the buffer size
			mixerInfo.smpAvail = SoundMixWorker(
					mixerInfo.mixBufferL, mixerInfo.mixBufferR,
					DECODE_AHEAD_LENGTH, true);
			mixerInfo.smpPos = DECODE_AHEAD_LENGTH;

			// Set the right rate, ...
			MP3GetLastFrameInfo(mixerInfo.hMP3Decoder, &mp3FrameInfo);
			mixerInfo.smpRate = mp3FrameInfo.samprate;
			// From GBATek: SOUNDxTMR.timerval = -(33513982Hz/2)/freq
			mixerInfo.curTimer = (BUS_CLOCK/2) / mixerInfo.smpRate;

			SetMixState(oldState, SNDMIXER_IDLE);
			ipcSend_Ready(mixerInfo.smpRate);
		} break;
		case SNDMIXER_PLAY: {
			// Setup the stream
			// Left channel
			SCHANNEL_CR(0)				= 0;
			SCHANNEL_TIMER(0)			= 0x10000 - mixerInfo.curTimer;
			SCHANNEL_SOURCE(0)			= (u32)mixerInfo.mixBufferL;
			SCHANNEL_REPEAT_POINT(0)	= 0;
			SCHANNEL_LENGTH(0)			= mixerInfo.bufSize >> 1;

			// Right channel
			SCHANNEL_CR(1)				= 0;
			SCHANNEL_TIMER(1)			= 0x10000 - mixerInfo.curTimer;
			SCHANNEL_SOURCE(1)			= (u32)mixerInfo.mixBufferR;
			SCHANNEL_REPEAT_POINT(1)	= 0;
			SCHANNEL_LENGTH(1)			= mixerInfo.bufSize >> 1;

			SoundSetTimer(mixerInfo.curTimer);
			SCHANNEL_CR(0)				= SCHANNEL_ENABLE | SOUND_REPEAT | SOUND_VOL(127) | SOUND_PAN(0) | SOUND_FORMAT_16BIT;
			SCHANNEL_CR(1)				= SCHANNEL_ENABLE | SOUND_REPEAT | SOUND_VOL(127) | SOUND_PAN(127) | SOUND_FORMAT_16BIT;

			SetMixState(oldState, SNDMIXER_PLAYING);
		} break;
		case SNDMIXER_PLAYING: {
			s32 curTimer = TIMER1_DATA;
			s32 smpCount = curTimer - mixerInfo.lstTimer;

			/* Wrap smpCount properly. Equivalent to:
			 * if (smpCount < 0) smpCount += 0x10000; */
			smpCount += (smpCount < 0) << 16;

			ipcSend_Samples(smpCount);
			mixerInfo.smpAvail -= smpCount;

			mixerInfo.lstTimer = curTimer;

			mixerInfo.smpAvail += SoundMix(smpCount);

			mixerInfo.smpPos += smpCount;
			mixerInfo.smpPos %= mixerInfo.bufSize;
		} break;
		case SNDMIXER_PAUSE: {
			s32 curTimer = TIMER1_DATA;
			s32 smpCount = curTimer - mixerInfo.lstTimer;

			// Stop playback
			SCHANNEL_CR(0) = 0;
			SCHANNEL_CR(1) = 0;
			SoundSetTimer(0);

			smpCount += (smpCount < 0) << 16;

			ipcSend_Samples(smpCount);
			mixerInfo.smpAvail -= smpCount;

			mixerInfo.lstTimer = curTimer;

			mixerInfo.smpAvail += SoundMix(smpCount);

			mixerInfo.smpPos += smpCount;
			mixerInfo.smpPos %= mixerInfo.bufSize;

			// Adjust data so that playback starts from the beginning of the buffer
			if (mixerInfo.smpPos >= DECODE_AHEAD_LENGTH) {
				int offset = mixerInfo.smpPos - DECODE_AHEAD_LENGTH;
				memmove (mixerInfo.mixBufferL, mixerInfo.mixBufferL + offset, DECODE_AHEAD_LENGTH);
				memmove (mixerInfo.mixBufferR, mixerInfo.mixBufferR + offset, DECODE_AHEAD_LENGTH);
				mixerInfo.smpPos -= offset;
			} else {
				int offset = mixerInfo.smpPos + DECODE_AHEAD_LENGTH;
				int endLength = mixerInfo.bufSize - offset;
				memmove (mixerInfo.mixBufferL + endLength, mixerInfo.mixBufferL, DECODE_AHEAD_LENGTH - endLength);
				memmove (mixerInfo.mixBufferL, mixerInfo.mixBufferL + offset, endLength);
				memmove (mixerInfo.mixBufferR + endLength, mixerInfo.mixBufferR, DECODE_AHEAD_LENGTH - endLength);
				memmove (mixerInfo.mixBufferR, mixerInfo.mixBufferR + offset, endLength);
				mixerInfo.smpPos += endLength;
			}
			mixerInfo.lstTimer = 0;

			SetMixState(oldState, SNDMIXER_IDLE);
			ipcSend_Ready(mixerInfo.smpRate);
		} break;
		case SNDMIXER_FINISHING: {
			/* Finish playing what is left in the decoded buffer, then stop
			 * when it has run out. */
			s32 curTimer = TIMER1_DATA;
			s32 smpCount = curTimer - mixerInfo.lstTimer;

			smpCount += (smpCount < 0) << 16;

			ipcSend_Samples(smpCount);
			mixerInfo.smpAvail -= smpCount;

			mixerInfo.lstTimer = curTimer;

			if (mixerInfo.smpAvail <= 0)
			{
				// Immediately stop playback to prevent noise, but cleanup later
				SCHANNEL_CR(0) = 0;
				SCHANNEL_CR(1) = 0;
				SoundSetTimer(0);
				SetMixState(oldState, SNDMIXER_STOP);
			}
		} break;
		case SNDMIXER_STOP: {
			SCHANNEL_CR(0) = 0;
			SCHANNEL_CR(1) = 0;
			SoundSetTimer(0);

			// Free MP3 decoder, next time will start fresh
			if (mixerInfo.hMP3Decoder)
			{
				MP3FreeDecoder(mixerInfo.hMP3Decoder);
				mixerInfo.hMP3Decoder = NULL;
			}
			SetMixState(oldState, SNDMIXER_IDLE);
			ipcSend_End();
		} break;
	}
}
