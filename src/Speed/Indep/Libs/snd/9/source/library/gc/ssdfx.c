#include "../cmn/sndcmn.h"
#include "../cmn/sndfxcmn.h"
#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/os.h>
#endif
#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/ax.h>
#endif

// Driver types duplicate snddrv.c until its private layouts have a shared
// header. Keeping the complete state declaration preserves the voice array.
struct SNDDRVVOICE {
    float balances[6];
    volatile int sampleaddr;
    signed char pktchan;
    unsigned char currentvol;
    unsigned char oldfinalvol;
    unsigned char freenextframe;
    volatile unsigned int dmahandle;
    volatile unsigned char state;
    volatile unsigned char freed;
    char pad[20];
    AXVPB *paxvpb;
};

struct ARQRequest {
    struct ARQRequest *next;
    unsigned int owner;
    unsigned int type;
    unsigned int priority;
    unsigned int source;
    unsigned int dest;
    unsigned int length;
    void (*callback)(unsigned long);
};

typedef struct SNDDRVDMASLOT {
    volatile int handle;    // offset 0x0
    ARQRequest req;         // offset 0x4
    unsigned long priority; // offset 0x24
    void *psrc;             // offset 0x28
    void *pdst;             // offset 0x2C
    unsigned long size;     // offset 0x30
    unsigned int tick;      // offset 0x34
    char pad[8];            // offset 0x38
} SNDDRVDMASLOT;

typedef struct SNDDRVPKTCHAN {
    char xferbuf[6][1600];        // offset 0x0, size 0x2580
    char *pxferbuf[6];            // offset 0x2580, size 0x18
    signed char mastervoice;      // offset 0x2598, size 0x1
    unsigned char playbuf;        // offset 0x2599, size 0x1
    signed char prebuffer;        // offset 0x259A, size 0x1
    unsigned char numchannels;    // offset 0x259B, size 0x1
    unsigned int aramaddr[6];     // offset 0x259C, size 0x18
    unsigned int aramaddrhalf;    // offset 0x25B4, size 0x4
    short framesinbuf[6];         // offset 0x25B8, size 0xC
    char *ppacket[6];             // offset 0x25C4, size 0x18
    int packetbytestotal;         // offset 0x25DC, size 0x4
    int packetxferoffset;         // offset 0x25E0, size 0x4
} SNDDRVPKTCHAN;

typedef struct SNDDRVSTATE {
    SNDDRVVOICE voice[80];                  // 0x0000
    float updateperiod;                     // 0x1400
    unsigned int aramaddr;                  // 0x1404
    unsigned int aramaddrhalf;              // 0x1408
    unsigned int aramaddrdouble;            // 0x140C
    SNDDRVPKTCHAN *pktchan[4];              // 0x1410
    char pad_1420[0x14A0 - 0x1420];         // 0x1420
    char zerobuf[0x1AC0 - 0x14A0];          // 0x14A0
    char *pzerobuf;                         // 0x1AC0
    char pad_1AC4[0x1AC8 - 0x1AC4];         // 0x1AC4
    OSThread thread;                        // 0x1AC8
    OSThreadQueue threadqueue;              // 0x1DE0
    char threadstack[0x8000];               // 0x1DE8
    unsigned int lastdmaowner;              // 0x9DE8
    int dmahandle;                          // 0x9DEC
    char pad_9DF0[0x9E30 - 0x9DF0];         // 0x9DF0
    SNDDRVDMASLOT dmaslot[64];              // 0x9E30
    char pad_AE30[0xBE30 - 0xAE30];         // 0xAE30
    OSMutex mutex;                          // 0xBE30
    char pad_BE48[0xC048 - 0xBE48];         // 0xBE48
    unsigned char mixerinited;              // 0xC048
    unsigned char numfreenextframe;         // 0xC049
    char pad_C04A[0xC050 - 0xC04A];         // 0xC04A
    void *mixbuf[6];                        // 0xC050
    void *pmixbuf[6];                       // 0xC068
    char pad_C080[0xC0C0 - 0xC080];         // 0xC080
} SNDDRVSTATE;
extern SNDDRVSTATE snddrv;

extern "C" unsigned int MIX_initreverb(int outputrate, int fxBusIndex, void *pFxFile);
extern "C" unsigned int MIX_restorereverb(int fxBusIndex, unsigned int instanceHandle);
extern "C" void SNDMIX_setwetgain(int voice, int bus, float gain);
void SNDDRV_DSPMixerSetAuxBus(AXVPB *p, unsigned short fxlevel, int auxbus);

unsigned int Snd::Hal::SetCustomFx(void *pData, void *pFxDefinition) {
    GlobalFxProcessorData *pfxData = static_cast<GlobalFxProcessorData *>(pData);
    SNDSYS_entercritical();
    unsigned int retVal = MIX_initreverb(sndgs.sso.set.outputrate, pfxData->busId, pFxDefinition);
    SNDSYS_leavecritical();
    return retVal;
}

unsigned int Snd::Hal::Reset(void *pData) {
    GlobalFxProcessorData *pfxData = static_cast<GlobalFxProcessorData *>(pData);
    SNDSYS_entercritical();
    unsigned int retVal = MIX_restorereverb(pfxData->busId, pfxData->instanceHandle);
    SNDSYS_leavecritical();
    return retVal;
}

int SNDPLATFORM_setfxlevel(int voice, int bus) {
    CHANPUB *ppubv = &sndgs.chan[voice];
    if (ppubv->rendermode & 0x200) {
        float gain = ppubv->finalvol * ppubv->pFxVolume[bus].fxLevel;
        SNDDRV_DSPMixerSetAuxBus(snddrv.voice[voice].paxvpb,
                                static_cast<unsigned short>(static_cast<int>(gain * 32767.0f)), 1);
    } else if (ppubv->rendermode & 4) {
        float gain = ppubv->finalvol * ppubv->pFxVolume[bus].fxLevel;
        SNDMIX_setwetgain(voice - sndgs.sso.set.voicesdsp, bus, gain);
    } else {
        return -15;
    }
    return 0;
}
