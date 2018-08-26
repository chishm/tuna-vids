#include "main.h"

#ifndef __FIFO7_H__
#define __FIFO7_H__

void ipcInit(void);
void ipcHandler(void);
u32 ipcReceive(void);
void ipcSend(u32 sendData);

void ipcSoundPlay(void);
void ipcSoundSeek(void);
void ipcSoundPause(void);
void ipcSoundStop(void);
void ipcSoundVolume(void);
void ipcSoundStart(void);
void ipcBacklightToggle(void);

#endif
