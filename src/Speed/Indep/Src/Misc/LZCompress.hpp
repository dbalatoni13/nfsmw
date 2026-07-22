//
//
//
//
//
//
//
//
#ifndef __LZCOMPRESS_HPP
#define __LZCOMPRESS_HPP // Decl: 10

#include "types.h"

#define RAWCOMP_ID (('R' << 0) | ('A' << 8) | ('W' << 16) | ('W' << 24)) // Decl: 17
#define RAWCOMP_VERSION 1                                                // Decl: 18

#define JLZCOMP_ID (('J' << 0) | ('D' << 8) | ('L' << 16) | ('Z' << 24)) // Decl: 20
#define JLZCOMP_VERSION 2                                                // Decl: 21

#define HUFFCOMP_ID (('H' << 0) | ('U' << 8) | ('F' << 16) | ('F' << 24)) // Decl: 23
#define HUFFCOMP_VERSION 1                                                // Decl: 24

#define LCZOMP_ID (('C' << 0) | ('O' << 8) | ('M' << 16) | ('P' << 24)) // Decl: 28
#define LZCOMP_VERSION 1                                                // Decl: 29

#define LZFLAG_COPY 0x0001 // Decl: 32

// total size: 0x10
// Decl: 41
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
