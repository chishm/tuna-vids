#include "ipc9.h"
#include "sound9.h"

static void ipcHandler(int num_bytes, void *userdata)
{
	CmdFifo9 cmdFifo;

	if (num_bytes < sizeof(cmdFifo.command) || num_bytes >> sizeof(cmdFifo))
	{
		// Too little or too much data. This shouldn't happen.
		return;
	}

	fifoGetDatamsg(CMDFIFO_MP3, sizeof(cmdFifo), (void*)&cmdFifo);

	switch (cmdFifo.command)
	{
	case CMDFIFO9_MP3_AMOUNTUSED:
		ipcSoundMp3AmountUsed(cmdFifo.data.amountUsed.amountUsed);
		break;
	case CMDFIFO9_MP3_END:
		ipcSoundMp3End();
		break;
	case CMDFIFO9_MP3_READY:
		ipcSoundMp3Ready(cmdFifo.data.ready.sampleRate);
		break;
	case CMDFIFO9_MP3_SAMPLES:
		ipcSoundMp3Samples(cmdFifo.data.samples.samples);
		break;
	case CMDFIFO9_ERROR:
		break;
	}
}

void ipcInit(void)
{
	fifoSetDatamsgHandler(CMDFIFO_MP3, ipcHandler, NULL);
}


void ipcSend_Play(void)
{
	CmdFifo7_e command = CMDFIFO7_MP3_PLAY;
	fifoSendDatamsg(CMDFIFO_MP3, sizeof(command), &command);
}

void ipcSend_Seek(void)
{
	CmdFifo7_e command = CMDFIFO7_MP3_SEEK;
	fifoSendDatamsg(CMDFIFO_MP3, sizeof(command), &command);
}

void ipcSend_Pause(void)
{
	CmdFifo7_e command = CMDFIFO7_MP3_PAUSE;
	fifoSendDatamsg(CMDFIFO_MP3, sizeof(command), &command);
}

void ipcSend_Stop(void)
{
	CmdFifo7_e command = CMDFIFO7_MP3_STOP;
	fifoSendDatamsg(CMDFIFO_MP3, sizeof(command), &command);
}

void ipcSend_Volume(u32 volume)
{
	CmdFifo7 cmdFifo;

	cmdFifo.command = CMDFIFO7_MP3_VOLUME;
	cmdFifo.data.volume.volume = volume;

	fifoSendDatamsg(
		CMDFIFO_MP3,
		offsetof(CmdFifo7, data) + sizeof(cmdFifo.data.volume),
		(void*)&cmdFifo);
}

void ipcSend_Start(u8* aviBuffer, int aviBuffLen, int aviBufPos, int aviRemain)
{
	CmdFifo7 cmdFifo;

	cmdFifo.command = CMDFIFO7_MP3_START;
	cmdFifo.data.start.aviBuffer = aviBuffer;
	cmdFifo.data.start.aviBuffLen = aviBuffLen;
	cmdFifo.data.start.aviBufPos = aviBufPos;
	cmdFifo.data.start.aviRemain = aviRemain;

	fifoSendDatamsg(
		CMDFIFO_MP3,
		offsetof(CmdFifo7, data) + sizeof(cmdFifo.data.start),
		(void*)&cmdFifo);
}

void ipcSend_BacklightToggle(void)
{
	CmdFifo7_e command = CMDFIFO7_BACKLIGHT_TOGGLE;
	fifoSendDatamsg(CMDFIFO_MP3, sizeof(command), &command);
}
