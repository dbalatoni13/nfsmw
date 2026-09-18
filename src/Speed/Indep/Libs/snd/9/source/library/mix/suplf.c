#include <snd/sfilter.h>

// smixi.h (shared with the other mix/ units) only carries a partial view of
// sndmix, so the fields this unit needs are declared locally.

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

// total size: 0x30
typedef struct UNPACKLFSTATE {
    SFILTERNODE sfn;            // offset 0x0, size 0x1C
    char *psample;              // offset 0x1C
    unsigned int curframe;      // offset 0x20
    unsigned int sustainstart;  // offset 0x24
    unsigned int sustainend;    // offset 0x28
    int bits16;                 // offset 0x2C
} UNPACKLFSTATE;

int SFILTER_unpacklfgetframe(void *pstate) {
    UNPACKLFSTATE *pups = (UNPACKLFSTATE *)pstate;

    return pups->curframe;
}

int SFILTER_unpacklf(void *pstate, int framestoup, void *psrcp, void *pdstp, int requester) {
    UNPACKLFSTATE *pups = (UNPACKLFSTATE *)pstate;
    int maxframes;
    float *pdst;

    pdst = (float *)pdstp;
    while (framestoup > 0) {
        maxframes = pups->sustainend - pups->curframe + 1;
        if (framestoup < maxframes) {
            maxframes = framestoup;
        }
        if (pups->bits16) {
            sndmix.decode16(maxframes, (short *)(pups->psample + (pups->curframe << 1)), pdst);
        }
        pdst += maxframes;
        pups->curframe += maxframes;
        framestoup -= maxframes;
        if (pups->curframe > pups->sustainend) {
            pups->curframe = pups->sustainstart;
        }
    }
    return 1;
}

void SFILTER_unpacklfinit(void *pstate, UNPACKINITPARAMS *pupip) {
    UNPACKLFSTATE *pups = (UNPACKLFSTATE *)pstate;

    pups->sfn.filterfn = SFILTER_unpacklf;
    pups->psample = (char *)pupip->psample;
    pups->curframe = 0;
    pups->sustainstart = pupip->sustainstart;
    pups->sustainend = pupip->sustainend;
    pups->bits16 = pupip->bits16;
    pupip->unpackgetframe = SFILTER_unpacklfgetframe;
}
