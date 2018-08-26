#include "sound9.h"
#include "ipc9.h"
#include "video.h"
#include "avi_buffer.h"

static int amountUsed = 0;
static bool doEnd;
static bool ready;
static bool playerEnded;

void ipcSoundMp3AmountUsed (void) {
	amountUsed += ipcReceive();
}

void ipcSoundMp3End (void) {
	doEnd = true;
	playerEnded = true; 
}

void ipcSoundMp3Ready (void) {
	int sampleRate = ipcReceive();
	ready = true;
	vidBuf_SetSoundSampleRate (sampleRate);
}

void ipcSoundMp3Samples (void) {
	int samples = ipcReceive();
	vidBuf_AddSoundSamples (samples);
}

void mp3PlayerLoop (void) {
	u16 oldIME = REG_IME;
	REG_IME = 0;
	
	if (doEnd) {
		doEnd = false;
		/* Stick any finalisation stuff here */
	}
	if (amountUsed > 0) {
		aviAudioUsedData (amountUsed);
		amountUsed = 0;
	}

	REG_IME = oldIME;
}


bool mp3PlayerStart (void) {
	ready = false;
	// Stop all previous playback
	mp3PlayerStop(false);
	mp3PlayerLoop();	// Catch any last requests of the ARM7
	ipcSend(CMDFIFO7_MP3_START);
	// aviBuffer
	ipcSend((int)aviBuffer.bufferStart);
	// aviBufLen
	ipcSend(AVI_BUFFER_SIZE);
	// aviBufPos
	ipcSend(aviBuffer.audioReadPointer - aviBuffer.bufferStart);
	// aviRemain
	ipcSend( aviBuffer.moviRemain + (aviBuffer.audioAmountLeft - aviBuffer.videoAmountLeft));
	return true;
}

void mp3PlayerPause(void) {
	ready = false;
    ipcSend(CMDFIFO7_MP3_PAUSE);
}

void mp3PlayerStop(bool waitUntilStopped) {
    playerEnded = false;
	ipcSend(CMDFIFO7_MP3_STOP);
	if (waitUntilStopped) {
		// Delay until player has stopped
		while (!playerEnded) {
			swiWaitForVBlank ();
		}
	}
}

bool mp3PlayerPlay(bool delayUntilReady) {
	if (delayUntilReady) {
		// Delay until player is ready
		while (!ready) {
			swiWaitForVBlank ();
		}
	} else {
		if (!ready) {
			return false;
		}
	}
	ipcSend(CMDFIFO7_MP3_PLAY);
	playerEnded = false;
	return true;
}

void mp3PlayerVolume(u32 volume) {
    ipcSend(CMDFIFO7_MP3_VOLUME);
    ipcSend(volume);
}

bool mp3PlayerHasEnded (void) {
	return playerEnded;
}
