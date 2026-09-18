#include <cstddef>
#include <snd/sfilter.h>

extern "C" {
void *memset(void *dst, int c, unsigned int bytes);
}

namespace Snd {

// total size: 0x8
struct XAFSTATE {
    float sample1; // offset 0x0
    float sample2; // offset 0x4
};

// total size: 0xA8 (eaxadecf.cpp)
struct CEAXABLKDecf {
    static void *operator new(size_t size);
    static void operator delete(void *ptr);

    CEAXABLKDecf();
    ~CEAXABLKDecf() {}
    int Feed(void *pSampleData, int sampleDataSize, int numSamples);
    int Decode(float **pDstBuf, int numSamples);
    void SetState(XAFSTATE *pstate);

    char pad0[0xA8];
};

} // namespace Snd

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

// total size: 0x3C
typedef struct UNPACKXAPFSTATE {
    SFILTERNODE sfn;                 // offset 0x0, size 0x1C
    Snd::CEAXABLKDecf *pDecoder;     // offset 0x1C
    unsigned char *psample;          // offset 0x20
    int totalframes;                 // offset 0x24
    int curframe;                    // offset 0x28
    int platformver;                 // offset 0x2C
    int packetinstancehandle;        // offset 0x30
    unsigned int prevframesunpacked; // offset 0x34
    unsigned char samplechan;        // offset 0x38
    char pad[2];                     // offset 0x39
} UNPACKXAPFSTATE;

int SNDDRV_getmastervoice(int voice);
int SNDDRV_getsamplechan(int voice);
int SNDPKTPLAYI_voicetopackethandle(int voice);
char *SNDPKTPLAYI_get(int pktchan, int chan, int *pframes, int *pcontinuation);
void SNDPKTPLAYI_freeframes(int pktchan, int chan, int frames);

int SFILTER_unpackxapf(void *pstate, int framestoup, void *psrcp, void *pdstp, int requester) {
    UNPACKXAPFSTATE *pups = (UNPACKXAPFSTATE *)pstate;
    Snd::XAFSTATE xas;
    int continuation;
    short *pshort;
    int retVal;
    int framesdecoded;
    int setstate;
    float *ptmpdst;

    framesdecoded = 0;
    setstate = 0;
    ptmpdst = (float *)pdstp;
    if (pups->prevframesunpacked) {
        SNDPKTPLAYI_freeframes(pups->packetinstancehandle, pups->samplechan,
                               pups->prevframesunpacked);
        pups->prevframesunpacked = 0;
    }
    if (framestoup > 0) {
        do {
            retVal = pups->pDecoder->Decode(&ptmpdst, framestoup);
            pups->curframe += retVal;
            ptmpdst += retVal;
            pups->prevframesunpacked += retVal;
            framesdecoded += retVal;
            if (retVal < framestoup) {
                framestoup -= retVal;
                pups->psample = (unsigned char *)SNDPKTPLAYI_get(pups->packetinstancehandle,
                                                                 pups->samplechan,
                                                                 &pups->totalframes,
                                                                 &continuation);
                if (pups->psample != 0) {
                    if (pups->platformver <= 2) {
                        setstate = 1;
                    }
                    if (setstate) {
                        pshort = (short *)pups->psample;
                        xas.sample1 = (float)pshort[0];
                        xas.sample2 = (float)pshort[1];
                        pups->pDecoder->SetState(&xas);
                        pups->psample += 4;
                    }
                    pups->curframe = 0;
                    pups->pDecoder->Feed(pups->psample, pups->totalframes * sizeof(float),
                                         pups->totalframes);
                } else {
                    if (framestoup > 0 && pups->prevframesunpacked != 0) {
                        memset(ptmpdst, 0, framestoup * sizeof(float));
                    }
                    framestoup = 0;
                }
            } else {
                framestoup -= retVal;
            }
        } while (framestoup > 0);
    }
    return framesdecoded;
}

void SFILTER_unpackxapfrestore(void *pstate) {
    UNPACKXAPFSTATE *pups = (UNPACKXAPFSTATE *)pstate;

    delete pups->pDecoder;
}

void SFILTER_unpackxapfinit(void *pstate, UNPACKINITPARAMS *pupip) {
    UNPACKXAPFSTATE *pups = (UNPACKXAPFSTATE *)pstate;

    pups->sfn.filterfn = SFILTER_unpackxapf;
    pups->sfn.restorefn = SFILTER_unpackxapfrestore;
    pups->packetinstancehandle =
        SNDPKTPLAYI_voicetopackethandle(SNDDRV_getmastervoice(pupip->voice));
    pups->samplechan = SNDDRV_getsamplechan(pupip->voice);
    pups->platformver = pupip->platformver;
    pups->psample = 0;
    pups->totalframes = pupip->totalframes;
    pups->curframe = 0;
    pups->prevframesunpacked = 0;
    pups->pDecoder = new Snd::CEAXABLKDecf();
}
