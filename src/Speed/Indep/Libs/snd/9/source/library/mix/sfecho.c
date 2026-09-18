#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include <snd/sfilter.h>

extern "C" {
void *memset(void *dst, int c, unsigned int bytes);
}

void *SNDMEMI_allocz(int size);
void SNDMEMI_free(void *pmem);
int SNDI_findmult16(int samplerate, int delaytime);

// total size: 0x34
typedef struct ECHOSTATE {
    SFILTERNODE sfn;             // offset 0x0, size 0x1C
    float *pdelaybuf;            // offset 0x1C
    float *punaligneddelaybuf;   // offset 0x20
    int sampleindex;             // offset 0x24
    int bufsamples;              // offset 0x28
    float fblevels;              // offset 0x2C
    float fbneglevels;           // offset 0x30
} ECHOSTATE;

static inline void SNDI_echo(ECHOSTATE *pes, int frames, void *psrcp, void *pdstp) {
    float *ptaken;
    float fedforward[4];
    float *psrc;
    float *pdst;
    float *pdstend;

    ptaken = pes->pdelaybuf + pes->sampleindex;
    psrc = (float *)psrcp;
    pdst = (float *)pdstp;
    pdstend = pdst + frames;
    while (pdst < pdstend) {
        fedforward[0] = ptaken[0] * pes->fblevels + psrc[0] + 1e-20f;
        fedforward[1] = ptaken[1] * pes->fblevels + psrc[1] + 1e-20f;
        fedforward[2] = ptaken[2] * pes->fblevels + psrc[2] + 1e-20f;
        fedforward[3] = ptaken[3] * pes->fblevels + psrc[3] + 1e-20f;
        pdst[0] = fedforward[0] * pes->fbneglevels + ptaken[0];
        pdst[1] = fedforward[1] * pes->fbneglevels + ptaken[1];
        pdst[2] = fedforward[2] * pes->fbneglevels + ptaken[2];
        pdst[3] = fedforward[3] * pes->fbneglevels + ptaken[3];
        ptaken[0] = fedforward[0];
        ptaken[1] = fedforward[1];
        ptaken[2] = fedforward[2];
        ptaken[3] = fedforward[3];
        psrc += 4;
        pdst += 4;
        ptaken += 4;
    }
}

int SFILTER_echo(void *pstate, int frames, void *psrcp, void *pdstp, int requester) {
    ECHOSTATE *pes = (ECHOSTATE *)pstate;
    float *ptempsrc;
    float *ptempdst;
    int moduleframes;
    int temp;

    if (pes->sfn.pfnnext) {
        temp = pes->sfn.pfnnext->filterfn(pes->sfn.pfnnext, frames, pdstp, psrcp,
                                          pes->sfn.requester);
        if (temp <= 0) {
            return temp;
        }
    }
    ptempsrc = (float *)psrcp;
    ptempdst = (float *)pdstp;
    moduleframes = frames;
    if (moduleframes > 0) {
        do {
            if (pes->sampleindex >= pes->bufsamples) {
                pes->sampleindex = 0;
            }
            temp = pes->bufsamples - pes->sampleindex;
            if (temp > moduleframes) {
                temp = moduleframes;
            }
            SNDI_echo(pes, temp, ptempsrc, ptempdst);
            ptempsrc += temp;
            ptempdst += temp;
            moduleframes -= temp;
            pes->sampleindex += temp;
        } while (moduleframes > 0);
    }
    return frames;
}

void SFILTER_echorestore(void *pstate) {
    ECHOSTATE *pes = (ECHOSTATE *)pstate;

    if (pes->punaligneddelaybuf != 0) {
        SNDMEMI_free(pes->punaligneddelaybuf);
        pes->punaligneddelaybuf = 0;
        pes->pdelaybuf = 0;
    }
}

int SFILTER_createECHO(ECHOSTATE *pes) {
    pes->sfn.pfnnext = 0; pes->sfn.pfnnext2 = 0; pes->sfn.pfnprev1 = 0; pes->sfn.pfnprev2 = 0;
    pes->sfn.clean = 0;
    pes->punaligneddelaybuf = 0;
    pes->sfn.restorefn = SFILTER_echorestore;
    pes->sfn.filterfn = SFILTER_echo;
    pes->pdelaybuf = 0;
    return 0;
}

void SFILTER_modifyECHO(ECHOSTATE *pes, int *pparams) {
    int feedback;
    int samplerate;
    int delaytime;
    int delaybufsize;

    feedback = pparams[0] >> 1;
    samplerate = pparams[1] >> 8;
    delaytime = pparams[2] >> 8;
    SFILTER_echorestore(pes);
    pes->fblevels = (float)feedback * 0.007874016f;
    pes->fbneglevels = -pes->fblevels;
    pes->bufsamples = SNDI_findmult16(samplerate, delaytime);
    delaybufsize = pes->bufsamples * sizeof(float);
    pes->punaligneddelaybuf = (float *)SNDMEMI_allocz(delaybufsize + 16);
    pes->pdelaybuf = (float *)(((sndptruint)pes->punaligneddelaybuf + 15) & ~15);
    memset(pes->pdelaybuf, 0, delaybufsize);
    pes->sampleindex = 0;
}
