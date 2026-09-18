#include <snd/sfilter.h>

// smixi.h (shared with the other mix/ units) only carries a partial view of
// sndmix, so the fields this unit needs are declared locally.

extern "C" {
void *memset(void *dst, int c, unsigned int bytes);
}

#if defined(__ANDROID__)
// `char pad0[0x1E4]` es una cuenta de bytes medida con punteros de 4
// bytes: en 64 bits decode16 cae en otro sitio que en smixer.c.
#include "smixport.h"
#else
// total size: 0x1E8
typedef struct SNDMIXSTATE {
    char pad0[0x1E4];
    void (*decode16)(int frames, short *psrc, float *pdst); // offset 0x1E4
} SNDMIXSTATE;

extern SNDMIXSTATE sndmix;
#endif

// total size: 0x2C
typedef struct UNPACKINITPARAMS {
    void *psample;                 // offset 0x0
    void *ploopstart;              // offset 0x4
    int numchan;                   // offset 0x8
    int totalframes;               // offset 0xC
    int sustainstart;              // offset 0x10
    int sustainend;                // offset 0x14
    int bits16;                    // offset 0x18
    int voice;                     // offset 0x1C
    int cpu;                       // offset 0x20
    int platformver;               // offset 0x24
    int (*unpackgetframe)(void *); // offset 0x28
} UNPACKINITPARAMS;

// total size: 0x2C
typedef struct UNPACKFSTATE {
    SFILTERNODE sfn;          // offset 0x0, size 0x1C
    char *psample;            // offset 0x1C
    unsigned int curframe;    // offset 0x20
    unsigned int totalframes; // offset 0x24
    int bits16;               // offset 0x28
} UNPACKFSTATE;

int SFILTER_unpackfgetframe(void *pstate) {
    UNPACKFSTATE *pups = (UNPACKFSTATE *)pstate;

    return pups->curframe;
}

int SFILTER_unpackf(void *pstate, int framestoup, void *psrcp, void *pdstp, int requester) {
    UNPACKFSTATE *pups = (UNPACKFSTATE *)pstate;
    int midframe;
    int startframe;
    float *pdst;

    startframe = pups->curframe;
    if (startframe >= pups->totalframes) {
        return -1;
    }
    pups->curframe = startframe + framestoup;
    if (pups->curframe < pups->totalframes) {
        if (pups->bits16) {
            sndmix.decode16(framestoup, (short *)(pups->psample + (startframe << 1)), (float *)pdstp);
        }
    } else {
        midframe = pups->totalframes - startframe;
        if (pups->bits16) {
            sndmix.decode16(midframe, (short *)(pups->psample + (startframe << 1)), (float *)pdstp);
        }
        pdst = (float *)pdstp + midframe;
        memset(pdst, 0, (framestoup - midframe) * sizeof(float));
    }
    return 1;
}

void SFILTER_unpackfinit(void *pupsin, UNPACKINITPARAMS *pupip) {
    UNPACKFSTATE *pups = (UNPACKFSTATE *)pupsin;

    pups->sfn.filterfn = SFILTER_unpackf;
    pups->psample = (char *)pupip->psample;
    pups->curframe = 0;
    pups->totalframes = pupip->totalframes;
    pups->bits16 = pupip->bits16;
    pupip->unpackgetframe = SFILTER_unpackfgetframe;
    pups->sfn.restorefn = 0;
}
