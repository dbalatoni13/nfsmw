#include "LZCompress.hpp"

static void ShortMove(unsigned char *pDest, unsigned char *pSrc, int Len) {
    int Run = 0;
    do {
        pDest[Run] = pSrc[Run];
        Run++;
    } while (Run < Len);
}

unsigned int LZGetMaxCompressedSize(unsigned int source_data_size) {
    return source_data_size * 2 + 0x12;
}