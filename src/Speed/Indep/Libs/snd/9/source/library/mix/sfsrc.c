#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include <snd/sfilter.h>

// total size: 0x20
typedef struct SRCSTATE {
    SFILTERNODE sfn; // offset 0x0
    float *pdata;    // offset 0x1C
} SRCSTATE;

int SFILTER_src(void *pstate, int frames, void *psrcp, void *pdstp, int requester) {
    SRCSTATE *pss = (SRCSTATE *)pstate;

    Snd::Util::MemCpy(pdstp, pss->pdata, frames * sizeof(float));
    pss->pdata += frames;

    return frames;
}

void SFILTER_initSOURCE(SRCSTATE *psrcs, void *psrc) {
    psrcs->sfn.filterfn = SFILTER_src;
    psrcs->sfn.restorefn = 0;
    psrcs->pdata = (float *)psrc;
}

int SFILTER_createSOURCE(SRCSTATE *pnew) {
    pnew->sfn.pfnnext = 0;
    pnew->sfn.pfnnext2 = 0;
    pnew->sfn.pfnprev1 = 0;
    pnew->sfn.pfnprev2 = 0;
    pnew->sfn.clean = 0;
    pnew->pdata = 0;

    return 0;
}
