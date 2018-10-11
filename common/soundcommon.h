#ifndef COMMON_SOUNDCOMMON_H_
#define COMMON_SOUNDCOMMON_H_

#include <nds/fifocommon.h>
#include <nds/ndstypes.h>

// FIFO channel used for MP3 control
#define CMDFIFO_MP3 FIFO_USER_01

// Commands the ARM7 can receive
typedef enum
{
    CMDFIFO7_MP3_PLAY = 1,
    CMDFIFO7_MP3_SEEK,
    CMDFIFO7_MP3_PAUSE,
    CMDFIFO7_MP3_STOP,
    CMDFIFO7_MP3_VOLUME,
    CMDFIFO7_MP3_START,
    CMDFIFO7_BACKLIGHT_TOGGLE,
    CMDFIFO7_EXIT,
} CmdFifo7_e;

struct CmdFifo7_Mp3_Volume
{
    u32 volume;
};

struct CmdFifo7_Mp3_Start
{
    u8* aviBuffer;
    int aviBuffLen;
    int aviBufPos;
    int aviRemain;
};

typedef struct CmdFifo7
{
    CmdFifo7_e command;
    union
    {
        struct CmdFifo7_Mp3_Volume volume;
        struct CmdFifo7_Mp3_Start start;
    } data;
} CmdFifo7;

// Commands the ARM9 can receive
typedef enum
{
    CMDFIFO9_MP3_AMOUNTUSED = 1,
    CMDFIFO9_MP3_END,
    CMDFIFO9_MP3_READY,
    CMDFIFO9_MP3_SAMPLES,

    CMDFIFO9_ERROR,
} CmdFifo9_e;

struct CmdFifo9_Mp3_AmountUsed
{
    int amountUsed;
};

struct CmdFifo9_Mp3_Ready
{
    int sampleRate;
};

struct CmdFifo9_Mp3_Samples
{
    int samples;
};

struct CmdFifo9_Mp3_Error
{
    u32 error;
};

typedef struct CmdFifo9
{
    CmdFifo9_e command;
    union
    {
        struct CmdFifo9_Mp3_AmountUsed amountUsed;
        struct CmdFifo9_Mp3_Ready ready;
        struct CmdFifo9_Mp3_Samples samples;
        struct CmdFifo9_Mp3_Error error;
    } data;
} CmdFifo9;

#endif // COMMON_SOUNDCOMMON_H_
