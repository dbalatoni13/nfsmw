#include <cstddef>
#include <snd/sfilter.h>

namespace Snd {

// total size: 0xC
struct MTFSTATE {
    unsigned int shiftreg;  // offset 0x0
    unsigned int bufframes; // offset 0x4
    int doWholeInit;        // offset 0x8
};

// total size: 0xD68 (mtdecf.cpp)
struct CMTBLKDecf {
    static void *operator new(size_t size);
    static void operator delete(void *ptr);

    CMTBLKDecf();
    ~CMTBLKDecf() {}
    int Feed(void *pSampleData, int sampleDataSize, int numSamples);
    int Decode(float **pDstBuf, int numSamples);
    MTFSTATE GetState();
    void SetState(MTFSTATE *pstate, int ignoresamples);
    void SetCodecVersion(int codecversion);

    char pad0[0xD68];
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

// total size: 0x30
typedef struct UNPACKMTLFSTATE {
    SFILTERNODE sfn;           // offset 0x0, size 0x1C
    Snd::CMTBLKDecf *pDecoder; // offset 0x1C
    int curframe;              // offset 0x20
    int loopstart;             // offset 0x24
    int loopend;               // offset 0x28
    void *ploop;               // offset 0x2C
} UNPACKMTLFSTATE;

int SFILTER_unpackgetframemtlf(void *pstate) {
    UNPACKMTLFSTATE *pups = (UNPACKMTLFSTATE *)pstate;

    return pups->curframe;
}

int SFILTER_unpackmtlf(void *pstate, int framestoup, void *psrcp, void *pdstp, int requester) {
    UNPACKMTLFSTATE *pups = (UNPACKMTLFSTATE *)pstate;
    Snd::MTFSTATE mts;
    float *ptmpdst;
    int retVal;
    int framesdecoded;
    int frames;

    ptmpdst = (float *)pdstp;
    if (framestoup > 0) {
        do {
            framesdecoded = pups->pDecoder->Decode(&ptmpdst, framestoup);
            ptmpdst += framesdecoded;
            pups->curframe += framesdecoded;
            framestoup -= framesdecoded;
            if (framesdecoded < framestoup) {
                pups->curframe = pups->loopstart;
                mts.bufframes = 0;
                mts.shiftreg = 0;
                mts.doWholeInit = 1;
                pups->pDecoder->SetState(&mts, 0);
                frames = pups->loopend - pups->loopstart + 1;
                pups->pDecoder->Feed(pups->ploop, frames * sizeof(float), frames);
            }
        } while (framestoup > 0);
    }
    retVal = 1;
    return retVal;
}

void SFILTER_unpackmtlfrestore(void *pstate) {
    UNPACKMTLFSTATE *pups = (UNPACKMTLFSTATE *)pstate;

    delete pups->pDecoder;
}

void SFILTER_unpackmtlfinit(void *pstate, UNPACKINITPARAMS *pupip) {
    UNPACKMTLFSTATE *pups = (UNPACKMTLFSTATE *)pstate;
    Snd::MTFSTATE mts;

    pups->sfn.filterfn = SFILTER_unpackmtlf;
    pups->sfn.restorefn = SFILTER_unpackmtlfrestore;
    pups->ploop = pupip->ploopstart;
    pups->curframe = 0;
    pups->loopstart = pupip->sustainstart;
    pups->loopend = pupip->sustainend;
    pups->pDecoder = new Snd::CMTBLKDecf();
    mts = pups->pDecoder->GetState();
    mts.doWholeInit = 1;
    pups->pDecoder->SetState(&mts, 0);
    pups->pDecoder->Feed(pupip->psample, pups->loopstart * sizeof(float), pups->loopstart);
    pupip->unpackgetframe = SFILTER_unpackgetframemtlf;
}
