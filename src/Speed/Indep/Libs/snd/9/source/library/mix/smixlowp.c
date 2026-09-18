#include "../cmn/sndcmn.h"
#include <snd/sfilter.h>
#include "smixi.h"

// total size: 0xC (sflpf.c)
typedef struct LPFRC {
    float accumulator; // offset 0x0
    float leakage;     // offset 0x4
    float inverse;     // offset 0x8
} LPFRC;

// total size: 0x28 (sflpf.c)
typedef struct LPFRCSTATE {
    SFILTERNODE sfn; // offset 0x0, size 0x1C
    LPFRC lpf;       // offset 0x1C, size 0xC
} LPFRCSTATE;

int SFILTER_createLPFRC(LPFRCSTATE *pls);
void SFILTER_modifyLPFRC(LPFRCSTATE *pls, int *pparams);

extern "C" {
void SFILTER_add(SFILTERNODE **pphead, SFILTERNODE *pnew);
void SFILTER_remove(SFILTERNODE **pphead, SFILTERNODE *pcur);
}

extern "C" void MIX_setlowpass(int voice, float leakage) {
    MIXVOICE *psmv;
    int params[3];

    psmv = &sndmix.voices[voice];
    if (leakage < 1.0f) {
        if (psmv->plpf == 0) {
            psmv->plpf = (LPFRCSTATE *)SNDMEMI_allocz(sizeof(LPFRCSTATE));
            psmv->plpf->sfn.restorefn = 0;
            psmv->plpf->sfn.priority = 0x28;
            psmv->plpf->sfn.requester = 0;
            SFILTER_createLPFRC(psmv->plpf);
            SFILTER_add(&psmv->pfilterhead, (SFILTERNODE *)psmv->plpf);
        }
        params[1] = sndgs.sso.set.outputrate << 8;
        params[0] = SNDI_ftoiround(leakage * sndgs.sso.set.outputrate) << 7;
        params[2] = 0x100;
        SFILTER_modifyLPFRC(psmv->plpf, params);
    } else {
        if (psmv->plpf != 0) {
            SFILTER_remove(&psmv->pfilterhead, (SFILTERNODE *)psmv->plpf);
            SNDMEMI_free(psmv->plpf);
            psmv->plpf = 0;
        }
    }
}
