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

// total size: 0x3C
typedef struct UNPACKMTPFSTATE {
    SFILTERNODE sfn;                 // offset 0x0, size 0x1C
    unsigned char *psample;          // offset 0x1C
    int totalframes;                 // offset 0x20
    int packetinstancehandle;        // offset 0x24
    unsigned int prevframesunpacked; // offset 0x28
    unsigned short curframe;         // offset 0x2C
    int platformver;                 // offset 0x30
    Snd::CMTBLKDecf *pDecoder;       // offset 0x34
    unsigned char samplechan;        // offset 0x38
    char pad[3];                     // offset 0x39
} UNPACKMTPFSTATE;

int SNDDRV_getmastervoice(int voice);
int SNDDRV_getsamplechan(int voice);
int SNDPKTPLAYI_voicetopackethandle(int voice);
char *SNDPKTPLAYI_get(int pktchan, int chan, int *pframes, int *pcontinuation);
void SNDPKTPLAYI_freeframes(int pktchan, int chan, int frames);

int SFILTER_unpackmtpf(void *pstate, int framestoup, void *psrcpassed, void *pdstpassed,
                       int requester) {
    UNPACKMTPFSTATE *pups = (UNPACKMTPFSTATE *)pstate;
    int continuation;
    Snd::MTFSTATE mts;
    float *pdst;
    int retVal;
    int framesdecoded;

    framesdecoded = 0;
    pdst = (float *)pdstpassed;
    if (pups->prevframesunpacked) {
        SNDPKTPLAYI_freeframes(pups->packetinstancehandle, pups->samplechan,
                               pups->prevframesunpacked);
        pups->prevframesunpacked = 0;
    }
    if (framestoup > 0) {
        do {
            retVal = pups->pDecoder->Decode(&pdst, framestoup);
            pups->curframe += retVal;
            pdst += retVal;
            pups->prevframesunpacked += retVal;
            framesdecoded += retVal;
            if (retVal < framestoup) {
                framestoup -= retVal;
                pups->psample = (unsigned char *)SNDPKTPLAYI_get(pups->packetinstancehandle,
                                                                 pups->samplechan,
                                                                 &pups->totalframes,
                                                                 &continuation);
                if (pups->psample != 0) {
                    pups->curframe = 0;
                    if (pups->platformver <= 2) {
                        if (continuation == 0) {
                            mts = pups->pDecoder->GetState();
                            mts.doWholeInit = 1;
                            pups->pDecoder->SetState(&mts, 0);
                        } else {
                            mts = pups->pDecoder->GetState();
                            mts.doWholeInit = 0;
                            pups->pDecoder->SetState(&mts, 0);
                        }
                        pups->pDecoder->SetCodecVersion(0);
                    } else {
                        if (continuation == 0) {
                            mts = pups->pDecoder->GetState();
                            mts.bufframes = 0;
                            mts.doWholeInit = 1;
                        } else {
                            mts = pups->pDecoder->GetState();
                            mts.doWholeInit = 0;
                        }
                        pups->pDecoder->SetState(&mts, 0);
                        pups->pDecoder->SetCodecVersion(1);
                    }
                    pups->pDecoder->Feed(pups->psample + 1, pups->totalframes * sizeof(float),
                                         pups->totalframes);
                } else {
                    if (framestoup > 0 && pups->prevframesunpacked != 0) {
                        memset(pdst, 0, framestoup * sizeof(float));
                    }
                    pups->totalframes = 0;
                    framestoup = 0;
                }
            } else {
                framestoup -= retVal;
            }
        } while (framestoup > 0);
    }
    return framesdecoded;
}

void SFILTER_unpackmtpfrestore(void *pstate) {
    UNPACKMTPFSTATE *pups = (UNPACKMTPFSTATE *)pstate;

    delete pups->pDecoder;
}

void SFILTER_unpackmtpfinit(void *pstate, UNPACKINITPARAMS *pupip) {
    UNPACKMTPFSTATE *pups = (UNPACKMTPFSTATE *)pstate;

    pups->sfn.filterfn = SFILTER_unpackmtpf;
    pups->packetinstancehandle =
        SNDPKTPLAYI_voicetopackethandle(SNDDRV_getmastervoice(pupip->voice));
    pups->samplechan = SNDDRV_getsamplechan(pupip->voice);
    pups->psample = 0;
    pups->curframe = 0;
    pups->platformver = pupip->platformver;
    pups->totalframes = pupip->totalframes;
    pups->sfn.restorefn = SFILTER_unpackmtpfrestore;
    pups->pDecoder = new Snd::CMTBLKDecf();
    pups->prevframesunpacked = 0;
}
