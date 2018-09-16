#include "errors.h"
#include "ipc7.h"
#include "main.h"

static void ipcHandler(int num_bytes, void *userdata)
{
	CmdFifo7 cmdFifo;

	if (num_bytes != sizeof(cmdFifo))
	{
		ipcSend_Error(ERROR_ARM7_FIFO_BUFF_SIZE);
		return;
	}

	if (fifoGetDatamsg(CMDFIFO_MP3, sizeof(cmdFifo), (u8*)&cmdFifo) != sizeof(cmdFifo))
	{
		ipcSend_Error(ERROR_ARM7_FIFO_RECEIVE);
		return;
	}

	fifoGetDatamsg(CMDFIFO_MP3, sizeof(cmdFifo), (u8*)&cmdFifo);

	switch (cmdFifo.command)
	{
	case CMDFIFO7_MP3_PLAY:
		SoundState_Play();
		break;
	case CMDFIFO7_MP3_SEEK:
		// Nothing for now
		break;
	case CMDFIFO7_MP3_PAUSE:
		SoundState_Pause();
		break;
	case CMDFIFO7_MP3_STOP:
		SoundState_Stop();
		break;
	case CMDFIFO7_MP3_VOLUME:
		SoundState_Volume(cmdFifo.data.volume.volume);
		break;
	case CMDFIFO7_MP3_START:
		// Play the MP3 buffer
		SoundState_Start(
			cmdFifo.data.start.aviBuffer, cmdFifo.data.start.aviBuffLen,
			cmdFifo.data.start.aviBufPos, cmdFifo.data.start.aviRemain);
		break;
	case CMDFIFO7_BACKLIGHT_TOGGLE:
		toggleBottomLight();
		break;
	default:
		ipcSend_Error(ERROR_ARM7_FIFO_INVALID_COMMAND);
		break;
	}
}

static void ipcSendCmd9(CmdFifo9* cmdFifo)
{
	if (!fifoSendDatamsg(CMDFIFO_MP3, sizeof(CmdFifo9), (u8*)cmdFifo))
	{
		ipcSend_Error(ERROR_ARM7_FIFO_SEND);
	}
}

void ipcInit(void)
{
	fifoSetDatamsgHandler(CMDFIFO_MP3, ipcHandler, NULL);
}

void ipcSend_AmountUsed(int amountUsed)
{
	CmdFifo9 cmdFifo = {.command = CMDFIFO9_MP3_AMOUNTUSED};
	cmdFifo.data.amountUsed.amountUsed = amountUsed;
	ipcSendCmd9(&cmdFifo);
}

void ipcSend_End(void)
{
	CmdFifo9 cmdFifo = {.command = CMDFIFO9_MP3_END};
	ipcSendCmd9(&cmdFifo);
}

void ipcSend_Ready(int sampleRate)
{
	CmdFifo9 cmdFifo = {.command = CMDFIFO9_MP3_READY};
	cmdFifo.data.ready.sampleRate = sampleRate;
	ipcSendCmd9(&cmdFifo);
}

void ipcSend_Samples(int samples)
{
	CmdFifo9 cmdFifo = {.command = CMDFIFO9_MP3_SAMPLES};
	cmdFifo.data.samples.samples = samples;
	ipcSendCmd9(&cmdFifo);
}

void ipcSend_Error(u32 error)
{
	CmdFifo9 cmdFifo = {.command = CMDFIFO9_ERROR};
	cmdFifo.data.error.error = error;
	ipcSendCmd9(&cmdFifo);
}
