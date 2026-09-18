#include <snd/sfilter.h>

// total size: 0x3C (sfir8.c)
typedef struct SNDFIRSTATE {
    float history[8]; // offset 0x0, size 0x20
    float coef[5];    // offset 0x20, size 0x14
    float cutLow;     // offset 0x34
    float cutHigh;    // offset 0x38
} SNDFIRSTATE;

// total size: 0x58
typedef struct BPFSTATE {
    SFILTERNODE sfn; // offset 0x0, size 0x1C
    SNDFIRSTATE fir; // offset 0x1C, size 0x3C
} BPFSTATE;

void SNDI_fir8init(SNDFIRSTATE *psfs);
void SNDI_fir8(SNDFIRSTATE *pfir, int frames, void *psrcp, void *pdstp);
void calcFIRCoeffs(SNDFIRSTATE *pfir, int filtertype);

int SFILTER_bpfFIR8(void *pstate, int outputframes, void *psrcp, void *pdstp, int requester) {
    BPFSTATE *pbs = (BPFSTATE *)pstate;
    int temp;

    if (pbs->sfn.pfnnext) {
        temp = pbs->sfn.pfnnext->filterfn(pbs->sfn.pfnnext, outputframes, pdstp, psrcp,
                                          pbs->sfn.requester);
        if (temp <= 0) {
            return temp;
        }
    } else {
        temp = outputframes;
    }
    SNDI_fir8(&pbs->fir, temp, psrcp, pdstp);
    return temp;
}

int SFILTER_createBPFFIR8(BPFSTATE *pbs) {
    pbs->sfn.pfnnext = 0; pbs->sfn.pfnnext2 = 0; pbs->sfn.pfnprev1 = 0; pbs->sfn.pfnprev2 = 0;
    pbs->sfn.clean = 0;
    pbs->sfn.restorefn = 0;
    pbs->sfn.filterfn = SFILTER_bpfFIR8;
    SNDI_fir8init(&pbs->fir);
    return 0;
}

void SFILTER_modifyBPFFIR8(BPFSTATE *pbs, int *pparams) {
    int cutLow;
    int cutHigh;
    int samplerate;

    cutLow = pparams[0] >> 7;
    cutHigh = pparams[1] >> 7;
    samplerate = pparams[2] >> 8;
    pbs->fir.cutLow = (float)cutLow / (float)samplerate;
    pbs->fir.cutHigh = (float)cutHigh / (float)samplerate;
    calcFIRCoeffs(&pbs->fir, 4);
}
