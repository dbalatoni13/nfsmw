#include <snd/sfilter.h>

// total size: 0xC
typedef struct LPFRC {
    float accumulator; // offset 0x0
    float leakage;     // offset 0x4
    float inverse;     // offset 0x8
} LPFRC;

// total size: 0x28
typedef struct LPFRCSTATE {
    SFILTERNODE sfn; // offset 0x0, size 0x1C
    LPFRC lpf;       // offset 0x1C, size 0xC
} LPFRCSTATE;

int SFILTER_lpfRC(void *pstate, int outputframes, void *psrcp, void *pdstp, int requester) {
    LPFRCSTATE *pls = (LPFRCSTATE *)pstate;
    int temp;

    if (pls->sfn.pfnnext) {
        temp = pls->sfn.pfnnext->filterfn(pls->sfn.pfnnext, outputframes, pdstp, psrcp,
                                          pls->sfn.requester);
        if (temp <= 0) {
            return temp;
        }
    }
    {
        LPFRC *plpf = &pls->lpf;
        float *psrc = (float *)psrcp;
        float *pdst = (float *)pdstp;
        int i;

        for (i = 0; i < outputframes; i++) {
            plpf->accumulator = plpf->accumulator * plpf->leakage + plpf->inverse * psrc[i];
            pdst[i] = plpf->accumulator;
        }
    }
    return outputframes;
}

int SFILTER_createLPFRC(LPFRCSTATE *pls) {
    pls->sfn.pfnnext = 0; pls->sfn.pfnnext2 = 0; pls->sfn.pfnprev1 = 0; pls->sfn.pfnprev2 = 0;
    pls->sfn.clean = 0;
    pls->sfn.restorefn = 0;
    pls->sfn.filterfn = SFILTER_lpfRC;
    pls->lpf.accumulator = 0.0f;
    return 0;
}

void SFILTER_modifyLPFRC(LPFRCSTATE *pls, int *pparams) {
    pls->lpf.inverse = 2.0f * pparams[0] / pparams[1];
    pls->lpf.leakage = 1.0f - pls->lpf.inverse;
    pls->lpf.inverse *= (1.0f / 256.0f) * pparams[2];
    pls->lpf.leakage *= (1.0f / 256.0f) * pparams[2];
}
