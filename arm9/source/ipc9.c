#include "ipc9.h"
#include "sound9.h"

void ipcError (void) {
#ifdef REPORT_ARM7_ERRORS
	u32 error = ipcReceive();
	iprintf ("ARM7 error %08X\n", error);
#endif
}

const voidFuncPtr msgHandlers[CMDFIFO9_SIZE] =
{
    ipcSoundMp3AmountUsed,
	ipcSoundMp3End,
	ipcSoundMp3Ready,
	ipcSoundMp3Samples,
	
	ipcError,
};

void ipcInit()
{
		// Enable FIFO irq
	irqEnable(IRQ_FIFO_NOT_EMPTY);
	irqSet(IRQ_FIFO_NOT_EMPTY, ipcHandler);
		
		// Enable FIFO RX and TX
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_EMPTY | IPC_FIFO_RECV_IRQ;
}

u32 ipcReceive()
{
		// Wait until FIFO has data
	while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		
		// Read data
	return REG_IPC_FIFO_RX;
}

void ipcSend(u32 sendData)
{
		// Saving IME's value
	u16 oldIME = REG_IME;
	REG_IME = 0;
		
		// No errors present
	while(REG_IPC_FIFO_CR & IPC_FIFO_ERROR) {
		REG_IPC_FIFO_CR = IPC_FIFO_ERROR;
	}
		
		// Wait until FIFO has space on queue
	while(REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL);
		
		// Send away
	REG_IPC_FIFO_TX = sendData;
		
		// Restoring IME's value
	REG_IME = oldIME;
}

void ipcHandler(void)
{
	// Handle all received messages
	while (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) {
		// Receive message from FIFO queue
		u32 curMsg = ipcReceive();
		// Execute command according to table
		msgHandlers[curMsg]();
	}
}

void toggleBacklight (void) {
	ipcSend (CMDFIFO7_BACKLIGHT_TOGGLE);
}
