#include "../../../include/common/realcore/std.h"

void MEM_fill(void *dest, unsigned int val, int count) {
    unsigned int *desti;

    if (count > 0 && (reinterpret_cast<unsigned int>(dest) & 1) != 0) {
        *reinterpret_cast<unsigned char *>(dest) = val;
        count -= 1;
        dest = reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(dest) + 1);
    }
    if (count > 1 && (reinterpret_cast<unsigned int>(dest) & 2) != 0) {
        *reinterpret_cast<unsigned short *>(dest) = val;
        count -= 2;
        dest = reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(dest) + 2);
    }
    if (count > 3 && (reinterpret_cast<unsigned int>(dest) & 4) != 0) {
        *reinterpret_cast<unsigned int *>(dest) = val;
        count -= 4;
        dest = reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(dest) + 4);
    }
    if (count > 7 && (reinterpret_cast<unsigned int>(dest) & 8) != 0) {
        *reinterpret_cast<unsigned int *>(dest) = val;
        count -= 8;
        *(reinterpret_cast<unsigned int *>(dest) + 1) = val;
        dest = reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(dest) + 8);
    }
    if (count > 15 && (reinterpret_cast<unsigned int>(dest) & 16) != 0) {
        *reinterpret_cast<unsigned int *>(dest) = val;
        count -= 16;
        *(reinterpret_cast<unsigned int *>(dest) + 1) = val;
        *(reinterpret_cast<unsigned int *>(dest) + 2) = val;
        *(reinterpret_cast<unsigned int *>(dest) + 3) = val;
        dest = reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(dest) + 16);
    }

    desti = reinterpret_cast<unsigned int *>(dest);
    while (count > 31) {
        desti[0] = val;
        count -= 32;
        desti[1] = val;
        desti[2] = val;
        desti[3] = val;
        desti[4] = val;
        desti[5] = val;
        desti[6] = val;
        desti[7] = val;
        desti += 8;
    }
    if (count > 15) {
        desti[0] = val;
        count -= 16;
        desti[1] = val;
        desti[2] = val;
        desti[3] = val;
        desti += 4;
    }
    if (count > 7) {
        desti[0] = val;
        count -= 8;
        desti[1] = val;
        desti += 2;
    }
    if (count > 3) {
        desti[0] = val;
        count -= 4;
        desti += 1;
    }
    if (count > 1) {
        *reinterpret_cast<unsigned short *>(desti) = val;
        count -= 2;
        desti = reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(desti) + 2);
    }
    if (count > 0) {
        *reinterpret_cast<unsigned char *>(desti) = val;
    }
}
