#include "./smixi.h"

#ifdef __cplusplus
extern "C" {
#endif

void MIXI_initunpack16() {
    sndmix.unpackinit[0] = SFILTER_unpackfinit;
    sndmix.unpackstatesize[0] = 0x2C;
    sndmix.unpackinit[1] = SFILTER_unpacklfinit;
    sndmix.unpackstatesize[1] = 0x30;
    sndmix.unpackinit[2] = SFILTER_unpackpfinit;
    sndmix.unpackstatesize[2] = 0x34;
}

#ifdef __cplusplus
}
#endif
