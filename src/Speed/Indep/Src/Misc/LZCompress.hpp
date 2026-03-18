#ifndef _LZCOMPRESS
#define _LZCOMPRESS

#include "types.h"

// total size: 0x10
class LZHeader {
  public:
    unsigned int ID;               // offset 0x0, size 0x4
    unsigned char Version;         // offset 0x4, size 0x1
    unsigned char HeaderSize;      // offset 0x5, size 0x1
    unsigned short Flags;          // offset 0x6, size 0x2
    unsigned int UncompressedSize; // offset 0x8, size 0x4
    unsigned int CompressedSize;   // offset 0xC, size 0x4
};

unsigned int LZDecompress(uint8 *pSrc, uint8 *pDst);
int JLZDecompress(unsigned char *pSrc, unsigned char *pDst);
int JLZCompress(unsigned char *pSrc, int SrcSize, unsigned char *pDst);
int OldLZDecompress(unsigned char *pSrc, unsigned char *pDst);
int LZCompress(unsigned char *pSrc, unsigned int SrcSize, unsigned char *pDst);
int LZValidHeader(LZHeader *header);
void LZByteSwapHeader(LZHeader *header);
unsigned int LZGetMaxCompressedSize(unsigned int source_data_size);
int RAWCompress(unsigned char *pSrc, int SrcSize, unsigned char *pDst);
int HUFFCompress(unsigned char *pSrc, int SrcSize, unsigned char *pDst);

#endif
