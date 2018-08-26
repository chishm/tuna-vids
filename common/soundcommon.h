#include <nds.h>
#include <stdio.h>

#ifndef __SOUNDCOMMON_H
#define __SOUNDCOMMON_H

//#define REPORT_ARM7_ERRORS
		
typedef void (*voidFuncPtr)();
		
// Commands the ARM7 can receive
typedef enum
{
	CMDFIFO7_MP3_PLAY,
	CMDFIFO7_MP3_SEEK,
	CMDFIFO7_MP3_PAUSE,
	CMDFIFO7_MP3_STOP,
    CMDFIFO7_MP3_VOLUME,
	CMDFIFO7_MP3_START,
	CMDFIFO7_BACKLIGHT_TOGGLE,
	
	CMDFIFO7_SIZE,
		
} cmdFifo7;
		
// Commands the ARM9 can receive
typedef enum
{
	CMDFIFO9_MP3_AMOUNTUSED,
	CMDFIFO9_MP3_END,
	CMDFIFO9_MP3_READY,
	CMDFIFO9_MP3_SAMPLES,
	
	CMDFIFO9_ERROR,
    
    CMDFIFO9_SIZE,
		
} cmdFifo9;
		
// Sound mixer state
typedef enum
{
	SNDMIXER_IDLE,
	SNDMIXER_SETUPSTREAM,
	SNDMIXER_STREAMING,
	SNDMIXER_PAUSE,
	SNDMIXER_STOP,
		
	SNDMIXER_SIZE,
		
} sndMixerState;
		
typedef enum
{
	SNDSTATE_IDLE,
	SNDSTATE_PLAY,
	SNDSTATE_ENDOFDATA,
	SNDSTATE_ERROR,
		
	SNDSTATE_SIZE,
		
} sndState;
		
#endif
