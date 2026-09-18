#include "./sndcmn.h"

int randrange(int range) {
    if (range > 0x10000) {
        range = 0x10000;
    } else if (range < 0) {
        range = 0;
    }

    return (int)((iSNDrandom() & 0x7FFF) - 0x4000) * range >> 14;
}
