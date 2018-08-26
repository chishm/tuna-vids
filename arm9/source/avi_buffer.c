
#include "avi_buffer.h"

#include <string.h>
#include <malloc.h>

//#define UNCACHED_BUFFER
/*
When using the data buffer, load data into UNCACHED memory, 
then invalidate the cache for the loaded area. This will keep 
the data valid for the ARM7 and simply force the ARM9 to take a 
cache miss when it first accesses the data.
Alternatively, use CACHED memory as normal, and flush it after
data is read from disc, to update the ARM7's audio data stream.
*/

AviBuffer aviBuffer = {0};


int aviBufferLoadChunk (void) {
	if (!aviBuffer.bufferStart) {
		return 0;
	}
	
	int len;
	
	int writeOffset = aviBuffer.writePointer - aviBuffer.bufferStart;
	int audioReadOffset = aviBuffer.audioReadPointer - aviBuffer.bufferStart;
	int videoReadOffset = aviBuffer.videoReadPointer - aviBuffer.videoReadStart;
	if ((aviBuffer.videoAmountLeft > 0 && writeOffset <= videoReadOffset && videoReadOffset < writeOffset + AVI_CHUNK_SIZE) ||
		(aviBuffer.audioAmountLeft > 0 && writeOffset <= audioReadOffset && audioReadOffset < writeOffset + AVI_CHUNK_SIZE))
	{
		// Buffer full
		return 0;
	}
	len = AVI_CHUNK_SIZE < (aviBuffer.moviSize - aviBuffer.moviOffset) ? 
		AVI_CHUNK_SIZE : (aviBuffer.moviSize - aviBuffer.moviOffset);
	len = fread (aviBuffer.writePointer, 1, len, aviBuffer.aviFile);
	if (len <= 0) {
		return 0;
	}
	if (aviBuffer.writePointer < aviBuffer.bufferStart + AVI_CHUNK_SIZE) {
		memcpy (aviBuffer.bufferStart + AVI_BUFFER_SIZE, aviBuffer.bufferStart, AVI_CHUNK_SIZE);
#ifdef UNCACHED_BUFFER
		DC_InvalidateRange (aviBuffer.videoReadStart + AVI_BUFFER_SIZE, len); 
#else
		DC_FlushRange (aviBuffer.videoReadStart + AVI_BUFFER_SIZE, len); 
#endif
	}
	// Invalidate data cache for loaded chunk, so that the ARM9 will load the correct data
	// The data is always read to uncached memory, for the benefit of the ARM7
#ifdef UNCACHED_BUFFER
	DC_InvalidateRange (aviBuffer.videoReadStart + writeOffset, len);
#else
	DC_FlushRange (aviBuffer.videoReadStart + writeOffset, len);
#endif
	
	// Wrap the write pointer back to the beginning of the buffer if needed
	aviBuffer.writePointer += len;
	if (aviBuffer.writePointer >= aviBuffer.bufferStart + AVI_BUFFER_SIZE) {
		aviBuffer.writePointer -= AVI_BUFFER_SIZE;
	}
	aviBuffer.videoAmountLeft += len;
	aviBuffer.audioAmountLeft += len;
	
	return len;
}

void aviBufferRefill (void) {
	while (aviBuffer.videoAmountLeft < AVI_BUFFER_SIZE && aviBufferLoadChunk() > 0);
}

AviDataChunk aviGetNextVideoChunk (void) {
	return aviBuffer.videoDataChunk;
}

