#include "ipc7.h"

const voidFuncPtr msgHandlers[CMDFIFO7_SIZE] =
{
	ipcSoundPlay,
	ipcSoundSeek,
	ipcSoundPause,
	ipcSoundStop,
    ipcSoundVolume,
	ipcSoundStart,
	ipcBacklightToggle,
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

void ipcHandler()
{
	// Handle all received messages
	while (!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) {
		// Receive message from FIFO queue
		u32 curMsg = ipcReceive();
		// Execute command according to table
		msgHandlers[curMsg]();
	}
}

// ------------------------------- FIFO Functions ---------------------------- //

void ipcSoundPlay()
{
	SoundPlayMP3();
}

void ipcSoundSeek(void)
{
	// Nothing for now
}

void ipcSoundPause(void)
{
	SoundPauseMP3();
}

void ipcSoundStop(void)
{
		// Stop sound playback
	SoundStopPlayback();
}

void ipcSoundVolume(void)
{
    u32 volume = (u32)ipcReceive();
    SoundVolume(volume);
}

void ipcSoundStart (void) 
{
	// Receive arguments
	u8* aviBuffer = (u8*)ipcReceive();
	int aviBufLen = (int)ipcReceive();
	int aviBufPos = (int)ipcReceive();
	int aviRemain = (int)ipcReceive();
	
	// Play the MP3 buffer
	SoundStartMP3(aviBuffer, aviBufLen, aviBufPos, aviRemain);
}

extern void toggleBottomLight (void);

void ipcBacklightToggle (void) {
	toggleBottomLight ();
}
