#include "controls.h"
#include "errors.h"
#include "ipc9.h"
#include "sound9.h"
#include "soundcommon.h"

static void ipcHandler(int num_bytes, void *userdata)
{
    CmdFifo9 cmdFifo;

    if (num_bytes != sizeof(cmdFifo)) {
        displayError(ERROR_ARM9_FIFO_BUFF_SIZE, true, false);
        displayError(num_bytes, true, true);
        return;
    }

    if (fifoGetDatamsg(CMDFIFO_MP3, sizeof(cmdFifo), (u8*) &cmdFifo)
        != sizeof(cmdFifo)) {
        displayError(ERROR_ARM9_FIFO_RECEIVE, true, true);
        return;
    }

    switch (cmdFifo.command) {
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
        displayError(cmdFifo.data.error.error, false, true);
        break;
    default:
        displayError(ERROR_ARM9_FIFO_INVALID_COMMAND, true, true);
        break;
    }
}

static void ipcSendCmd7(CmdFifo7* cmdFifo)
{
    if (!fifoSendDatamsg(CMDFIFO_MP3, sizeof(CmdFifo7), (u8*) cmdFifo)) {
        displayError(ERROR_ARM9_FIFO_SEND, true, false);
        displayError(cmdFifo->command, true, true);
    }
}

bool ipcInit(void)
{
    return fifoSetDatamsgHandler(CMDFIFO_MP3, ipcHandler, NULL);
}

void ipcSend_Play(void)
{
    CmdFifo7 cmdFifo = {.command = CMDFIFO7_MP3_PLAY};
    ipcSendCmd7(&cmdFifo);
}

void ipcSend_Seek(void)
{
    CmdFifo7 cmdFifo = {.command = CMDFIFO7_MP3_SEEK};
    ipcSendCmd7(&cmdFifo);
}

void ipcSend_Pause(void)
{
    CmdFifo7 cmdFifo = {.command = CMDFIFO7_MP3_PAUSE};
    ipcSendCmd7(&cmdFifo);
}

void ipcSend_Stop(void)
{
    CmdFifo7 cmdFifo = {.command = CMDFIFO7_MP3_STOP};
    ipcSendCmd7(&cmdFifo);
}

void ipcSend_Volume(u32 volume)
{
    CmdFifo7 cmdFifo = {.command = CMDFIFO7_MP3_VOLUME};
    cmdFifo.data.volume.volume = volume;

    ipcSendCmd7(&cmdFifo);
}

void ipcSend_Start(u8* aviBuffer, int aviBuffLen, int aviBufPos, int aviRemain)
{
    CmdFifo7 cmdFifo = {.command = CMDFIFO7_MP3_START};
    cmdFifo.data.start.aviBuffer = aviBuffer;
    cmdFifo.data.start.aviBuffLen = aviBuffLen;
    cmdFifo.data.start.aviBufPos = aviBufPos;
    cmdFifo.data.start.aviRemain = aviRemain;

    ipcSendCmd7(&cmdFifo);
}

void ipcSend_BacklightToggle(void)
{
    CmdFifo7 cmdFifo = {.command = CMDFIFO7_BACKLIGHT_TOGGLE};
    ipcSendCmd7(&cmdFifo);
}

void ipcSend_Exit(void)
{
    CmdFifo7 cmdFifo = {.command = CMDFIFO7_EXIT};
    ipcSendCmd7(&cmdFifo);
}
