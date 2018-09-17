#include "sound9.h"
#include "ipc9.h"
#include "video.h"
#include "avi_buffer.h"

static volatile int amountUsed = 0;
static volatile bool doEnd;
static volatile bool ready;
static volatile bool playerEnded;

void ipcSoundMp3AmountUsed (int amountUsed) {
	amountUsed += amountUsed;
}

void ipcSoundMp3End (void) {
	doEnd = true;
	playerEnded = true;
}

void ipcSoundMp3Ready (int sampleRate) {
	ready = true;
	vidBuf_SetSoundSampleRate (sampleRate);
}

void ipcSoundMp3Samples (int samples) {
	vidBuf_AddSoundSamples (samples);
}

void mp3PlayerLoop (void) {
	int oldIME = enterCriticalSection();
	if (doEnd) {
		doEnd = false;
		/* Stick any finalisation stuff here */
	}
	int amountUsed_copy = amountUsed;
	amountUsed = 0;
	leaveCriticalSection(oldIME);

	if (amountUsed_copy > 0) {
		aviAudioUsedData (amountUsed_copy);
	}
}

bool mp3PlayerStart (void) {
	ready = false;
	// Stop all previous playback
	mp3PlayerStop(true);
	amountUsed = 0;
	ipcSend_Start(aviBuffer.bufferStart, AVI_BUFFER_SIZE,
		aviBuffer.audioReadPointer - aviBuffer.bufferStart,
		aviBuffer.moviRemain + (aviBuffer.audioAmountLeft - aviBuffer.videoAmountLeft));
	return true;
}

void mp3PlayerPause(void) {
	ready = false;
    ipcSend_Pause();
}

void mp3PlayerStop(bool waitUntilStopped) {
    playerEnded = false;
	ipcSend_Stop();
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
	ipcSend_Play();
	playerEnded = false;
	return true;
}

void mp3PlayerVolume(u32 volume) {
	ipcSend_Volume(volume);
}

bool mp3PlayerHasEnded (void) {
	return playerEnded;
}
