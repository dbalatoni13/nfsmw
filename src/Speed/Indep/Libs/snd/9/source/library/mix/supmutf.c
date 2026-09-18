#include <cstddef>
#include <snd/sfilter.h>

extern "C" {
void *memset(void *dst, int c, unsigned int bytes);
}

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

// total size: 0x2C
typedef struct UNPACKMTFSTATE {
    SFILTERNODE sfn;           // offset 0x0, size 0x1C
    unsigned int totalframes;  // offset 0x1C
    unsigned int curframe;     // offset 0x20
    int platformver;           // offset 0x24
    Snd::CMTBLKDecf *pDecoder; // offset 0x28
} UNPACKMTFSTATE;

int SFILTER_unpackgetframemtf(void *pstate) {
    UNPACKMTFSTATE *pups = (UNPACKMTFSTATE *)pstate;

    return pups->curframe;
}

int SFILTER_unpackmtf(void *pstate, int framestoup, void *psrcp, void *pdstp, int requester) {
    UNPACKMTFSTATE *pups = (UNPACKMTFSTATE *)pstate;
    float *pdst;
    int retVal;
    int framesdecoded;

    pdst = (float *)pdstp;
    if (pups->curframe >= pups->totalframes) {
        retVal = -1;
    } else {
        framesdecoded = pups->pDecoder->Decode(&pdst, framestoup);
        pups->curframe += framesdecoded;
        pdst += framesdecoded;
        if (framesdecoded < framestoup) {
            framestoup -= framesdecoded;
            memset(pdst, 0, framestoup * sizeof(float));
        }
        retVal = 1;
    }
    return retVal;
}

void SFILTER_unpackmtfrestore(void *pstate) {
    UNPACKMTFSTATE *pups = (UNPACKMTFSTATE *)pstate;

    delete pups->pDecoder;
}

void SFILTER_unpackmtfinit(void *pstate, UNPACKINITPARAMS *pupip) {
    UNPACKMTFSTATE *pups = (UNPACKMTFSTATE *)pstate;
    Snd::MTFSTATE mts;

    pups->sfn.filterfn = SFILTER_unpackmtf;
    pups->totalframes = pupip->totalframes;
    pups->curframe = 0;
    pups->platformver = pupip->platformver;
    pupip->unpackgetframe = SFILTER_unpackgetframemtf;
    pups->sfn.restorefn = SFILTER_unpackmtfrestore;
    pups->pDecoder = new Snd::CMTBLKDecf();
    mts = pups->pDecoder->GetState();
    mts.doWholeInit = 1;
    pups->pDecoder->SetState(&mts, 0);
    if (pups->platformver <= 2) {
        pups->pDecoder->SetCodecVersion(0);
    } else {
        pups->pDecoder->SetCodecVersion(1);
    }
    pups->pDecoder->Feed(pupip->psample, pups->totalframes * sizeof(float), pups->totalframes);
}
