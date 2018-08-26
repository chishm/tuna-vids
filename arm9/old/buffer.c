
#include "buffer.h"
#include <string.h>

int bufferLoadChunk (DATA_BUFFER* buffer) {
	if (!buffer->bufferStart) {
		return 0;
	}
	int len;
	int readLen;
	if (buffer->amountLeft > 0 && 
		buffer->writePointer <= buffer->readPointer && buffer->readPointer < buffer->writePointer + buffer->chunkSize)
	{
		// Buffer full
		return 0;
	}
	readLen = 	(buffer->chunkSize < buffer->fileLen - buffer->fileOffset) ? 
				 buffer->chunkSize : buffer->fileLen - buffer->fileOffset;
	len = fread (buffer->writePointer, 1, readLen, buffer->file);
	if (len <= 0) {
		return 0;
	}
	if (buffer->writePointer < buffer->bufferStart + buffer->chunkSize) {
		memcpy (buffer->bufferStart + buffer->bufferSize, buffer->bufferStart, buffer->chunkSize);
	}
	buffer->writePointer += len;
	if (buffer->writePointer >= buffer->bufferStart + buffer->bufferSize) {
		buffer->writePointer = buffer->bufferStart;
	}
	buffer->amountLeft += len;
	buffer->fileOffset += len;
	return len;
}

void bufferAmountWasUsed (DATA_BUFFER* buffer, size_t amount) {
	if (!buffer->bufferStart) {
		return;
	}
	buffer->readPointer += amount;
	while (buffer->readPointer >= buffer->bufferStart + buffer->bufferSize) {
		buffer->readPointer -= buffer->bufferSize;
	}
	buffer->amountLeft -= amount;
	if (buffer->amountLeft < 2 * buffer->chunkSize) {
		bufferLoadChunk (buffer);
	}
}

void bufferRefill (DATA_BUFFER* buffer) {
	while (buffer->amountLeft < buffer->bufferSize && bufferLoadChunk(buffer) > 0);
}

void bufferSeek (DATA_BUFFER* buffer, int pos) {
	fseek (buffer->file, pos - buffer->fileOffset, SEEK_CUR);
	buffer->fileOffset = pos;
	buffer->readPointer = buffer->writePointer = buffer->bufferStart;
	buffer->amountLeft = 0;
	bufferRefill (buffer);
}

bool bufferInit (DATA_BUFFER* buffer, FILE* file, int fileLen, int bufferSize, int chunkSize, bool avoidCache) {
	buffer->file = file;
	buffer->fileLen = fileLen;
	buffer->fileOffset = 0;
	buffer->bufferSize = bufferSize;
	buffer->chunkSize = chunkSize;
	buffer->bufferStart = (u8*) malloc(bufferSize + chunkSize);
	if (!buffer->bufferStart) {
		return false;
	}
	if (avoidCache) {
		buffer->bufferStart = (u8*) ((int)(buffer->bufferStart) | 0x00400000);
	}
	buffer->readPointer = buffer->writePointer = buffer->bufferStart;
	buffer->amountLeft = 0;
	bufferRefill (buffer);
	return true;
}

void bufferFree (DATA_BUFFER* buffer) {
	if (buffer->bufferStart) {
		buffer->bufferStart = (u8*) ((int)(buffer->bufferStart) & ~0x00400000);
		free (buffer->bufferStart);
		buffer->bufferStart = 0;
	}
}
