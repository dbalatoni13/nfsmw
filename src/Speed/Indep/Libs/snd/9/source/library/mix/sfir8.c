#include <snd/sfilter.h>

// total size: 0x3C
typedef struct SNDFIRSTATE {
    float history[8]; // offset 0x0, size 0x20
    float coef[5];    // offset 0x20, size 0x14
    float cutLow;     // offset 0x34
    float cutHigh;    // offset 0x38
} SNDFIRSTATE;

void SNDI_fir8init(SNDFIRSTATE *psfs) {
    psfs->history[0] = 0.0f;
    psfs->history[1] = 0.0f;
    psfs->history[2] = 0.0f;
    psfs->history[3] = 0.0f;
    psfs->history[4] = 0.0f;
    psfs->history[5] = 0.0f;
    psfs->history[6] = 0.0f;
    psfs->history[7] = 0.0f;
}

void SNDI_fir8(SNDFIRSTATE *pfir, int frames, void *psrcp, void *pdstp) {
    float *psrc;
    float *pdst;
    int i;

    psrc = (float *)psrcp;
    pdst = (float *)pdstp;
    for (i = 0; i < frames; i++) {
        pdst[i] = pfir->coef[0] * (psrc[i] + pfir->history[7]) +
                  pfir->coef[1] * (pfir->history[0] + pfir->history[6]) +
                  pfir->coef[2] * (pfir->history[1] + pfir->history[5]) +
                  pfir->coef[3] * (pfir->history[2] + pfir->history[4]) +
                  pfir->coef[4] * pfir->history[3];
        pfir->history[7] = pfir->history[6];
        pfir->history[6] = pfir->history[5];
        pfir->history[5] = pfir->history[4];
        pfir->history[4] = pfir->history[3];
        pfir->history[3] = pfir->history[2];
        pfir->history[2] = pfir->history[1];
        pfir->history[1] = pfir->history[0];
        pfir->history[0] = psrc[i];
    }
}
