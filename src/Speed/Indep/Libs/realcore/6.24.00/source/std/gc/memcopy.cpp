#include "../../../include/common/realcore/std.h"

void MEM_copy(void *vdst, const void *vsrc, int size) {
    char *dst = (char *)vdst;
    const char *src = (const char *)vsrc;
    int diff = (int)src - (int)dst;

    if (size == 0) {
        return;
    }
    if (diff == 0) {
        return;
    }

    if ((diff & 1) != 0) {
        while (size > 3) {
            char b0 = src[0];
            char b1 = src[1];
            char b2 = src[2];
            char b3 = src[3];
            dst[0] = b0;
            dst[1] = b1;
            dst[2] = b2;
            dst[3] = b3;
            src += 4;
            dst += 4;
            size -= 4;
        }
        while (size > 0) {
            *dst = *src;
            dst++;
            src++;
            size--;
        }
        return;
    }

    if ((diff & 2) != 0) {
        const short *s = (const short *)src;
        short *d = (short *)dst;

        if (((int)src & 1) != 0 && size > 0) {
            *(char *)d = *(const char *)s;
            d = (short *)((char *)d + 1);
            s = (const short *)((const char *)s + 1);
            size -= 1;
        }
        while (size > 7) {
            int v0 = s[0];
            int v1 = s[1];
            int v2 = s[2];
            int v3 = s[3];
            d[0] = v0;
            d[1] = v1;
            d[2] = v2;
            d[3] = v3;
            d += 4;
            s += 4;
            size -= 8;
        }
        while (size > 1) {
            *(unsigned short *)d = *(unsigned short *)s;
            d++;
            s++;
            size -= 2;
        }
        if (size > 0) {
            *(char *)d = *(const char *)s;
        }
        return;
    }

    if (((int)src & 1) != 0 && size > 0) {
        *dst = *src;
        dst++;
        src++;
        size--;
    }
    if (((int)src & 2) != 0 && size > 1) {
        *(unsigned short *)dst = *(unsigned short *)src;
        dst += 2;
        src += 2;
        size -= 2;
    }
    while (size > 15) {
        int w0 = ((const int *)src)[0];
        int w1 = ((const int *)src)[1];
        int w2 = ((const int *)src)[2];
        int w3 = ((const int *)src)[3];
        ((int *)dst)[0] = w0;
        ((int *)dst)[1] = w1;
        ((int *)dst)[2] = w2;
        ((int *)dst)[3] = w3;
        dst += 16;
        src += 16;
        size -= 16;
    }
    while (size > 3) {
        *(int *)dst = *(const int *)src;
        dst += 4;
        src += 4;
        size -= 4;
    }
    if (size > 1) {
        *(unsigned short *)dst = *(unsigned short *)src;
        dst += 2;
        src += 2;
        size -= 2;
    }
    if (size > 0) {
        *dst = *src;
    }
}
