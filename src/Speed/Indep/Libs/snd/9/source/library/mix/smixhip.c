#include "../cmn/sndcmn.h"
#include <snd/sfilter.h>
#include "smixi.h"

// total size: 0x3C (sfir8.c)
typedef struct SNDFIRSTATE {
    float history[8]; // offset 0x0, size 0x20
    float coef[5];    // offset 0x20, size 0x14
    float cutLow;     // offset 0x34
    float cutHigh;    // offset 0x38
} SNDFIRSTATE;

// total size: 0x58 (sfhpffir8.c)
typedef struct HPFSTATE {
    SFILTERNODE sfn; // offset 0x0, size 0x1C
    SNDFIRSTATE fir; // offset 0x1C, size 0x3C
} HPFSTATE;

int SFILTER_createHPFFIR8(HPFSTATE *phs);
void SFILTER_modifyHPFFIR8(HPFSTATE *phs, int *pparams);

extern "C" {
void SFILTER_add(SFILTERNODE **pphead, SFILTERNODE *pnew);
void SFILTER_remove(SFILTERNODE **pphead, SFILTERNODE *pcur);
}

extern "C" void MIX_sethighpass(int voice, int cutofffreq) {
    MIXVOICE *psmv;
    int params[2];

    psmv = &sndmix.voices[voice];
    if (cutofffreq > 0) {
        if (psmv->phpf == 0) {
            psmv->phpf = (HPFSTATE *)SNDMEMI_allocz(sizeof(HPFSTATE));
            psmv->phpf->sfn.restorefn = 0;
            psmv->phpf->sfn.priority = 0x50;
            psmv->phpf->sfn.requester = 0;
            SFILTER_createHPFFIR8(psmv->phpf);
            SFILTER_add(&psmv->pfilterhead, (SFILTERNODE *)psmv->phpf);
        }
        params[0] = cutofffreq << 8;
        params[1] = sndgs.sso.set.outputrate << 8;
        SFILTER_modifyHPFFIR8(psmv->phpf, params);
    } else {
        if (psmv->phpf != 0) {
            SFILTER_remove(&psmv->pfilterhead, (SFILTERNODE *)psmv->phpf);
            SNDMEMI_free(psmv->phpf);
            psmv->phpf = 0;
        }
    }
}
