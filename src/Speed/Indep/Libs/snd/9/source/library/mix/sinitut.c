#include "./smixi.h"

#ifdef __cplusplus
extern "C" {
#endif

void MIXI_initunpackmt() {
    sndmix.unpackinit[6] = SFILTER_unpackmtfinit;
    sndmix.unpackstatesize[6] = 0x2C;
    sndmix.unpackinit[8] = SFILTER_unpackmtpfinit;
    sndmix.unpackstatesize[8] = 0x3C;
    sndmix.unpackinit[7] = SFILTER_unpackmtlfinit;
    sndmix.unpackstatesize[7] = 0x30;
    sndmix.unpackinit[9] = SFILTER_unpackmtfinit;
    sndmix.unpackstatesize[9] = 0x2C;
    sndmix.unpackinit[11] = SFILTER_unpackmtpfinit;
    sndmix.unpackstatesize[11] = 0x3C;
    sndmix.unpackinit[10] = SFILTER_unpackmtlfinit;
    sndmix.unpackstatesize[10] = 0x30;
}

#ifdef __cplusplus
}
#endif
