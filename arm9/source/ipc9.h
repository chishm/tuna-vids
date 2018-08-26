#ifndef __FIFO9_H__
#define __FIFO9_H__

#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif

void ipcInit();
void ipcHandler();
u32 ipcReceive();
void ipcSend(u32 sendData);

void toggleBacklight (void);

#ifdef __cplusplus
}
#endif

#endif
