#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern "C" {
void bMemCpy(void *dest, const void *src, unsigned int numbytes) {
    char *pdest = (char *)dest;
    char *psrc = (char *)src;
    int size = numbytes;
    int alignment = (int)pdest | (int)psrc;
    
    if (!(alignment & 7)) {
        for (; size > 15; ) {
            size -= 16;
            unsigned long long t0 = ((unsigned long long*)psrc)[0];
            unsigned long long t1 = ((unsigned long long*)psrc)[1];
            ((unsigned long long*)pdest)[0] = t0;
            ((unsigned long long*)pdest)[1] = t1;
            psrc += 16;
            pdest += 16;
        }
    }
    if (!(alignment & 3)) {
        for (; size > 7; ) {
            size -= 8;
            unsigned int t0 = ((unsigned int*)psrc)[0];
            unsigned int t1 = ((unsigned int*)psrc)[1];
            ((unsigned int*)pdest)[0] = t0;
            ((unsigned int*)pdest)[1] = t1;
            psrc += 8;
            pdest += 8;
        }
    }
    for ( ; size; )
    {
        --size;
        unsigned char t0 = *psrc;
        *pdest = t0;
        ++psrc;
        ++pdest;
    }
}

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

void bMemSet(void *dest, unsigned char c, unsigned int numbytes) {
    unsigned char *cdest = reinterpret_cast<unsigned char *>(dest);
    unsigned int fill = c;
    fill |= fill << 8;
    fill |= fill << 16;

    if (((int)cdest & 3) == 0) {
        unsigned int align16 = (int)cdest & 0xf;
        if (align16 != 0) {
            while ((align16 != 0) && (numbytes >= 4)) {
                *reinterpret_cast<unsigned int *>(cdest) = fill;
                cdest += 4;
                numbytes -= 4;
                align16 = (int)cdest & 0xf;
            }
        }
    }

    if ((((int)cdest & 7) == 0) && (numbytes > 0x1f)) {
        do {
            reinterpret_cast<unsigned int *>(cdest)[0] = fill;
            reinterpret_cast<unsigned int *>(cdest)[1] = fill;
            reinterpret_cast<unsigned int *>(cdest)[2] = fill;
            reinterpret_cast<unsigned int *>(cdest)[3] = fill;
            reinterpret_cast<unsigned int *>(cdest)[4] = fill;
            reinterpret_cast<unsigned int *>(cdest)[5] = fill;
            reinterpret_cast<unsigned int *>(cdest)[6] = fill;
            reinterpret_cast<unsigned int *>(cdest)[7] = fill;
            cdest += 0x20;
            numbytes -= 0x20;
        } while (numbytes > 0x1f);
    }

    if ((((int)cdest & 3) == 0) && (numbytes > 0xf)) {
        do {
            reinterpret_cast<unsigned int *>(cdest)[0] = fill;
            reinterpret_cast<unsigned int *>(cdest)[1] = fill;
            reinterpret_cast<unsigned int *>(cdest)[2] = fill;
            reinterpret_cast<unsigned int *>(cdest)[3] = fill;
            cdest += 0x10;
            numbytes -= 0x10;
        } while (numbytes > 0xf);
    }

    if (((int)cdest & 3) == 0) {
        while (numbytes > 7) {
            reinterpret_cast<unsigned int *>(cdest)[0] = fill;
            reinterpret_cast<unsigned int *>(cdest)[1] = fill;
            numbytes -= 8;
            cdest += 8;
        }
    }

    while (numbytes != 0) {
        *cdest = c;
        cdest++;
        numbytes--;
    }
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
