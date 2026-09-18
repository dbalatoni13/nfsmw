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

// total size: 0x34
typedef struct UNPACKPFSTATE {
    SFILTERNODE sfn;                  // offset 0x0, size 0x1C
    signed char *psample;             // offset 0x1C
    int totalframes;                  // offset 0x20
    int curframe;                     // offset 0x24
    int packetinstancehandle;         // offset 0x28
    unsigned int prevframesunpacked;  // offset 0x2C
    unsigned char bits16;             // offset 0x30
    unsigned char samplechan;         // offset 0x31
    char pad[2];                      // offset 0x32
} UNPACKPFSTATE;

int SNDDRV_getmastervoice(int voice);
int SNDDRV_getsamplechan(int voice);
int SNDPKTPLAYI_voicetopackethandle(int voice);
char *SNDPKTPLAYI_get(int pktchan, int chan, int *pframes, int *pcontinuation);
void SNDPKTPLAYI_freeframes(int pktchan, int chan, int frames);

int SFILTER_unpackpf(void *pstate, int framestoup, void *psrcp, void *pdstp, int requester) {
    UNPACKPFSTATE *pups = (UNPACKPFSTATE *)pstate;
    int maxframes;
    int totalframes;
    int continuation;
    float *pdst;

    pdst = (float *)pdstp;
    if (pups->prevframesunpacked) {
        SNDPKTPLAYI_freeframes(pups->packetinstancehandle, pups->samplechan,
                               pups->prevframesunpacked);
        pups->prevframesunpacked = 0;
    }
    if (framestoup > 0) {
        do {
        if (pups->curframe >= pups->totalframes) {
            pups->psample = (signed char *)SNDPKTPLAYI_get(pups->packetinstancehandle,
                                                           pups->samplechan, &totalframes,
                                                           &continuation);
            if (pups->psample == 0) {
                if (pups->prevframesunpacked == 0) {
                    break;
                }
                memset(pdst, 0, framestoup * sizeof(float));
                break;
            }
            pups->curframe = 0;
            pups->totalframes = totalframes;
        }
        maxframes = pups->totalframes - pups->curframe;
        if (framestoup < maxframes) {
            maxframes = framestoup;
        }
        if (pups->bits16) {
            sndmix.decode16(maxframes, (short *)(pups->psample + (pups->curframe << 1)), pdst);
        }
        pdst += maxframes;
        pups->curframe += maxframes;
        framestoup -= maxframes;
        pups->prevframesunpacked += maxframes;
        } while (framestoup > 0);
    }
    return pups->prevframesunpacked;
}

void SFILTER_unpackpfinit(void *pstate, UNPACKINITPARAMS *pupip) {
    UNPACKPFSTATE *pups = (UNPACKPFSTATE *)pstate;

    pups->sfn.filterfn = SFILTER_unpackpf;
    pups->packetinstancehandle =
        SNDPKTPLAYI_voicetopackethandle(SNDDRV_getmastervoice(pupip->voice));
    pups->samplechan = SNDDRV_getsamplechan(pupip->voice);
    pups->totalframes = -1;
    pups->psample = 0;
    pups->curframe = 0;
    pups->bits16 = pupip->bits16;
    pups->prevframesunpacked = 0;
}
