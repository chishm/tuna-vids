#ifndef IPC7_H_
#define IPC7_H_

#include <nds/ndstypes.h>

bool ipcInit(void);

void ipcSend_AmountUsed(int amountUsed);
void ipcSend_End(void);
void ipcSend_Ready(int sampleRate);
void ipcSend_Samples(int samples);
void ipcSend_Error(u32 error);

#endif // IPC7_H_
