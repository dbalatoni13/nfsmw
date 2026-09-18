#include <cstddef>
#include <snd/sfilter.h>

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
    XAFSTATE GetState();
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

// total size: 0x54
typedef struct UNPACKXALFSTATE {
    SFILTERNODE sfn;             // offset 0x0, size 0x1C
    Snd::CEAXABLKDecf *pDecoder; // offset 0x1C
    int curframe;                // offset 0x20
    int loopstart;               // offset 0x24
    int loopend;                 // offset 0x28
    void *ploop;                 // offset 0x2C
    int platformver;             // offset 0x30
    unsigned char *psample;      // offset 0x34
    int totalframes;             // offset 0x38
    int startblockmet;           // offset 0x3C
    int feddatabeforess;         // offset 0x40
    int sustainstart;            // offset 0x44
    int sustainend;              // offset 0x48
    float sustainstarts1;        // offset 0x4C
    float sustainstarts2;        // offset 0x50
} UNPACKXALFSTATE;

int SFILTER_unpackxalf(void *pstate, int framestoup, void *psrcp, void *pdstp, int requester) {
    UNPACKXALFSTATE *pups = (UNPACKXALFSTATE *)pstate;
    float *ptmpdst;
    int retVal;
    int framesdecoded;
    int frames;
    Snd::XAFSTATE xas;
    float tmpbuf[30];
    float *ptmpbuf;
    int framestoend;
    int tmpframecnt;

    framesdecoded = 0;
    ptmpdst = (float *)pdstp;
    if (pups->platformver <= 2) {
        if (pups->feddatabeforess == 0) {
            tmpframecnt = (pups->sustainstart / 28) * 28;
            pups->pDecoder->Feed(pups->psample, tmpframecnt * sizeof(float), tmpframecnt);
            pups->feddatabeforess = 1;
        }
        if (framestoup != 0) {
            do {
                retVal = pups->pDecoder->Decode(&ptmpdst, framestoup);
                framesdecoded += retVal;
                ptmpdst += retVal;
                pups->curframe += retVal;
                if (retVal < framestoup) {
                    framestoup -= retVal;
                    tmpframecnt = pups->sustainstart % 28;
                    pups->curframe = pups->sustainstart - tmpframecnt;
                    framestoend = pups->sustainend - pups->curframe + 1;
                    pups->pDecoder->Feed(pups->psample + (pups->curframe / 28) * 15,
                                         framestoend * sizeof(float), framestoend);
                    if (pups->startblockmet == 0) {
                        xas = pups->pDecoder->GetState();
                        pups->sustainstarts1 = xas.sample1;
                        pups->sustainstarts2 = xas.sample2;
                        retVal = pups->pDecoder->Decode(&ptmpdst, framestoup);
                        ptmpdst += retVal;
                        framestoup -= retVal;
                        framesdecoded += retVal;
                        pups->curframe += retVal;
                        pups->startblockmet = 1;
                    } else {
                        xas.sample1 = pups->sustainstarts1;
                        xas.sample2 = pups->sustainstarts2;
                        pups->pDecoder->SetState(&xas);
                        ptmpbuf = &tmpbuf[2];
                        retVal = pups->pDecoder->Decode(&ptmpbuf, tmpframecnt);
                        pups->curframe += retVal;
                    }
                } else {
                    framestoup -= retVal;
                }
            } while (framestoup != 0);
        }
    } else {
        while (framestoup > 0) {
            retVal = pups->pDecoder->Decode(&ptmpdst, framestoup);
            framestoup -= retVal;
            ptmpdst += retVal;
            pups->curframe += retVal;
            framesdecoded += retVal;
            if (retVal < framestoup) {
                pups->curframe = pups->loopstart;
                frames = pups->loopend - pups->loopstart + 1;
                pups->pDecoder->Feed(pups->ploop, frames * sizeof(float), frames);
            }
        }
    }
    return framesdecoded;
}

int SFILTER_unpackgetframexalf(void *pstate) {
    UNPACKXALFSTATE *pups = (UNPACKXALFSTATE *)pstate;

    return pups->curframe;
}

void SFILTER_unpackxalfrestore(void *pstate) {
    UNPACKXALFSTATE *pups = (UNPACKXALFSTATE *)pstate;

    delete pups->pDecoder;
}

void SFILTER_unpackxalfinit(void *pstate, UNPACKINITPARAMS *pupip) {
    UNPACKXALFSTATE *pups = (UNPACKXALFSTATE *)pstate;

    pups->sfn.filterfn = SFILTER_unpackxalf;
    pups->sfn.restorefn = SFILTER_unpackxalfrestore;
    pups->ploop = pupip->ploopstart;
    pups->curframe = 0;
    pups->platformver = pupip->platformver;
    pups->loopstart = pupip->sustainstart;
    pups->loopend = pupip->sustainend;
    pups->pDecoder = new Snd::CEAXABLKDecf();
    pupip->unpackgetframe = SFILTER_unpackgetframexalf;
    if (pups->platformver <= 2) {
        pups->psample = (unsigned char *)pupip->psample;
        pups->totalframes = pupip->totalframes;
        pups->startblockmet = 0;
        pups->feddatabeforess = 0;
        pups->sustainstart = pupip->sustainstart;
        pups->sustainend = pupip->sustainend;
    } else {
        pups->pDecoder->Feed(pupip->psample, pups->loopstart * sizeof(float), pups->loopstart);
    }
}
