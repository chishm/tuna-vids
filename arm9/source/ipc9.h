#ifndef __FIFO9_H__
#define __FIFO9_H__

#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ipcInit();

void ipcSend_Play(void);
void ipcSend_Seek(void);
void ipcSend_Pause(void);
void ipcSend_Stop(void);
void ipcSend_Volume(u32 volume);
void ipcSend_Start(u8* aviBuffer, int aviBuffLen, int aviBufPos, int aviRemain);
void ipcSend_BacklightToggle(void);

static inline void toggleBacklight (void) {
	ipcSend_BacklightToggle();
}

#ifdef __cplusplus
}
#endif

#endif
