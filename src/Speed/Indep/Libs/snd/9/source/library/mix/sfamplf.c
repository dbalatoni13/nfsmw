#include <snd/sfilter.h>

// total size: 0x20
typedef struct AMPLFSTATE {
    SFILTERNODE sfn; // offset 0x0, size 0x1C
    float gain;      // offset 0x1C
} AMPLFSTATE;

int SFILTER_amplf(void *pstate, int frames, void *psrcp, void *pdstp, int requester) {
    AMPLFSTATE *pas = (AMPLFSTATE *)pstate;
    int temp;

    if (pas->sfn.pfnnext) {
        temp = pas->sfn.pfnnext->filterfn(pas->sfn.pfnnext, frames, pdstp, psrcp,
                                          pas->sfn.requester);
        if (temp <= 0) {
            return temp;
        }
    }
    {
        float *psrc = (float *)psrcp;
        float *pdst = (float *)pdstp;
        int i;

        for (i = 0; i < frames; i++) {
            pdst[i] = pas->gain * psrc[i];
        }
    }
    return frames;
}

int SFILTER_createAMPLF(AMPLFSTATE *pas) {
    pas->sfn.pfnnext = 0; pas->sfn.pfnnext2 = 0; pas->sfn.pfnprev1 = 0; pas->sfn.pfnprev2 = 0;
    pas->sfn.clean = 0;
    pas->sfn.restorefn = 0;
    pas->sfn.filterfn = SFILTER_amplf;
    pas->gain = 0.0f;
    return 0;
}

void SFILTER_modifyAMPLF(AMPLFSTATE *pas, int *pparams) {
    pas->gain = (float)pparams[0] * (1.0f / 256.0f);
}