void aviVideoUsedAmount (int amount) {
	aviBuffer.videoDataChunk.start += amount;
	aviBuffer.videoDataChunk.size -= amount;

	if (aviBuffer.videoDataChunk.size <= 0 || amount == 0) {
		// Scan forward in AVI buffer for next video data chunk
		do {
			aviBuffer.videoReadPointer += aviBuffer.videoChunkHeader.size;
			aviBuffer.videoAmountLeft -= aviBuffer.videoChunkHeader.size;
			aviBuffer.moviRemain -= aviBuffer.videoChunkHeader.size;
			if (aviBuffer.videoReadPointer >= aviBuffer.videoReadStart + AVI_BUFFER_SIZE) {
				aviBuffer.videoReadPointer -= AVI_BUFFER_SIZE;
			}
			// Ensure there is enough data in the buffer
			while (aviBuffer.videoAmountLeft < AVI_MIN_CHUNKS * AVI_CHUNK_SIZE && aviBufferLoadChunk() > 0);
			memcpy (&aviBuffer.videoChunkHeader, aviBuffer.videoReadPointer, sizeof (AviChunkHeader));
			// Lists contain chunks, but we're only interested in the chunks inside, so ignore list headers
			if (aviBuffer.videoChunkHeader.fourcc == LIST_ID) {
				aviBuffer.videoChunkHeader.size = sizeof(AviListHeader)-sizeof(AviChunkHeader);
			}
			aviBuffer.videoDataChunk.start = aviBuffer.videoReadPointer + sizeof(AviChunkHeader);
			aviBuffer.videoDataChunk.size = aviBuffer.videoChunkHeader.size;
			aviBuffer.videoChunkHeader.size = ((aviBuffer.videoChunkHeader.size+1)&~1) + sizeof(AviChunkHeader); 			
		} while (
			(	(aviBuffer.videoChunkHeader.fourcc != CHUNK_video && aviBuffer.videoChunkHeader.fourcc != CHUNK_bmp) 
				|| aviBuffer.videoDataChunk.size == 0)	/* skip chunks that don't contain any data */
			&& aviBuffer.videoAmountLeft > 0 && aviBuffer.moviRemain > 0);
		
		if (aviBuffer.videoChunkHeader.fourcc != CHUNK_video && aviBuffer.videoChunkHeader.fourcc != CHUNK_bmp) {
			aviBuffer.videoDataChunk.size = 0;
		}
	}
}

void aviAudioUsedData (int amount) {
	aviBuffer.audioAmountLeft -= amount;
	aviBuffer.audioReadPointer += amount;
	if (aviBuffer.audioReadPointer >= aviBuffer.bufferStart + AVI_BUFFER_SIZE) {
		aviBuffer.audioReadPointer -= AVI_BUFFER_SIZE;
	}
	while (aviBuffer.audioAmountLeft < AVI_MIN_CHUNKS * AVI_CHUNK_SIZE && aviBufferLoadChunk() > 0);
}

bool aviSeekChunk (int chunk) {
	AviIndexEntry aviIndexEntry;
	int chunkOffset;
	bool foundKeyframe;
	int i;

	if (!aviBuffer.idx1Offset || !aviBuffer.aviFile) {
		return false;
	}
	
	if (chunk * sizeof(AviIndexEntry) >= aviBuffer.idx1Size) {
		return false;
	}
	
	// Reset the buffer status
	aviBuffer.videoReadPointer = aviBuffer.videoDataChunk.start = aviBuffer.videoReadStart;
	aviBuffer.videoAmountLeft = aviBuffer.audioAmountLeft = 0;
	aviBuffer.audioReadPointer = aviBuffer.writePointer = aviBuffer.bufferStart;

	// Get index entry for chunk
	fseek (aviBuffer.aviFile, aviBuffer.idx1Offset + chunk * sizeof(AviIndexEntry), SEEK_SET);
	// Try to find a key frame
	foundKeyframe = false;
	if (chunk > AVI_START_CHUNK_TOLERANCE) {
		for (i = 0; i < MAX_KEYFRAME_SEARCH_ATTEMPTS && !foundKeyframe; i++) {
			fread (&aviIndexEntry, sizeof(AviIndexEntry), 1, aviBuffer.aviFile);
			foundKeyframe = (aviIndexEntry.flags & AVIIF_KEYFRAME) && (aviIndexEntry.fourcc == CHUNK_video);
		}
	}
	if (!foundKeyframe) {
		// Just use what was found
		fseek (aviBuffer.aviFile, aviBuffer.idx1Offset + chunk * sizeof(AviIndexEntry), SEEK_SET);
		fread (&aviIndexEntry, sizeof(AviIndexEntry), 1, aviBuffer.aviFile);
	}
	// Calculate file offset
	chunkOffset = aviBuffer.indexOffset + aviIndexEntry.offset;
	// Jump to chunk and refill buffer
	fseek (aviBuffer.aviFile, chunkOffset, SEEK_SET);
	aviBuffer.moviRemain = aviBuffer.moviSize - (chunkOffset - aviBuffer.moviOffset);
	aviBufferRefill ();
	// Seek to start point for video
	aviBuffer.videoChunkHeader.size = 0;
	aviBuffer.videoDataChunk.size = 0;
	aviVideoUsedAmount (0);

	return true;
}

