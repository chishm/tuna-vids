#include "bios_decompress_callback.h"

int getSizeBiosCallback(uint8 * source, uint16 * dest, uint32 r2)
{
    return *((int*) source);
}

uint8 readByteBiosCallback(uint8 * source)
{
    return *source;
}

TDecompressionStream decompressBiosCallback =
{
    getSizeBiosCallback,
    (void*) 0,
    readByteBiosCallback
};
