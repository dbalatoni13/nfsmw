#include "../../../include/common/realcore/std.h"

void MEM_fill(void *vdst, unsigned int fill, int size) {
    char *d = (char *)vdst;

    if (size > 0 && ((unsigned int)d & 1) != 0) {
        *d = (char)fill;
        ++d;
        --size;
    }
    if (size > 1 && ((unsigned int)d & 2) != 0) {
        *(unsigned short *)d = (unsigned short)fill;
        d += 2;
        size -= 2;
    }
    if (size > 3 && ((unsigned int)d & 4) != 0) {
        *(unsigned int *)d = fill;
        d += 4;
        size -= 4;
    }
    if (size > 7 && ((unsigned int)d & 8) != 0) {
        *(unsigned int *)d = fill;
        ((unsigned int *)d)[1] = fill;
        d += 8;
        size -= 8;
    }
    if (size > 15 && ((unsigned int)d & 16) != 0) {
        *(unsigned int *)d = fill;
        ((unsigned int *)d)[1] = fill;
        ((unsigned int *)d)[2] = fill;
        ((unsigned int *)d)[3] = fill;
        d += 16;
        size -= 16;
    }
    while (size > 31) {
        ((unsigned int *)d)[0] = fill;
        ((unsigned int *)d)[1] = fill;
        ((unsigned int *)d)[2] = fill;
        ((unsigned int *)d)[3] = fill;
        ((unsigned int *)d)[4] = fill;
        ((unsigned int *)d)[5] = fill;
        ((unsigned int *)d)[6] = fill;
        ((unsigned int *)d)[7] = fill;
        d += 32;
        size -= 32;
    }
    if (size > 15) {
        ((unsigned int *)d)[0] = fill;
        ((unsigned int *)d)[1] = fill;
        ((unsigned int *)d)[2] = fill;
        ((unsigned int *)d)[3] = fill;
        d += 16;
        size -= 16;
    }
    if (size > 7) {
        ((unsigned int *)d)[0] = fill;
        ((unsigned int *)d)[1] = fill;
        d += 8;
        size -= 8;
    }
    if (size > 3) {
        *(unsigned int *)d = fill;
        d += 4;
        size -= 4;
    }
    if (size > 1) {
        *(unsigned short *)d = (unsigned short)fill;
        d += 2;
        size -= 2;
    }
    if (size > 0) {
        *d = (char)fill;
    }
}
