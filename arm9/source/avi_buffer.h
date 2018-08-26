
#ifndef _AVI_BUFFER_H
#define _AVI_BUFFER_H

#include <nds.h>
#include <stdio.h>

#include "avi_headers.h"

#define AVI_BUFFER_SIZE 1048576	// 1 MiB
#define AVI_CHUNK_SIZE 32768	// 32 KiB
#define AVI_MIN_CHUNKS 2

#define MAX_KEYFRAME_SEARCH_ATTEMPTS 100
#define AVI_SEEK_OFFSET_FACTOR (1<<24)
#define AVI_START_CHUNK_TOLERANCE 20

typedef struct {
	u8*	start;
	int	size;
} AviDataChunk;

typedef struct {
	FILE*	aviFile;
	int		fileLen;
	int		moviOffset;
	int		moviSize;
	int		idx1Offset;
	int		idx1Size;
	int		indexOffset;
	u8*		bufferStart;
	int		moviRemain;
	u8*		videoReadStart;
	u8*		videoReadPointer;
	AviChunkHeader videoChunkHeader;
	int		videoAmountLeft;
	u8*		audioReadPointer;
	int		audioAmountLeft;
	u8*		writePointer;
	AviDataChunk	videoDataChunk;
} AviBuffer;

extern AviBuffer aviBuffer;

bool aviBufferInit (FILE* aviFile, int *frameWidth, int *frameHeight, int *frameRate, int *numFrames);
void aviBufferFree (void);

int aviBufferLoadChunk (void);
void aviBufferRefill (void);

void aviAudioUsedData (int amount);
AviDataChunk aviGetNextVideoChunk (void);
void aviVideoUsedAmount (int amount);
bool aviSeekChunk (int chunk);
bool aviSeekOffset (int offset);

#endif // _AVI_BUFFER_H
