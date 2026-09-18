#include "./smixi.h"

#ifdef __cplusplus
extern "C" {
#endif

void MIX_settimemult(int voice, int timemult) {
    MIXVOICE *pmv = &sndmix.voices[voice];

    if (pmv->ptimestretch != 0) {
        SFILTER_timestretchsetratio(pmv->ptimestretch, timemult);
    }
}

#ifdef __cplusplus
}
#endif
