#include "../cmn/sndcmn.h"
#include <snd/sfilter.h>
#include "smixi.h"

// total size: 0x3C (sfrsf.c)
typedef struct RSFSTATE {
    SFILTERNODE sfn; // offset 0x0, size 0x1C
    char pad0[0x3C - 0x1C];
} RSFSTATE;

void SFILTER_rsfinit(RSFSTATE *prs, int cputypes, int method);
void SFILTER_rsfsetpitch(RSFSTATE *prs, int pitchmult);

extern "C" {
void SFILTER_add(SFILTERNODE **pphead, SFILTERNODE *pnew);
}

extern "C" void MIX_setpitch(int voice, int pitchmult) {
    MIXVOICE *psmv;
    int method;

    psmv = &sndmix.voices[voice];
    method = 0;
    if (pitchmult > 0x40000) {
        pitchmult = 0x40000;
    }
    if (psmv->prsf == 0) {
        psmv->prsf = (RSFSTATE *)SNDMEMI_allocz(sizeof(RSFSTATE));
        if (sndmix.platformver > 0x31) {
            method = 1;
        }
        psmv->prsf->sfn.restorefn = 0;
        psmv->prsf->sfn.priority = 0xa0;
        psmv->prsf->sfn.requester = 0;
        SFILTER_rsfinit(psmv->prsf, sndgs.cputypes, method);
        SFILTER_add(&psmv->pfilterhead, (SFILTERNODE *)psmv->prsf);
    }
    SFILTER_rsfsetpitch(psmv->prsf, pitchmult);
}
