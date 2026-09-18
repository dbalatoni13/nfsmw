#include "./smixi.h"

#ifdef __cplusplus
extern "C" {
#endif

void MIXI_initunpackxa() {
    sndmix.unpackinit[3] = SFILTER_unpackxafinit;
    sndmix.unpackstatesize[3] = 0x2C;
    sndmix.unpackstatesize[4] = 0x54;
    sndmix.unpackinit[4] = SFILTER_unpackxalfinit;
    sndmix.unpackinit[5] = SFILTER_unpackxapfinit;
    sndmix.unpackstatesize[5] = 0x3C;
}

#ifdef __cplusplus
}
#endif