bool aviSeekOffset (int offset) {
	int64_t chunk;
	
	chunk = (((int64_t)aviBuffer.idx1Size/sizeof(AviIndexEntry)) * (int64_t)offset) / (int64_t)AVI_SEEK_OFFSET_FACTOR;

	return aviSeekChunk ((int)chunk);
}

bool aviBufferInit (FILE* aviFile, int *frameWidth, int *frameHeight, int *frameRate, int *numFrames) {
	RiffHeader riffHeader;
	AviListHeader aviListHeader;
	AviMainHeader aviMainHeader;
	AviStreamHeader aviStreamHeader;
	AviChunkHeader aviChunkHeader;
	AviIndexEntry aviIndexEntry;
	
	int aviMoviListStart;
	int nextListStart;
	
	aviBuffer.aviFile = aviFile;
	fseek (aviBuffer.aviFile, 0, SEEK_END);
	aviBuffer.fileLen = ftell (aviBuffer.aviFile);
	fseek (aviBuffer.aviFile, 0, SEEK_SET);
	
	if (aviBuffer.fileLen <= 0) {
		return false;
	}
	
	// Read in AVI file info
	// RIFF AVI header
	fread (&riffHeader, sizeof(RiffHeader), 1, aviBuffer.aviFile);
	if (riffHeader.riffID != RIFF_ID || riffHeader.fourcc != RIFF_AVI_FOURCC) return false;
	
	// Initial AVI header
	fread (&aviListHeader, sizeof(AviListHeader), 1, aviBuffer.aviFile);
	if (aviListHeader.listID != LIST_ID || aviListHeader.fourcc != LIST_hdrl) return false;
	// Calculate where the actual data starts within the file
	aviMoviListStart = ftell (aviBuffer.aviFile) + ((aviListHeader.size + 1) & ~1) - (sizeof(AviListHeader) - 8);
	
	// Main AVI header, gives info on frame size
	fread (&aviMainHeader, sizeof(AviMainHeader), 1, aviBuffer.aviFile);
	if (aviMainHeader.fourcc != CHUNK_avih) return false;
	fseek (aviBuffer.aviFile, ((aviMainHeader.size + 1) & ~1) - (sizeof(AviMainHeader) - 8), SEEK_CUR);
	if (frameWidth)
		*frameWidth = aviMainHeader.width;
	if (frameHeight)
		*frameHeight = aviMainHeader.height;
	
	// First stream list -- must be video stream!
	fread (&aviListHeader, sizeof(AviListHeader), 1, aviBuffer.aviFile);
	if (aviListHeader.listID != LIST_ID || aviListHeader.fourcc != LIST_strl) return false;
	nextListStart = ftell (aviBuffer.aviFile) + ((aviListHeader.size + 1) & ~1) - (sizeof(AviListHeader) - 8);
	fread (&aviChunkHeader, sizeof(AviChunkHeader), 1, aviBuffer.aviFile);
	if (aviChunkHeader.fourcc != CHUNK_strh) return false;
	fread (&aviStreamHeader, sizeof(AviStreamHeader), 1, aviBuffer.aviFile);
	if (aviStreamHeader.fourcc != STREAM_vids) return false;
	// Calculate frame rate in centihertz
	if (frameRate)
		*frameRate = aviStreamHeader.rate * 100 / aviStreamHeader.scale;
	if (numFrames)
		*numFrames = aviStreamHeader.length;
	
	// Second stream list -- must be audio stream!
	fseek (aviBuffer.aviFile, nextListStart, SEEK_SET);
	fread (&aviListHeader, sizeof(AviListHeader), 1, aviBuffer.aviFile);
	if (aviListHeader.listID != LIST_ID || aviListHeader.fourcc != LIST_strl) return false;
	nextListStart = ftell (aviBuffer.aviFile) + ((aviListHeader.size + 1) & ~1) - (sizeof(AviListHeader) - 8);
	fread (&aviChunkHeader, sizeof(AviChunkHeader), 1, aviBuffer.aviFile);
	if (aviChunkHeader.fourcc != CHUNK_strh) return false;
	fread (&aviStreamHeader, sizeof(AviStreamHeader), 1, aviBuffer.aviFile);
	if (aviStreamHeader.fourcc != STREAM_auds) return false;

	// Get information about movie data
	do {
		fseek (aviBuffer.aviFile, aviMoviListStart, SEEK_SET);
		fread (&aviListHeader, sizeof(AviListHeader), 1, aviBuffer.aviFile);
		if (aviListHeader.fourcc != LIST_movi || aviListHeader.listID != LIST_ID) {
			aviMoviListStart = ftell (aviBuffer.aviFile) + ((aviListHeader.size + 1) & ~1) - (sizeof(AviListHeader) - 8);
		}
	} while (aviListHeader.fourcc != LIST_movi && !feof(aviBuffer.aviFile) &&
		(aviListHeader.listID == LIST_ID || aviListHeader.listID == JUNK_ID));

	if (aviListHeader.fourcc != LIST_movi) 	return false;
	
	aviBuffer.moviOffset = aviMoviListStart + sizeof (AviListHeader);
	aviBuffer.moviSize = ((aviListHeader.size + 1) & ~1) - (sizeof(AviListHeader) - 8);

	// Find index
	fseek (aviBuffer.aviFile, aviBuffer.moviSize, SEEK_CUR);
	fread (&aviChunkHeader, sizeof(AviChunkHeader), 1, aviBuffer.aviFile);
	if (feof(aviBuffer.aviFile)) {
		aviBuffer.idx1Offset = 0;
	} else if (aviChunkHeader.fourcc != CHUNK_idx1) {
		aviBuffer.idx1Offset = 0;
	} else {
		aviBuffer.idx1Offset = ftell (aviBuffer.aviFile);
		aviBuffer.idx1Size = ((aviChunkHeader.size + 1) & ~1) - (sizeof(AviListHeader) - 8);
		fread (&aviIndexEntry, sizeof(AviIndexEntry), 1, aviBuffer.aviFile);
		aviBuffer.indexOffset = aviBuffer.moviOffset - aviIndexEntry.offset;
	}

	// Allocate memory for buffer
	aviBuffer.bufferStart = (u8*) calloc(AVI_BUFFER_SIZE + AVI_CHUNK_SIZE, 1);
	if (!aviBuffer.bufferStart) return false;
	aviBuffer.videoReadPointer = aviBuffer.videoReadStart = aviBuffer.bufferStart;

#ifdef UNCACHED_BUFFER
	// Use uncached RAM for buffer
	aviBuffer.bufferStart = (u8*) ((int)(aviBuffer.bufferStart) | 0x00400000);
#endif
	aviBuffer.writePointer = aviBuffer.bufferStart;
	aviBuffer.audioReadPointer = aviBuffer.bufferStart;
	
	// Jump back to movie stream, ready to play
	fseek (aviBuffer.aviFile, aviBuffer.moviOffset, SEEK_SET);

	// Fill AVI buffer so we have some data to work with
	aviBuffer.videoAmountLeft = aviBuffer.audioAmountLeft = 0;
	aviBuffer.moviRemain = aviBuffer.moviSize;
	aviBufferRefill ();

	// Seek to start point for video
	aviBuffer.videoChunkHeader.size = 0;
	aviBuffer.videoDataChunk.size = 0;
	aviVideoUsedAmount (0);
	
	return true;
}

void aviBufferFree (void) {
	if (!aviBuffer.bufferStart) {
		return;
	}
	free (aviBuffer.bufferStart);
	aviBuffer.bufferStart = NULL;
}

