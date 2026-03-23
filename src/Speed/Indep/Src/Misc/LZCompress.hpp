#ifndef MISC_LZCOMPRESS_H
#define MISC_LZCOMPRESS_H

#include "types.h"

// total size: 0x10
class LZHeader {
  public:
    uint32 ID;               // offset 0x0, size 0x4
    uint8 Version;           // offset 0x4, size 0x1
    uint8 HeaderSize;        // offset 0x5, size 0x1
    uint16 Flags;            // offset 0x6, size 0x2
    uint32 UncompressedSize; // offset 0x8, size 0x4
    uint32 CompressedSize;   // offset 0xC, size 0x4
};

uint32 LZDecompress(uint8 *pSrc, uint8 *pDst);
int32 JLZDecompress(uint8 *pSrc, uint8 *pDst);
int32 JLZCompress(uint8 *pSrc, int32 SrcSize, uint8 *pDst);
uint32 OldLZDecompress(uint8 *pSrc, uint8 *pDst);
uint32 LZCompress(uint8 *pSrc, uint32 SrcSize, uint8 *pDst);
int32 LZValidHeader(LZHeader *header);
void LZByteSwapHeader(LZHeader *header);
uint32 LZGetMaxCompressedSize(uint32 source_data_size);
int32 RAWCompress(uint8 *pSrc, int32 SrcSize, uint8 *pDst);
int32 HUFFCompress(uint8 *pSrc, int32 SrcSize, uint8 *pDst);

#endif
