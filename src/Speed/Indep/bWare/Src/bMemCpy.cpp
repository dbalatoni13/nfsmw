#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern "C" {
int bMemCmp(const void *s1, const void *s2, unsigned int numbytes) {
    if (numbytes == 0) {
        return 0;
    }
    while (numbytes--, numbytes != 0 && *(unsigned char *)s1 == *(unsigned char *)s2) {
        s1 = reinterpret_cast<void *>(reinterpret_cast<unsigned int>(s1) + 1);
        s2 = reinterpret_cast<void *>(reinterpret_cast<unsigned int>(s2) + 1);
    }
    return *reinterpret_cast<const unsigned char *>(s1) - *reinterpret_cast<const unsigned char *>(s2);
}

void bOverlappedMemCpy(void *dest, const void *src, unsigned int numbytes) {
    char *cdest = reinterpret_cast<char *>(dest);
    const char *csrc = reinterpret_cast<const char *>(src);
    int overlap_amount = cdest - csrc;

    if ((overlap_amount < 1) || (overlap_amount >= (int)numbytes)) {
        bMemCpy(dest, src, numbytes);
    } else {
        int pos = numbytes;
        if (pos >= overlap_amount) {
            do {
                pos -= overlap_amount;
                bMemCpy(cdest + pos, csrc + pos, overlap_amount);
            } while (pos >= overlap_amount);
        }
        bMemCpy(dest, src, pos);
    }
}
}
