#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include <snd/sfilter.h>

// total size: 0x1C
typedef struct FT24_32STATE {
    SFILTERNODE sfn; // offset 0x0
} FT24_32STATE;

int SFILTER_ft24_32(void *pstate, int frames, void *psrcp, void *pdstp, int requester) {
    float *psrc = (float *)psrcp;
    int *pdst = (int *)pdstp;
    int i;

    for (i = 0; i < frames; i++) {
        pdst[i] = SNDI_clipint32((int)psrc[i], -32767, 32767);
    }

    return frames;
}

void SFILTER_ft24_32init(FT24_32STATE *ps) {
    ps->sfn.filterfn = SFILTER_ft24_32;
    ps->sfn.restorefn = 0;
}
