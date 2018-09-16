
#include "video.h"
#include <nds/arm9/video.h>
#include <stdio.h>
#include <string.h>

// The following is used for page flipping / frame buffer
typedef struct {
	u8*		buffer;
	int		videoMode;
	bool	readyForDisplay;
} VID_PAGE;

#define NUM_VIDEO_PAGES 3

VID_PAGE videoPages[NUM_VIDEO_PAGES] = {
	{(u8*)VRAM_A, MODE_FB0, false},
	{(u8*)VRAM_B, MODE_FB1, false},
	{(u8*)VRAM_C, MODE_FB2, false},
};

static int vidBuf_verticalOffset = 0;
static int vidBuf_currentDecode = 0;
static int vidBuf_currentDisplay = -1;
static int vidBuf_audioDelay = 0;
static volatile int vidBuf_sampleRate = 44100;		// Default to a reasonable value (44.1kHz)
static int vidBuf_frameRate = 1000;			// Default to a reasonable value (10fps)
static volatile int vidBuf_soundSampleCount = 0;
static int volatile vidBuf_videoFrameCount = 0;
static volatile int vidBuf_running = false;

static int volatile vidBuf_videoFramesBehind = 0;

volatile int vidBuf_vblanks = 0;

void vidBuf_CurrentDecodeComplete (void) {
	u16 oldIME = REG_IME;
	REG_IME = 0;

	videoPages[vidBuf_currentDecode].readyForDisplay = true;
	vidBuf_currentDecode++;
	if (vidBuf_currentDecode >= NUM_VIDEO_PAGES)
		vidBuf_currentDecode = 0;

	REG_IME = oldIME;
}

u8* vidBuf_GetNextDecodeBuffer (void) {
	return videoPages[vidBuf_currentDecode].buffer + vidBuf_verticalOffset;
}

int vidBuf_NumFreeDecodeBuffers (void) {
	u16 oldIME = REG_IME;
	REG_IME = 0;

	int freeDecodeBuffers = NUM_VIDEO_PAGES -
	(
		videoPages[0].readyForDisplay +
		videoPages[1].readyForDisplay +
		videoPages[2].readyForDisplay
	);

	REG_IME = oldIME;

	return freeDecodeBuffers;
}

bool vidBuf_ShowNextFrame (void) {
	int nextFrame = vidBuf_currentDisplay + 1;
	if (nextFrame >= NUM_VIDEO_PAGES)
		nextFrame = 0;
	if (!videoPages[nextFrame].readyForDisplay)
		return false;
	videoSetMode(videoPages[nextFrame].videoMode);
	videoPages[vidBuf_currentDisplay].readyForDisplay = false;
	vidBuf_currentDisplay = nextFrame;
	return true;
}

bool vidBuf_NumFramesAvailable (void) {
	u16 oldIME = REG_IME;
	REG_IME = 0;

	int usedDecodeBuffers =
		videoPages[0].readyForDisplay +
		videoPages[1].readyForDisplay +
		videoPages[2].readyForDisplay;

	REG_IME = oldIME;

	return usedDecodeBuffers;
}


void vidBuf_SetSoundSampleRate (int sampleRate) {
	vidBuf_sampleRate = sampleRate;
}

void vidBuf_AddSoundSamples (int sampleCount) {
	vidBuf_soundSampleCount += sampleCount;
}

void vidBuf_SetFrameRate (int frameRate) {
	vidBuf_frameRate = frameRate;
}

void vidBuf_SetHeight (int height) {
	vidBuf_verticalOffset = (SCREEN_HEIGHT - height) * SCREEN_WIDTH * sizeof (u16) / 2;
}

void vidBuf_AdjustDelay (int audioDelay) {
	vidBuf_audioDelay = audioDelay;
}

void vidBuf_StartVideo (void) {
	vidBuf_soundSampleCount = 0;
	vidBuf_videoFrameCount = 0;
	vidBuf_vblanks = 0;
	vidBuf_audioDelay = 0;
	if (vidBuf_NumFramesAvailable() > 0 && vidBuf_ShowNextFrame())
	{
		vidBuf_videoFrameCount++;
	}
	vidBuf_running = true;
}

void vidBuf_ResetVideo (void) {
	int i;
	vidBuf_currentDecode = 0;
	vidBuf_currentDisplay = -1;
	vidBuf_audioDelay = 0;
	vidBuf_soundSampleCount = 0;
	vidBuf_videoFrameCount = 0;
	vidBuf_vblanks = 0;

	for (i = 0; i < NUM_VIDEO_PAGES; i++) {
		videoPages[i].readyForDisplay = false;
		memset (videoPages[i].buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
	}
}

void vidBuf_ClearFutureBuffers (void) {
	int i;
	vidBuf_soundSampleCount = 0;
	vidBuf_videoFrameCount = 0;
	vidBuf_vblanks = 0;
	vidBuf_currentDecode = vidBuf_currentDisplay + 1;
	if (vidBuf_currentDecode >= NUM_VIDEO_PAGES)
		vidBuf_currentDecode = 0;

	for (i = 0; i < NUM_VIDEO_PAGES; i++) {
		if (i != vidBuf_currentDisplay)
			videoPages[i].readyForDisplay = false;
	}
}

void vidBuf_ResumeVideo (void) {
	vidBuf_running = true;
}

void vidBuf_StopVideo (void) {
	vidBuf_running = false;
}

void vidBuf_GetStats (VidBufStats* stats) {
	stats->avgFrameRate = vidBuf_videoFrameCount * 60 * 100 / vidBuf_vblanks;
	stats->framesShown = vidBuf_videoFrameCount;
	if (vidBuf_videoFramesBehind >= 0) {
		stats->framesBehind = -vidBuf_videoFramesBehind  / (100 * vidBuf_sampleRate);
	} else {
		stats->framesBehind = vidBuf_NumFramesAvailable();
	}
}


void vidBuf_VblankHandler (void) {
	int64_t delay;

	if (!vidBuf_running)
		return;

	vidBuf_vblanks++;

	delay = (int64_t)vidBuf_soundSampleCount * (int64_t)vidBuf_frameRate
		- (int64_t)(vidBuf_videoFrameCount - vidBuf_audioDelay) * (int64_t)vidBuf_sampleRate * 100;

	vidBuf_videoFramesBehind = (int)delay;

	if (delay > 0 && vidBuf_NumFramesAvailable() > 0 && vidBuf_ShowNextFrame())
	{
		vidBuf_videoFrameCount++;
	}
}

