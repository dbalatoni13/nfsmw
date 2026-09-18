#include <snd/sfilter.h>

void *SNDMEMI_allocz(int size);
void SNDMEMI_free(void *pmem);

// total size: 0x24
typedef struct MIXERSTATE {
    SFILTERNODE sfn;   // offset 0x0, size 0x1C
    float *ptempbuf;   // offset 0x1C
    int tempbufframes; // offset 0x20
} MIXERSTATE;

int SFILTER_mixer(void *pstate, int frames, void *psrcp, void *pdstp, int requester) {
    MIXERSTATE *pms = (MIXERSTATE *)pstate;
    int totalbufsize;
    int retframes1;
    int retframes2;

    if (frames > pms->tempbufframes) {
        if (pms->ptempbuf) {
            SNDMEMI_free(pms->ptempbuf);
        }
        pms->tempbufframes = frames;
        totalbufsize = frames * sizeof(float);
        pms->ptempbuf = (float *)SNDMEMI_allocz(totalbufsize);
    }
    retframes1 = pms->sfn.pfnnext->filterfn(pms->sfn.pfnnext, frames, psrcp, pdstp, 1);
    if (retframes1 <= 0) {
        return retframes1;
    }
    retframes2 = pms->sfn.pfnnext2->filterfn(pms->sfn.pfnnext2, frames, psrcp, pms->ptempbuf, 2);
    if (retframes2 <= 0) {
        return retframes2;
    }
    {
        float *pdst = (float *)pdstp;
        int i;

        for (i = 0; i < frames; i++) {
            pdst[i] = pdst[i] + pms->ptempbuf[i];
        }
    }
    return frames;
}

void SFILTER_mixerrestore(void *pstate) {
    MIXERSTATE *pms = (MIXERSTATE *)pstate;

    if (pms->ptempbuf) {
        SNDMEMI_free(pms->ptempbuf);
    }
}

int SFILTER_createMIX(MIXERSTATE *pms) {
    pms->sfn.pfnnext = 0; pms->sfn.pfnnext2 = 0; pms->sfn.pfnprev1 = 0; pms->sfn.pfnprev2 = 0;
    pms->sfn.clean = 0;
    pms->ptempbuf = 0;
    pms->sfn.restorefn = SFILTER_mixerrestore;
    pms->sfn.filterfn = SFILTER_mixer;
    pms->tempbufframes = 0;
    return 0;
}
