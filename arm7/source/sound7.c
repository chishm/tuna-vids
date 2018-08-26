#include "sound7.h"
		
tMixerInfo mixerInfo = {0};

#define DECODE_AHEAD_LENGTH (mixerInfo.bufSize >> 1)

void SoundNextAviChunk (void) {
	AviChunkHeader aviChunkHeader;
	
	// Scan forward in AVI buffer for next audio data chunk
	do {
		mixerInfo.mp3ChunkAddr += mixerInfo.mp3ChunkSize;
		mixerInfo.aviRemain -= mixerInfo.mp3ChunkSize;
		ipcSend(CMDFIFO9_MP3_AMOUNTUSED);
		ipcSend(mixerInfo.mp3ChunkSize);
		memcpy (&aviChunkHeader, mixerInfo.mp3ChunkAddr, sizeof (AviChunkHeader));
		// Lists contain chunks, but we're only interested in the chunks inside, so ignore list headers
		if (aviChunkHeader.fourcc == LIST_ID) {
			aviChunkHeader.size = sizeof(AviListHeader)-sizeof(AviChunkHeader);
		}
		mixerInfo.mp3ChunkSize = ((aviChunkHeader.size + 1) & ~1) + sizeof(AviChunkHeader);
		if (mixerInfo.mp3ChunkAddr >= mixerInfo.aviBufStart + mixerInfo.aviBufSize) {
			mixerInfo.mp3ChunkAddr -= mixerInfo.aviBufSize;
		}
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

void SoundInit()
{
	// Enable Sound
	powerON(POWER_SOUND);
	SOUND_CR = SOUND_ENABLE | SOUND_VOL(127);
	SOUND_BIAS = 0x200;
		
	// Allocate buffers
	mixerInfo.mixBufferL = malloc (sizeof(s16) * SNDMIXER_BUFFER_SIZE);
	mixerInfo.mixBufferR = malloc (sizeof(s16) * SNDMIXER_BUFFER_SIZE);
	mixerInfo.musicBuf = malloc(sizeof(s16) * SNDMIXER_BUFFER_SIZE);

	// Init sound
	SoundSetTimer(0);
		
	// Init the helix mp3 decoder
	mixerInfo.hMP3Decoder = MP3InitDecoder();
		
	// Adjust settings
	mixerInfo.curState = SNDSTATE_IDLE;
	mixerInfo.bufSize = SNDMIXER_BUFFER_SIZE;
}

void SoundStartMP3(u8* aviBuffer, int aviBufLen, int aviBufPos, int aviRemain)
{
	AviChunkHeader aviChunkHeader;

	mixerInfo.aviBufStart = aviBuffer;
	mixerInfo.aviBufSize = aviBufLen;
	mixerInfo.mp3ChunkAddr = aviBuffer + aviBufPos;
	mixerInfo.aviRemain = aviRemain;
	
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
		
	// Decode 1 / 2 of the buffersize
	SoundMixCallback(mixerInfo.mixBufferL, mixerInfo.mixBufferR, DECODE_AHEAD_LENGTH);
	mixerInfo.smpPos = DECODE_AHEAD_LENGTH;
		
	// Set the right rate, ...
	MP3GetLastFrameInfo(mixerInfo.hMP3Decoder, &mixerInfo.mp3FrameInfo);
	mixerInfo.smpRate = mixerInfo.mp3FrameInfo.samprate;
//	mixerInfo.curTimer = 0x1000000 / mixerInfo.smpRate;
	mixerInfo.curTimer = 0xFFB0FF / mixerInfo.smpRate;

	ipcSend(CMDFIFO9_MP3_READY);
	ipcSend(mixerInfo.smpRate);
}

void SoundPlayMP3(void) {
	// Start streaming
	mixerInfo.mixState = SNDMIXER_SETUPSTREAM;
	mixerInfo.curState = SNDSTATE_PLAY;
}

void SoundPauseMP3(void) {
	mixerInfo.mixState = SNDMIXER_PAUSE;
}

void SoundStopPlayback(void) 
{
	mixerInfo.mixState = SNDMIXER_STOP;
}

void SoundVolume(u32 volume) {
    if (volume > 127) volume = 127;
    SCHANNEL_CR(0) =  (SCHANNEL_CR(0) & ~SOUND_VOL(127)) | SOUND_VOL(volume);
    SCHANNEL_CR(1) =  (SCHANNEL_CR(1) & ~SOUND_VOL(127)) | SOUND_VOL(volume);
}

void SoundLoopStep()
{
	switch(mixerInfo.mixState) {
		case SNDMIXER_SETUPSTREAM: {
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
			SCHANNEL_CR(0)				= SCHANNEL_ENABLE | SOUND_REPEAT | SOUND_VOL(127) | SOUND_PAN(0) | SOUND_16BIT;
			SCHANNEL_CR(1)				= SCHANNEL_ENABLE | SOUND_REPEAT | SOUND_VOL(127) | SOUND_PAN(127) | SOUND_16BIT;
			
			mixerInfo.mixState = SNDMIXER_STREAMING;
		} break;
		case SNDMIXER_STREAMING: {
			s32 curTimer = TIMER1_DATA;
			s32 smpCount = curTimer - mixerInfo.lstTimer;
				
			smpCount += (smpCount < 0) << 16;
			
			ipcSend(CMDFIFO9_MP3_SAMPLES);
			ipcSend(smpCount);
			
			mixerInfo.lstTimer = curTimer;
				
			SoundMix(smpCount);
				
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
			
			ipcSend(CMDFIFO9_MP3_SAMPLES);
			ipcSend(smpCount);
			
			mixerInfo.lstTimer = curTimer;
				
			SoundMix(smpCount);
				
			mixerInfo.smpPos += smpCount;
			mixerInfo.smpPos %= mixerInfo.bufSize;
			
			// Adjust data so that playback starts from the begining of the buffer
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
			
			mixerInfo.mixState = SNDMIXER_IDLE;
			mixerInfo.curState = SNDSTATE_IDLE;

			ipcSend(CMDFIFO9_MP3_READY);
			ipcSend(mixerInfo.smpRate);
		} break;
		case SNDMIXER_STOP: {
			u8* aid_readPtr = mixerInfo.mp3BufAddr;
			int aid_bytesLeft = 0;
			
			SCHANNEL_CR(0) = 0;
			SCHANNEL_CR(1) = 0;
			SoundSetTimer(0);
			
			// Reset MP3 decoder
			MP3Decode(mixerInfo.hMP3Decoder, &aid_readPtr, &aid_bytesLeft, mixerInfo.musicBuf, 0);
			mixerInfo.mixState = SNDMIXER_IDLE;
			mixerInfo.curState = SNDSTATE_IDLE;
			ipcSend(CMDFIFO9_MP3_END);
		} break;
		default:
			break;
	}
}

void SoundMixCallback(s16* streamL, s16* streamR, u32 smpCount)
{
	int restSample, minSamples, usedBytes;
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
			deinterleaveStereo (streamL, streamR, ((u32*)mixerInfo.musicBuf) + mixerInfo.nMusicBufStart, minSamples);
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
	if(smpCount > 0) {
		while(smpCount > 0) {
			// More data is needed. Decode a mp3 frame to outBuf
			// Find start of next MP3 frame - assume EOF if no sync found
            usedBytes = MP3FindSyncWord(mixerInfo.mp3BufAddr, mixerInfo.mp3BufRemain);
			if(usedBytes < 0) {
				SoundStopPlayback();
				mixerInfo.curState = SNDSTATE_ENDOFDATA;
#ifdef REPORT_ARM7_ERRORS
				ipcSend(CMDFIFO9_ERROR);
				ipcSend((0x10000000 - (usedBytes << 24)) | (mixerInfo.mp3BufAddr - mixerInfo.aviBufStart));
#endif
				return;
			}
			SoundUsedAmount(usedBytes);
			
            aid_readPtr = mixerInfo.mp3BufAddr;
            aid_bytesLeft  = mixerInfo.mp3BufRemain;
            
			// Decode one MP3 frame to the buffer
			if ((error = MP3Decode(mixerInfo.hMP3Decoder, &aid_readPtr, &aid_bytesLeft, mixerInfo.musicBuf, 0))) {
#ifdef REPORT_ARM7_ERRORS
				ipcSend(CMDFIFO9_ERROR);
				ipcSend((0x20000000 - (error << 24)) | (mixerInfo.mp3BufAddr - mixerInfo.aviBufStart));
#endif
//				SoundStopPlayback();
//				mixerInfo.curState = SNDSTATE_ERROR;
//				return;
			}
            
			usedBytes = aid_readPtr - mixerInfo.mp3BufAddr;
			SoundUsedAmount(usedBytes);
				
			// Copy needed data to the stream
			MP3GetLastFrameInfo(mixerInfo.hMP3Decoder, &mixerInfo.mp3FrameInfo);
			mixerInfo.numChannels = mixerInfo.mp3FrameInfo.nChans;		// Stereo or mono?
			if (mixerInfo.numChannels == 2) {
				mixerInfo.mp3FrameInfo.outputSamps /= 2;
			}
			minSamples = min(mixerInfo.mp3FrameInfo.outputSamps, smpCount);
			restSample = mixerInfo.mp3FrameInfo.outputSamps - smpCount;
			smpCount -= minSamples;		
				
			
			if (mixerInfo.numChannels == 2) {
		 		deinterleaveStereo (streamL + outSample, streamR + outSample, (u32*)mixerInfo.musicBuf, minSamples);
			} else {
				memcpy(streamL + outSample, mixerInfo.musicBuf, minSamples * sizeof(s16));
				memcpy(streamR + outSample, mixerInfo.musicBuf, minSamples * sizeof(s16));
			}

 			outSample += minSamples;
		}
		// Set the rest of the decoded data to be used for the next frame
		mixerInfo.nMusicBufStart = minSamples;
		mixerInfo.nMusicBuf = restSample;
	}
}

// Manages the filling of the ring buffer.
void SoundMix(int smpCount)
{
	// Decode data to the ring buffer
 	if (mixerInfo.bufSize - mixerInfo.smpPos <= smpCount) {
 		SoundMixCallback(&mixerInfo.mixBufferL[mixerInfo.smpPos], &mixerInfo.mixBufferR[mixerInfo.smpPos], mixerInfo.bufSize - mixerInfo.smpPos);
 		SoundMixCallback(&mixerInfo.mixBufferL[0], &mixerInfo.mixBufferR[0], smpCount - (mixerInfo.bufSize - mixerInfo.smpPos));
	} else {
		SoundMixCallback(&mixerInfo.mixBufferL[mixerInfo.smpPos], &mixerInfo.mixBufferR[mixerInfo.smpPos], smpCount);
	}
}

void SoundSetTimer(int period)
{
	if(period) {
		TIMER1_DATA = 0;
		TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE | TIMER_DIV_1;
		
		TIMER0_DATA = 0x10000 - (period << 1);
		TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1;
	} else {
		TIMER0_DATA = 0;
		TIMER0_CR = 0;
			
		TIMER1_DATA = 0;
		TIMER1_CR = 0;
	}
}
