#ifndef COMMON_AVI_HEADERS_H_
#define COMMON_AVI_HEADERS_H_

#include <nds/ndstypes.h>

#define FOUR_CC_DATA(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))

typedef struct
{
    u32 riffID;
    u32 size;
    u32 fourcc;
} RiffHeader;

#define RIFF_ID FOUR_CC_DATA('R','I','F','F')
#define RIFF_AVI_FOURCC FOUR_CC_DATA('A','V','I',' ')

typedef struct
{
    u32 listID;
    u32 size;
    u32 fourcc;
} AviListHeader;

#define LIST_ID FOUR_CC_DATA('L','I','S','T')
#define JUNK_ID FOUR_CC_DATA('J','U','N','K')

enum LIST_FOURCCS
{
    LIST_hdrl = FOUR_CC_DATA('h', 'd', 'r', 'l'),   // Header list
    LIST_strl = FOUR_CC_DATA('s', 't', 'r', 'l'),   // Stream list
    LIST_movi = FOUR_CC_DATA('m', 'o', 'v', 'i'),   // Movie list
    LIST_rec = FOUR_CC_DATA('r', 'e', 'c', ' '),    // Record list
};

typedef struct
{
    u32 fourcc;
    u32 size;
} AviChunkHeader;

enum CHUNK_FOURCCS
{
    CHUNK_avih = FOUR_CC_DATA('a', 'v', 'i', 'h'),  // AVI header
    CHUNK_strh = FOUR_CC_DATA('s', 't', 'r', 'h'),  // Stream header
    CHUNK_strf = FOUR_CC_DATA('s', 't', 'r', 'f'),  // Stream format
    CHUNK_JUNK = FOUR_CC_DATA('J', 'U', 'N', 'K'),  // Junk (padding)
    CHUNK_idx1 = FOUR_CC_DATA('i', 'd', 'x', '1'),  // Index of offsets into movie stream
    CHUNK_strd = FOUR_CC_DATA('s', 't', 'r', 'd'),  // Stream description
    CHUNK_strn = FOUR_CC_DATA('s', 't', 'r', 'n'),  // Stream name
    /* The following aren't guaranteed to be correct in the AVI file, but should
     * be okay for nearly all normal AVIs */
    CHUNK_audio = FOUR_CC_DATA('0', '1', 'w', 'b'),
    CHUNK_video = FOUR_CC_DATA('0', '0', 'd', 'c'),
    CHUNK_bmp = FOUR_CC_DATA('0', '0', 'd', 'b'),
};

typedef struct
{
    u32 fourcc;
    u32 size;
    u32 microSecPerFrame;
    u32 maxBytesPerSec;
    u32 paddingGranularity;
    u32 flags;
    u32 totalFrames;
    u32 initialFrames;
    u32 streams;
    u32 suggestedBufferSize;
    u32 width;
    u32 height;
    u32 reserved[4];
} AviMainHeader;

typedef struct
{
    u32 fourcc;
    u32 handler;
    u32 flags;
    u16 priority;
    u16 language;
    u32 initialFrames;
    u32 scale;
    u32 rate;
    u32 start;
    u32 length;
    u32 suggestedBufferSize;
    u32 quality;
    u32 sampleSize;
    struct
    {
        s16 left;
        s16 top;
        s16 right;
        s16 bottom;
    } rcFrame;
} AviStreamHeader;

enum STREAM_FOURCCS
{
    STREAM_auds = FOUR_CC_DATA('a', 'u', 'd', 's'), // audio stream
    STREAM_mids = FOUR_CC_DATA('m', 'i', 'd', 's'), // midi stream
    STREAM_txts = FOUR_CC_DATA('t', 'x', 't', 's'), // text stream
    STREAM_vids = FOUR_CC_DATA('v', 'i', 'd', 's'), // video stream
};

#define XVID_HANDLER_ID FOUR_CC_DATA('x','v','i','d')

typedef struct
{
    u32 fourcc;
    u32 flags;
    u32 offset;
    u32 size;
} AviIndexEntry;

#define AVIIF_KEYFRAME	0x00000010  // This is a key frame

#endif // COMMON_AVI_HEADERS_H_
