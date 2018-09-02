#include "../../common/soundcommon.h"

#ifndef __SOUND9_H__
#define __SOUND9_H__

#include <nds.h>


enum Mp3PlayStyle { MP3_ONCE, MP3_LOOP };

    // called by ARM7
void ipcSoundMp3AmountUsed (int amountUsed);
void ipcSoundMp3FileSeek (void);
void ipcSoundMp3End (void);
void ipcSoundMp3Ready (int sampleRate);
void ipcSoundMp3Samples (int samples);

    // called by the user on ARM9
void mp3PlayerLoop (void);
bool mp3PlayerStart (void);
bool mp3PlayerPlay(bool delayUntilReady);
void mp3PlayerPause(void);
void mp3PlayerStop(bool waitUntilStopped);
void mp3PlayerVolume(u32 volume);
void soundMp3End (void);
void mp3PlayerInit(void);
bool mp3PlayerHasEnded(void);

#endif
