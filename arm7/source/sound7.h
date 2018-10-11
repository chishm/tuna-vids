#ifndef SOUND7_H_
#define SOUND7_H_

#include "main.h"
#include "helix/mp3dec.h"
#include "../../common/soundcommon.h"
#include "avi_headers.h"

#define SNDMIXER_BUFFER_SIZE (4 * MAX_NCHAN * MAX_NGRAN * MAX_NSAMP)

// Sound mixer state
typedef enum
{
    SNDMIXER_UNINITIALISED,
    SNDMIXER_IDLE,
    SNDMIXER_START,
    SNDMIXER_PLAY,
    SNDMIXER_PLAYING,
    SNDMIXER_PAUSE,
    SNDMIXER_FINISHING,
    SNDMIXER_STOP,
} sndMixerState;

typedef struct
{
    // State
    sndMixerState mixState;

    // Buffer info
    u32 bufSize;

    // Sample info
    u32 smpPos, smpRate;
    s32 curTimer, lstTimer;
    // Number of unplayed samples available in the mix buffers
    s32 smpAvail;
    int numChannels;

    // AVI buffer
    u8* aviBufStart;
    int aviBufSize;
    int aviRemain;
    // Current chunk within the buffer
    u8* mp3ChunkAddr;
    int mp3ChunkSize;
    // Data within the chunk
    u8* mp3BufAddr;
    int mp3BufRemain;

    HMP3Decoder hMP3Decoder;

    s16 *mixBufferL;
    s16 *mixBufferR;
    s16 *musicBuf;

    int nMusicBuf, nMusicBufStart;
    int totalUsed;
} tMixerInfo;

// functions
void SoundLoopStep(void);

/* SoundState_* functions may be called from FIFO interrupt context. They should
 * only set the state of the mixer, then allow the SoundLoopStep to take action.
 */
void SoundState_Start(u8* aviBuffer, int aviBufLen, int aviBufPos, int aviRemain);
void SoundState_Play(void);
void SoundState_Pause(void);
void SoundState_Finishing(void);
void SoundState_Stop(void);
void SoundState_Volume(u32 volume);

void deinterleaveStereo(s16* streamL, s16* streamR, const u32* inBuffer, int smpCount);

#endif // SOUND7_H_
