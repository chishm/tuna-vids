
#ifndef _BUFFER_H
#define _BUFFER_H

#include <nds.h>
#include <malloc.h>
#include <stdio.h>


/*****************************************************************************
 * Data buffering
 ****************************************************************************/
typedef struct {
	FILE*	file;
	int		fileLen;
	int		fileOffset;
	u8*		bufferStart;
	int		bufferSize;
	int		chunkSize;
	u8*		readPointer;
	u8*		writePointer;
	int		amountLeft;
} DATA_BUFFER;

bool bufferInit (DATA_BUFFER* buffer, FILE* file, int fileLen, int bufferSize, int chunkSize, bool avoidCache);
void bufferFree (DATA_BUFFER* buffer);

int bufferLoadChunk (DATA_BUFFER* buffer);
void bufferRefill (DATA_BUFFER* buffer);
void bufferAmountWasUsed (DATA_BUFFER* buffer, size_t amount);
void bufferSeek (DATA_BUFFER* buffer, int pos);

static inline int bufferSpaceToEnd (DATA_BUFFER* buffer) {
	if (buffer->readPointer < buffer->writePointer) {
		return buffer->amountLeft;
	} else if (buffer->writePointer < buffer->bufferStart + buffer->chunkSize) {
		return buffer->bufferStart + buffer->bufferSize - buffer->readPointer;
	} else {
		return buffer->bufferStart + buffer->bufferSize + buffer->chunkSize - buffer->readPointer;
	}
}

#endif // _BUFFER_H
