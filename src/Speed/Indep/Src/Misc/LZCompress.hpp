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

int32 LZDecompress(uint8 *pSrc, uint8 *pDst);
