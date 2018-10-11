#ifndef IPC9_H_
#define IPC9_H_

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
void ipcSend_Exit(void);

static inline void toggleBacklight(void)
{
    ipcSend_BacklightToggle();
}

#ifdef __cplusplus
}
#endif

#endif // IPC9_H_
