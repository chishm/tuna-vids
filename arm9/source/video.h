
#ifndef _VIDEO_H
#define _VIDEO_H

#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int avgFrameRate;
	int curFrameRate;
	int framesShown;
	int framesBehind;
} VidBufStats;

extern int vidBuf_vblanks;

void vidBuf_CurrentDecodeComplete (void);
u8* vidBuf_GetNextDecodeBuffer (void);
int vidBuf_NumFreeDecodeBuffers (void);

bool vidBuf_ShowNextFrame (void);
bool vidBuf_NumFramesAvailable (void);

void vidBuf_SetSoundSampleRate (int sampleRate);
void vidBuf_AddSoundSamples (int sampleCount);
void vidBuf_SetFrameRate (int frameRate);	// Frame rate in centihertz
void vidBuf_SetHeight (int height);

void vidBuf_AdjustDelay (int audioDelay);
void vidBuf_StartVideo (void);
void vidBuf_ResumeVideo (void);
void vidBuf_StopVideo (void);
void vidBuf_ResetVideo (void);
void vidBuf_ClearFutureBuffers (void);

void vidBuf_GetStats (VidBufStats* stats);		// Returns frame rates in centihertz
void vidBuf_VblankHandler (void);

#ifdef __cplusplus
}
#endif

#endif // _VIDEO_H
