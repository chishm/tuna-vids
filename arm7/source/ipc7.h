#ifndef __FIFO7_H__
#define __FIFO7_H__

#include <nds/ndstypes.h>

void ipcInit(void);

void ipcSend_AmountUsed(int amountUsed);
void ipcSend_End(void);
void ipcSend_Ready(int sampleRate);
void ipcSend_Samples(int samples);
void ipcSend_Error(u32 error);

#endif
