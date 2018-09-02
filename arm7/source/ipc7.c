#include "ipc7.h"
#include "main.h"

static void ipcHandler(int num_bytes, void *userdata)
{
	CmdFifo7 cmdFifo;

	if (num_bytes < sizeof(cmdFifo.command) || num_bytes >> sizeof(cmdFifo))
	{
		// Too little or too much data. This shouldn't happen.
		return;
	}

	fifoGetDatamsg(CMDFIFO_MP3, sizeof(cmdFifo), (void*)&cmdFifo);

	switch (cmdFifo.command)
	{
	case CMDFIFO7_MP3_PLAY:
		SoundPlayMP3();
		break;
	case CMDFIFO7_MP3_SEEK:
		// Nothing for now
		break;
	case CMDFIFO7_MP3_PAUSE:
		SoundPauseMP3();
		break;
	case CMDFIFO7_MP3_STOP:
		SoundStopPlayback();
		break;
	case CMDFIFO7_MP3_VOLUME:
		SoundVolume(cmdFifo.data.volume.volume);
		break;
	case CMDFIFO7_MP3_START:
		// Play the MP3 buffer
		SoundStartMP3(
			cmdFifo.data.start.aviBuffer, cmdFifo.data.start.aviBuffLen,
			cmdFifo.data.start.aviBufPos, cmdFifo.data.start.aviRemain);
		break;
	case CMDFIFO7_BACKLIGHT_TOGGLE:
		toggleBottomLight();
		break;
	}
}

void ipcInit(void)
{
	fifoSetDatamsgHandler(CMDFIFO_MP3, ipcHandler, NULL);
}

void ipcSend_AmountUsed(int amountUsed)
{
	CmdFifo9 cmdFifo;

	cmdFifo.command = CMDFIFO9_MP3_AMOUNTUSED;
	cmdFifo.data.amountUsed.amountUsed = amountUsed;

	fifoSendDatamsg(
		CMDFIFO_MP3,
		offsetof(CmdFifo9, data) + sizeof(cmdFifo.data.amountUsed),
		(void*)&cmdFifo);
}

void ipcSend_End(void)
{
	CmdFifo9_e command = CMDFIFO9_MP3_END;
	fifoSendDatamsg(CMDFIFO_MP3, sizeof(command), &command);
}

void ipcSend_Ready(int sampleRate)
{
	CmdFifo9 cmdFifo;

	cmdFifo.command = CMDFIFO9_MP3_READY;
	cmdFifo.data.ready.sampleRate = sampleRate;

	fifoSendDatamsg(
		CMDFIFO_MP3,
		offsetof(CmdFifo9, data) + sizeof(cmdFifo.data.ready),
		(void*)&cmdFifo);
}

void ipcSend_Samples(int samples)
{
	CmdFifo9 cmdFifo;

	cmdFifo.command = CMDFIFO9_MP3_SAMPLES;
	cmdFifo.data.samples.samples = samples;

	fifoSendDatamsg(
		CMDFIFO_MP3,
		offsetof(CmdFifo9, data) + sizeof(cmdFifo.data.samples),
		(void*)&cmdFifo);
}

void ipcSend_Error(u32 error)
{
	CmdFifo9 cmdFifo;

	cmdFifo.command = CMDFIFO9_ERROR;
	cmdFifo.data.error.error = error;

	fifoSendDatamsg(
		CMDFIFO_MP3,
		offsetof(CmdFifo9, data) + sizeof(cmdFifo.data.error),
		(void*)&cmdFifo);
}
