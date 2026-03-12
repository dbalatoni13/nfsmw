#include "LZCompress.hpp"

static void ShortMove(unsigned char *pDest, unsigned char *pSrc, int Len) {
    for (int Run = 0; Run < Len; Run++) {
        pDest[Run] = pSrc[Run];
    }
}

unsigned int LZGetMaxCompressedSize(unsigned int source_data_size) {
    return source_data_size * 2 + 0x12;
}