#include "../cmn/sndcmn.h"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/os.h>
#endif
#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/ax.h>
#endif

#define SND_PLAYLOC_DSP 0x200

extern "C" {
void *memset(void *s, int c, unsigned int n);
unsigned long ARGetBaseAddress(void);
}

// dolphin/arq.h no se puede incluir: redefine el typedef ARQRequest que ar.h
// (arrastrado por os.h) ya declara, y GCC 2.9 en C++ lo rechaza.
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

extern "C" {
void ARQPostRequest(ARQRequest *task, unsigned int owner, unsigned int type, unsigned int priority,
                    unsigned int source, unsigned int dest, unsigned int length,
                    void (*callback)(unsigned long));
void ARQRemoveRequest(ARQRequest *task);
}

// total size: 0x40
typedef struct SNDDRVVOICE {
    float balances[6];              // offset 0x0, size 0x18
    volatile int sampleaddr;        // offset 0x18, size 0x4
    signed char pktchan;            // offset 0x1C, size 0x1
    unsigned char currentvol;       // offset 0x1D, size 0x1
    unsigned char oldfinalvol;      // offset 0x1E, size 0x1
    unsigned char freenextframe;    // offset 0x1F, size 0x1
    volatile unsigned int dmahandle; // offset 0x20, size 0x4
    volatile unsigned char state;   // offset 0x24, size 0x1
    volatile unsigned char freed;   // offset 0x25, size 0x1
    char pad[20];                   // offset 0x26, size 0x14
    AXVPB *paxvpb;                  // offset 0x3C, size 0x4
} SNDDRVVOICE;

// total size: 0x40
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

// total size: 0x25E4
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

// total size: 0x10
typedef struct SNDDRVAUXABUFFERS {
    int *auxbufs[4]; // offset 0x0, size 0x10
} SNDDRVAUXABUFFERS;

typedef struct SNDMIXSTATE {
    char pad_0[0x14];
    void **pfxbus; // offset 0x14
} SNDMIXSTATE;

extern SNDMIXSTATE sndmix;

// total size: 0xC
typedef struct MIXCONFIG {
    int outputrate;                  // offset 0x0, size 0x4
    void (*chanovercallback)(int);   // offset 0x4, size 0x4
    unsigned char inputchannels;     // offset 0x8, size 0x1
    unsigned char outputchannels;    // offset 0x9, size 0x1
    unsigned char outputinterleaved; // offset 0xA, size 0x1
} MIXCONFIG;

// bss: 0x804C03C0, total size 0xC0C0
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

SNDDRVSTATE __attribute__((aligned(32))) snddrv;

unsigned int SNDARAM_alloc(int size);
void SNDARAM_free(unsigned int address);
void SNDARAM_setpool(unsigned int pooladdr, int poolsize);
int SNDARAM_largestfree(unsigned int *paddr);

void SNDARAM_restore();

unsigned long SNDDRV_dmapost(unsigned long priority, void *psrc, void *pdst, unsigned long size,
                             int failindex);
void SNDDRV_restoredma(int handle);
void SNDDRV_restoremixer();
void SNDDRV_audiocallback(void *data, void *context);

void SNDVOICEI_free(int voice);
void SNDDRV_freeaxvoice(int voice, int dropped);
void SNDDRV_DSPMixerReleaseChannel(AXVPB *p);
void SNDDRV_DSPMixerSetVol(AXVPB *p, int vol);
void SNDDRV_DSPMixerInitChannel(AXVPB *p, unsigned long vol, int fxlevel, unsigned short fxvol,
                                unsigned short auxb, float *pgains, unsigned short azimuth,
                                int lfechan);
void SNDI_aztospkrvol(int azimuth, float *pgains);
char *SNDPKTPLAYI_get(int pktchan, int chan, int *pframes, int *pcontinuation);
extern "C" float *MIX_getwetbuffer(int bus);
extern "C" void SNDMIX_setdrygain(int voice, int chan, float gain);
extern "C" void SNDSYS_linkmaincpumixer();
void SNDDRV_DSPMixerInit(int outputchannels);
void SNDDRV_DSPMixerUpdate();
void SNDPKTPLAYI_freeframes(int pktchan, int chan, int frames);
void SNDARAM_init(int maxallocs);

extern "C" {
void AXSetCompressor(unsigned int on);
void ARQInit(void);
unsigned int ARInit(unsigned int *stack_index_addr, unsigned int num_entries);
void AIInit(unsigned char *stack);
}
extern void (*MIXinitfn)(MIXCONFIG *);
extern void (*MIXaudioslicefn)(void *, int);
void SNDPKTPLAYI_flushcallbackdata();
void SNDSYSI_100hzserver();
void SNDSYSI_variabletimerservice();

namespace Snd {
extern volatile int gVariableTimerTick;
extern unsigned char gTotalOutputChannels;
}

extern unsigned short SNDDRV_dolbypl2balances[256][4];
void SNDDRV_DSPMixerSetPan(AXVPB *p, float *pgains, unsigned short azimuth, int lfechan);

static int everyother = 0;
extern "C" int MIX_getframe(int voice);
extern "C" void MIX_filteradd(int voice, int chan, SNDFILTERDEF *psfd);
extern void (*MIXstopfn)(int);
extern "C" void MIX_settimemult(int voice, int timemult);
extern "C" void MIX_sethighpass(int voice, int cutofffreq);
extern "C" void MIX_setlowpass(int voice, float leakage);
extern void (*MIXsetpitchfn)(int, unsigned int);
extern void (*MIXplayinitfn)(int voice, int samplerep, int playtype, void *psample,
                             void *ploopstart, void *ptsdata, int numchan, int totalframes,
                             int sustainstart, int sustainend, void *pcoeftable, void *ploopstate,
                             int platformver, int requester);
extern void (*MIXplayfn)(int voice);
extern "C" void MIX_playinit(int voice, int samplerep, int playtype, void *psample, void *ploopstart,
                  void *ptsdata, int numchan, int totalframes, int sustainstart, int sustainend,
                  void *pcoeftable, void *ploopstate, int platformver, int requester);
extern "C" void MIX_play(int voice);
int SNDPLATFORM_setfxlevel(int voice, int bus);

// total size: 0x8
typedef struct SNDDRVLOOPSTATE {
    unsigned short prevsample1;   // offset 0x0
    unsigned short prevsample2;   // offset 0x2
    unsigned char predictorscale; // offset 0x4
    char pad[3];                  // offset 0x5
} SNDDRVLOOPSTATE;
extern void (*MIXrestorefn)();

enum StartMode {
    COLD = 0,
    WARM = 1,
};

void SNDDRV_freenextframe() {
    int i;

    if (snddrv.numfreenextframe == 0) {
        return;
    }
    for (i = 0; i <= 47; i++) {
        if (snddrv.voice[i].freenextframe != 0) {
            if (snddrv.voice[i].paxvpb != 0) {
                AXFreeVoice(snddrv.voice[i].paxvpb);
            }
            snddrv.voice[i].freenextframe = 0;
            snddrv.numfreenextframe--;
            snddrv.voice[i].paxvpb = 0;
            if (snddrv.numfreenextframe == 0) {
                return;
            }
        }
    }
}

void SNDDRV_mixvoicefree(int voice) {
    SNDVOICEI_free(voice + sndgs.sso.set.voicesdsp);
}

void SNDDRV_callbackdropvoice(void *p) {
    SNDDRVVOICE *pv;
    CHANPUB *ppubv;
    AXVPB *pvpb;
    int voice;
    int i;

    pvpb = (AXVPB *)p;

    ppubv = &sndgs.chan[pvpb->userContext];

    if (ppubv->masterchan == -1) {
        voice = pvpb->userContext;
    } else {
        voice = ppubv->masterchan;
    }

    ppubv = &sndgs.chan[voice];

    for (i = 0; i < ppubv->numchan; i++) {

        if (snddrv.voice[ppubv->voices[i]].paxvpb != 0) {

            if (ppubv->voices[i] == pvpb->userContext) {
                SNDDRV_freeaxvoice(ppubv->voices[i], 1);
            } else {
                SNDDRV_freeaxvoice(ppubv->voices[i], 2);
            }
            snddrv.voice[ppubv->voices[i]].state = 0;
            snddrv.voice[ppubv->voices[i]].paxvpb = 0;
        }
    }
}

int SNDDRV_allocateaxvoice(int voice) {
    int i;
    int j;
    int priority;
    CHANPUB *ppubv;
    SNDDRVVOICE *pv;
    AXVPB *pvpb;
    int lfechan;
    float gain;
    unsigned short fxLevel;

    ppubv = &sndgs.chan[voice];

    // ---
    // ---
    gain = ppubv->finalvol * ppubv->drylevel;

    // ---
    float fx = ppubv->pFxVolume->fxLevel * 32767.0f;
    fxLevel = ppubv->finalvol * fx;

    // ---
    // ---
    // ---
    for (i = 0; i < ppubv->numchan; i++) {

        // ---
        if (ppubv->priority == 0x65) {
            priority = 0x1f;

        } else {
            priority = ppubv->priority / 3;
        }
        if (priority <= 0) {
            priority = 1;
        } else if (priority > 0x1f) {
            priority = 0x1e;
        }

        // ---
        // ---
        snddrv.voice[ppubv->voices[i]].paxvpb = pvpb = AXAcquireVoice(priority, SNDDRV_callbackdropvoice, ppubv->voices[i]);

        // ---
        if (pvpb != 0) {

            // ---
            snddrv.voice[ppubv->voices[i]].freed = 0;

            // ---
            if (i > 4) {

                // ---
                lfechan = 1;
                fxLevel = 0;

            } else {

                // ---
                lfechan = 0;
            }

            // ---
            // ---
            // ---
            // ---
            // ---
            SNDI_aztospkrvol(ppubv->azimuth, snddrv.voice[ppubv->voices[i]].balances);

            // ---
            // ---
            // ---
            // ---
            // ---
            // ---
            // ---
            // ---
            SNDDRV_DSPMixerInitChannel(snddrv.voice[ppubv->voices[i]].paxvpb, 0,
                                       (int)(gain * 258.0f), fxLevel, 0,
                                       snddrv.voice[ppubv->voices[i]].balances, ppubv->azimuth,
                                       lfechan);

            // ---
            // ---
            // ---
            // ---
        } else {

            snddrv.voice[ppubv->voices[i]].freed = 1;
            if (i > 0) {

                for (j = i; j >= 0; j--) {

                    if (snddrv.voice[ppubv->voices[j]].paxvpb != 0) {

                        AXFreeVoice(snddrv.voice[ppubv->voices[j]].paxvpb);

                        snddrv.voice[ppubv->voices[j]].freed = 1;
                        snddrv.voice[ppubv->voices[j]].paxvpb = 0;
                    }
                    if (sndgs.chan[ppubv->voices[j]].status != 0) {
                        SNDVOICEI_free(ppubv->voices[j]);
                    }
                }
            }
            return 0;
        }
    }
    return 1;
}

void SNDDRV_freeaxvoice(int voice, int dropped) {
    CHANPUB *ppubv;
    SNDDRVVOICE *pv;
    AXVPB *tempPB;

    ppubv = &sndgs.chan[voice];
    pv = &snddrv.voice[voice];

    tempPB = pv->paxvpb;

    SNDDRV_DSPMixerReleaseChannel(tempPB);

    pv->sampleaddr = -1;

    if (ppubv->status != 0) {
        SNDVOICEI_free(voice);
    }

    // ---
    if (dropped == 0) {

        if (tempPB != 0) {

            AXFreeVoice(tempPB);
            pv->paxvpb = 0;
        }

        // ---
    } else if (dropped == 2) {

        if (pv->freed == 0 && tempPB != 0) {

            pv->freenextframe = 1;
            snddrv.numfreenextframe++;
        }

        // ---
        // ---
        // ---
        // ---
        // ---
        // ---
        // ---
    } else if (dropped == 1) {

        if (pv->freenextframe == 1) {

            pv->freenextframe = 0;
            snddrv.numfreenextframe--;
        }

        pv->paxvpb = 0;
    }

    // ---
    if (pv->freed == 0) {
        pv->freed = 1;
    }
}

int SNDDRV_dmaslotenter(int handle) {
    int i;
    int old;

    i = 0;
    old = OSDisableInterrupts();
    if (snddrv.dmaslot[i].handle != -1) {
        do {
            i++;
            if (i > 0x3f) {
                break;
            }
        } while (snddrv.dmaslot[i].handle != -1);
    }
    snddrv.dmaslot[i].handle = handle;
    OSRestoreInterrupts(old);
    return i;
}

void SNDDRV_dmcallback(unsigned long dmatask) {
    int i;
    ARQRequest *p;

    p = (ARQRequest *)dmatask;
    snddrv.lastdmaowner = p->owner;
    for (i = 0; i <= 0x3f; i++) {
        if (snddrv.dmaslot[i].handle == p->owner) {
            snddrv.dmaslot[i].handle = -1;
            return;
        }
    }
}

unsigned long SNDDRV_dmapost(unsigned long priority, void *psrc, void *pdst, unsigned long size, int failindex) {
    int old;
    int index;
    unsigned long owner;

    old = OSDisableInterrupts();

    if (failindex == -1) {
        index = SNDDRV_dmaslotenter(snddrv.dmahandle);
        owner = snddrv.dmahandle;

        snddrv.dmaslot[index].priority = priority;
        snddrv.dmaslot[index].psrc = psrc;
        snddrv.dmaslot[index].pdst = pdst;
        snddrv.dmaslot[index].size = (size + 31) & ~31;
        snddrv.dmaslot[index].tick = sndgs.audiotick;

    } else {

        index = failindex;
        owner = snddrv.dmaslot[index].handle;
    }

    ARQPostRequest(&snddrv.dmaslot[index].req, owner, 0, priority, (unsigned int)psrc,
                   (unsigned int)pdst, (size + 31) & ~31, SNDDRV_dmcallback);

    OSRestoreInterrupts(old);

    return snddrv.dmahandle++;
}

void SNDDRV_restoredma(int handle) {
    int i;
    int old;

    old = OSDisableInterrupts();
    for (i = 0; i <= 0x3f; i++) {
        if (snddrv.dmaslot[i].handle == handle) {
            ARQRemoveRequest(&snddrv.dmaslot[i].req);
            SNDDRV_dmapost(snddrv.dmaslot[i].priority, snddrv.dmaslot[i].psrc,
                           snddrv.dmaslot[i].pdst, snddrv.dmaslot[i].size, i);
        }
    }
    OSRestoreInterrupts(old);
}

void SNDDRV_audiocallback(void *data, void *context) {
    SNDDRVAUXABUFFERS *auxA;
    static void *ptr;
    float *pdst;
    int i;
    int j;
    int *psrc;
    int old;

    auxA = (SNDDRVAUXABUFFERS *)data;

    old = OSEnableInterrupts();

    // ---
    if (snddrv.mixerinited) {

        for (j = 0; j < Snd::gMaxFxBuses; j++) {

            if (sndmix.pfxbus[j] != 0) {

                // ---
                ptr = MIX_getwetbuffer(j);
                pdst = (float *)ptr;

                // ---
                psrc = auxA->auxbufs[0];

                // ---
                // ---
                // ---
                for (i = 0; i <= 0x9f; i++) {
                    pdst[i] = psrc[i];
                }
            }
        }

        // ---
        // ---
        // ---
        if (sndgs.sso.set.outputchannels <= 3) {

            // ---
            Snd::Util::MemCpy(auxA->auxbufs[1], snddrv.pmixbuf[0], 0x280);
            Snd::Util::MemCpy(auxA->auxbufs[0], snddrv.pmixbuf[1], 0x280);

            // ---
            // ---
            // ---
        } else {

            Snd::Util::MemCpy(auxA->auxbufs[1], snddrv.pmixbuf[0], 0x280);

            Snd::Util::MemCpy(auxA->auxbufs[2], snddrv.pmixbuf[2], 0x280);
            Snd::Util::MemCpy(auxA->auxbufs[0], snddrv.pmixbuf[3], 0x280);

            Snd::Util::MemCpy(auxA->auxbufs[3], snddrv.pmixbuf[1], 0x280);
        }
    }

    // ---
    // ---
    OSWakeupThread(&snddrv.threadqueue);
    everyother++;

    // ---
    OSRestoreInterrupts(old);
}

void SNDDRV_initmixer() {
    int i;
    MIXCONFIG mc;

    SNDSYS_linkmaincpumixer();

    mc.inputchannels = sndgs.sso.set.voicesmaincpu;

    // ---
    // ---
    if (sndgs.sso.set.outputchannels != 1) {
        mc.outputchannels = sndgs.sso.set.outputchannels;

    } else {
        mc.outputchannels = 2;
    }

    mc.outputrate = sndgs.sso.set.outputrate;
    mc.chanovercallback = SNDDRV_mixvoicefree;
    mc.outputinterleaved = 0;
    MIXinitfn(&mc);

    SNDSYS_entercritical();

    for (i = 0; i < mc.outputchannels; i++) {

        // ---
        snddrv.mixbuf[i] = SNDMEMI_allocz(0x2a0);

        snddrv.pmixbuf[i] = (void *)(((unsigned int)snddrv.mixbuf[i] + 31) & ~31);
    }

    SNDSYS_leavecritical();

    // ---
    snddrv.mixerinited = 1;
}

void SNDDRV_restoremixer() {
    int i;

    snddrv.mixerinited = 0;
    SNDSYS_entercritical();
    for (i = 0; i < sndgs.sso.set.outputchannels; i++) {
        SNDMEMI_free(snddrv.mixbuf[i]);
    }
    SNDSYS_leavecritical();
    MIXrestorefn();
}

void SNDDRV_fillbufwithpackets(int pktchan, SNDDRVPKTCHAN *pc, CHANPUB *ppubv) {
    int continuation;
    int packetframes;
    int packetbytesremaining;
    int maxbytes;
    int i;
    int dmabytesremaining;
    void *psrc[6];

    dmabytesremaining = 0x600;

    // ---
    // ---
    while (dmabytesremaining > 0) {

        if (pc->ppacket[0] == 0) {

            for (i = 0; i < ppubv->numchan; i++) {
                // ---
                pc->ppacket[i] = SNDPKTPLAYI_get(pktchan, i, &packetframes, &continuation);
            }
            // ---
            pc->packetxferoffset = 0;
            // ---
            pc->packetbytestotal = packetframes << 1;
        }
        // ---
        // ---
        maxbytes = dmabytesremaining;
        // ---
        if (pc->ppacket[0] != 0) {

            // ---
            // ---
            packetbytesremaining = pc->packetbytestotal - pc->packetxferoffset;

            // ---
            // ---
            if (packetbytesremaining <= 0) {
                // ---
                pc->ppacket[0] = 0;
                continue;
            }
            // ---
            // ---
            if (dmabytesremaining > packetbytesremaining) {
                maxbytes = packetbytesremaining;
            }
            // ---
            // ---
            pc->framesinbuf[pc->playbuf] = pc->framesinbuf[pc->playbuf] + (maxbytes >> 1);

            // ---
            for (i = 0; i < ppubv->numchan; i++) {
                psrc[i] = pc->ppacket[i] + pc->packetxferoffset;
            }
            // ---
            pc->packetxferoffset += maxbytes;

            // ---
            // ---
        } else {

            for (i = 0; i < ppubv->numchan; i++) {
                psrc[i] = snddrv.pzerobuf;
            }
        }
        // ---
        // ---
        // ---
        for (i = 0; i < ppubv->numchan; i++) {

            // ---
            // ---
            // ---
            Snd::Util::MemCpy(&pc->pxferbuf[i][0x600 - dmabytesremaining], psrc[i], maxbytes);
        }
        // ---
        dmabytesremaining -= maxbytes;
    }

    // ---
    // ---
    for (i = 0; i < ppubv->numchan; i++) {

        // ---
        DCFlushRange(pc->pxferbuf[i], 0x600);

        // ---
        // ---
        SNDDRV_dmapost(1, pc->pxferbuf[i], (void *)(pc->aramaddr[i] + pc->playbuf * 0x600), 0x600, -1);
    }
}

void *SNDDRV_thread(void *param) {

    for (;;) {

        Snd::gMutexLockFn();

        // ---
        if (sndgs.profileenter) {
            sndgs.profileenter();
        }

        if (snddrv.mixerinited) {

            MIXaudioslicefn(snddrv.pmixbuf, 0xa0);
            SNDPKTPLAYI_flushcallbackdata();
        }

        // ---
        // ---
        if (everyother & 1) {

            SNDSYSI_100hzserver();

            // ---
            // ---
            // ---
            // ---
            // ---
            // ---
            // ---
            // ---
            // ---
            // ---
            static float curTick = 0.1f;
            Snd::gVariableTimerTick = SNDI_ftoiround(curTick += snddrv.updateperiod);

            // ---
            // ---
            // ---
            // ---
            if (curTick > 30.0f) {
                curTick -= 30.0f;
            }

            // ---
            SNDSYSI_variabletimerservice();
        }

        // ---
        if (sndgs.profileleave) {
            sndgs.profileleave();
        }

        Snd::gMutexUnlockFn();
        OSSleepThread(&snddrv.threadqueue);
    }
}

int SNDPLATFORM_outputcaps() {
    sndgs.sso.cap.voicesmaincpumax = 0x20;
    sndgs.sso.cap.voicesdspmax = 0x30;

    sndgs.sso.cap.outputratemin = 32000;
    sndgs.sso.cap.outputratemax = 32000;
    sndgs.sso.cap.outputchannelsmin = 1;
    sndgs.sso.cap.outputchannelsmax = 4;
    sndgs.sso.set.outputspdifmode = 1;

    sndgs.sso.cap.numrendermodes = 2;
    sndgs.sso.cap.rendermode[0] = 0x220;
    sndgs.sso.cap.rendermode[1] = 0x24;

    sndgs.sso.set.numrendermodes = 2;
    sndgs.sso.set.rendermode[0] = 0x220;
    sndgs.sso.set.rendermode[1] = 0x24;

    sndgs.sso.set.voicesmaincpu = 8;
    sndgs.sso.set.voicesdsp = 0x30;
    sndgs.sso.set.maxstreams = 8;
    sndgs.sso.set.outputrate = 32000;
    sndgs.sso.set.outputchannels = 2;

    sndgs.sso.set.initaram = 1;

    sndgs.sso.set.arampooladdr = ARGetBaseAddress() + 0x100;

    sndgs.sso.set.arampoolsize = 0x800000 - sndgs.sso.set.arampooladdr;

    return 0;
}

int SNDPLATFORM_outputset() {
    SNDSYSSET set;

    set = sndgs.sso.set;

    if (sndgs.installed) {
        sndgs.sso.set = sndgs.prevset;
    }

    set.outputrate = 32000;

    if (set.outputchannels < sndgs.sso.cap.outputchannelsmin) {
        set.outputchannels = sndgs.sso.cap.outputchannelsmin;
    }
    if (set.outputchannels > sndgs.sso.cap.outputchannelsmax) {
        set.outputchannels = sndgs.sso.cap.outputchannelsmax;
    }
    if (set.voicesmaincpu > sndgs.sso.cap.voicesmaincpumax) {
        set.voicesmaincpu = sndgs.sso.cap.voicesmaincpumax;
    }
    if (set.voicesdsp > sndgs.sso.cap.voicesdspmax) {
        set.voicesdsp = sndgs.sso.cap.voicesdspmax;
    }
    if (set.maxstreams > 32) {
        set.maxstreams = 32;
    }
    if (sndgs.installed) {
        return SND_OK;
    }

    sndgs.voicestotal = set.voicesmaincpu + set.voicesdsp;
    sndgs.sso.set = set;

    return SND_OK;
}

int SNDPLATFORM_init(StartMode startMode) {
    SNDDRVVOICE *pv;
    int i;
    static int __attribute__((aligned(8))) araminited = 0;
    int allocCount;

    SNDDRV_DSPMixerInit(sndgs.sso.set.outputchannels);

    // ---
    // ---
    // ---
    // ---
    Snd::gVariableTimerPeriod = sndgs.sso.set.updateperiod;

    // ---
    // ---
    // ---
    if (sndgs.sso.set.updateperiod == -1.0f) {
        Snd::gVariableTimerPeriod = 20.0f;
    }

    Snd::gVariableTimerPeriod = SNDI_clipf(Snd::gVariableTimerPeriod, 10.0f, 100.0f);

    // ---
    // ---
    // ---
    snddrv.updateperiod = 10.0f / Snd::gVariableTimerPeriod;

    if (sndgs.sso.set.initaram && araminited == 0) {
        ARInit(0, 0);
        ARQInit();
        araminited = 1;
    }

    for (i = 0; i <= 0x3f; i++) {
        snddrv.dmaslot[i].handle = -1;
    }

    if (startMode == COLD) {

        if (sndgs.sso.vec.aramalloc == 0 || sndgs.sso.vec.aramfree == 0) {

            if (sndgs.sso.set.maxstreams > sndgs.sso.set.voicesdsp) {
                allocCount = sndgs.sso.set.voicesdsp;
            } else {
                allocCount = sndgs.sso.set.maxstreams;
            }
            allocCount += 2;
            SNDARAM_init(sndgs.sso.set.maxbanks + allocCount);
            SNDARAM_setpool(sndgs.sso.set.arampooladdr, sndgs.sso.set.arampoolsize);
        }

        snddrv.aramaddr = SNDPLATFORM_memalloc(0, 0x500);
        snddrv.aramaddrhalf = snddrv.aramaddr >> 1;
        snddrv.aramaddrdouble = snddrv.aramaddr << 1;
        snddrv.pzerobuf = (char *)(((unsigned int)snddrv.zerobuf + 31) & ~31);
        SNDDRV_dmapost(1, snddrv.pzerobuf, (void *)snddrv.aramaddr, 0x500, -1);
        SNDDRV_dmapost(1, snddrv.pzerobuf, (void *)ARGetBaseAddress(), 0x100, -1);
    }

    AIInit(0);
    AXInit();
    AXSetCompressor(0);

    if (sndgs.sso.set.outputchannels > 2) {
        AXSetMode(2);
        sndgs.sso.set.outputchannels = 4;
    } else {
        AXSetMode(0);
    }

    for (i = 0; i <= 0x2f; i++) {
        pv = &snddrv.voice[i];
        pv->sampleaddr = -1;
        pv->pktchan = -1;
        pv->paxvpb = 0;
        pv->state = 0;
        pv->freed = 1;
        pv->freenextframe = 0;
    }

    for (i = 0; i < sndgs.sso.set.numrendermodes; i++) {
        if (sndgs.sso.set.rendermode[i] & 4) {
            if (snddrv.mixerinited == 0) {
                SNDDRV_initmixer();
                break;
            }
        }
    }

    snddrv.numfreenextframe = 0;

    OSCreateThread(&snddrv.thread, SNDDRV_thread, 0, &snddrv.threadstack[0x8000], 0x8000, 1, 1);
    OSInitThreadQueue(&snddrv.threadqueue);
    OSResumeThread(&snddrv.thread);

    AXRegisterAuxACallback(SNDDRV_audiocallback, 0);

    sndgs.installed = 1;
    return SND_OK;
}

int SNDPLATFORM_restore(StartMode startMode) {
    AXRegisterAuxACallback(0, 0);
    if (snddrv.mixerinited != 0) {
        SNDDRV_restoremixer();
    }
    if (startMode == COLD) {
        SNDARAM_restore();
    }
    while (sndgs.incritical != 0) {
        SNDSYS_leavecritical();
    }
    OSCancelThread(&snddrv.thread);
    AXQuit();
    return 0;
}

void iSNDserve() {
    AXPBADPCM adpcm;
    AXPBADPCMLOOP adpcmloop;
    AXPBADDR addr;
    SNDDRVPKTCHAN *pc;
    CHANPUB *ppubv;
    SNDDRVVOICE *pv;
    unsigned int loopstartaddr;
    unsigned int endaddr;
    int pos;
    int i;
    int j;

    for (i = 0; i < sndgs.sso.set.maxstreams; i++) {

        pc = snddrv.pktchan[i];
        if (pc == 0) {
            continue;
        }
        if (pc->mastervoice < 0) {
            continue;
        }
        ppubv = &sndgs.chan[pc->mastervoice];
        pv = &snddrv.voice[pc->mastervoice];

        if ((pc->prebuffer & 0x80) == 0) {
            pc->prebuffer--;
            if (pc->prebuffer & 0x80) {

                addr.loopFlag = 1;
                addr.format = 10;
                adpcm.gain = 0x800;
                memset(&adpcmloop, 0, sizeof(adpcmloop));

                SNDPLATFORM_setpitch(pc->mastervoice);

                for (j = 0; j < ppubv->numchan; j++) {

                    Snd::Hal::SetPan(ppubv->voices[j]);
                    Snd::Hal::SetVol(ppubv->voices[j]);

                    loopstartaddr = pc->aramaddr[j] / 2;
                    endaddr = (pc->aramaddr[j] + 0xbfe) / 2;

                    AXSetVoiceAdpcm(snddrv.voice[ppubv->voices[j]].paxvpb, &adpcm);
                    AXSetVoiceAdpcmLoop(snddrv.voice[ppubv->voices[j]].paxvpb, &adpcmloop);

                    addr.currentAddressHi = loopstartaddr >> 16;
                    addr.currentAddressLo = loopstartaddr;
                    addr.endAddressHi = endaddr >> 16;
                    addr.endAddressLo = endaddr;
                    addr.loopAddressHi = loopstartaddr >> 16;
                    addr.loopAddressLo = loopstartaddr;

                    AXSetVoiceAddr(snddrv.voice[ppubv->voices[j]].paxvpb, &addr);
                    AXSetVoiceState(snddrv.voice[ppubv->voices[j]].paxvpb, 1);
                    snddrv.voice[ppubv->voices[j]].state = 1;
                }
            }
        }

        if (pv->paxvpb == 0) {
            continue;
        }
        pos = (pv->paxvpb->pb.addr.currentAddressHi << 16) + pv->paxvpb->pb.addr.currentAddressLo;

        pos = (pos > (int)pc->aramaddrhalf);

        if (pos == pc->playbuf) {
            continue;
        }
        if (pc->framesinbuf[pc->playbuf] != 0) {
            for (j = 0; j < ppubv->numchan; j++) {
                SNDPKTPLAYI_freeframes(i, j, pc->framesinbuf[pc->playbuf]);
            }
            pc->framesinbuf[pc->playbuf] = 0;
        }
        SNDDRV_fillbufwithpackets(i, pc, ppubv);
        pc->playbuf = pos;
    }

    SNDPKTPLAYI_flushcallbackdata();

    for (i = 0; i < sndgs.sso.set.voicesdsp; i++) {

        pv = &snddrv.voice[i];
        ppubv = &sndgs.chan[i];

        if (pv->paxvpb == 0) {
            continue;
        }
        if (pv->state == 0) {
            continue;
        }
        if (pv->paxvpb->pb.state != 0) {
            continue;
        }
        if (pv->sampleaddr < 0) {
            continue;
        }
        if (pv->state != 1) {
            continue;
        }
        for (j = 0; j < sndgs.chan[i].numchan; j++) {
            if (snddrv.voice[ppubv->voices[j]].state != 0) {
                SNDDRV_freeaxvoice(ppubv->voices[j], 0);
                snddrv.voice[ppubv->voices[j]].state = 0;
            }
        }
    }

    SNDDRV_freenextframe();
    SNDDRV_DSPMixerUpdate();
}

void SNDPLATFORM_getvoicerange(int playloc, int *minvoicerange, int *maxvoicerange) {
    if (playloc & SND_PLAYLOC_DSP) {
        *minvoicerange = 0;
        *maxvoicerange = sndgs.sso.set.voicesdsp;
    } else if (playloc & SND_PLAYLOC_MAINCPU) {
        *minvoicerange = sndgs.sso.set.voicesdsp;
        *maxvoicerange = sndgs.voicestotal;
    }
}

int SNDPLATFORM_asyncresolvetimbre(SNDIPATCHHEADER *pph, char *pdata, int *pfirstoffset) {
    if (*pfirstoffset == 0) {
        *pfirstoffset = pph->sampleoffset[0];
    }
    *pph->paudioramaddr = (int)(pdata + (pph->sampleoffset[0] - *pfirstoffset));
    return 0;
}

int SNDPLATFORM_playtimbre(SNDIPATCHHEADER *pph, void *psampledata, int voice, int timemult,
                           int lowpasscutoff, int highpasscutoff) {
    CHANPUB *ppubv;
    SNDDRVLOOPSTATE *pls;
    SNDDRVVOICE *pv;
    AXPBADDR addr;
    AXPBADPCM adpcm;
    AXPBADPCMLOOP adpcmloop;
    unsigned int startaddr;
    unsigned int loopstartaddr;
    unsigned int endaddr;
    unsigned int samplebytes;
    unsigned int startoff;
    unsigned int loopstartoff;
    unsigned int endoff;
    int i;

    ppubv = &sndgs.chan[voice];
    pv = &snddrv.voice[voice];

    samplebytes = 0;
    startoff = 0;

    ppubv->samplerate = pph->samplerate;
    ppubv->frames = pph->totalframes;
    ppubv->numchan = pph->numchan;

    if (ppubv->rendermode & 0x200) {

        memset(&adpcmloop, 0, sizeof(adpcmloop));

        if (pph->sustainend >= 0) {
            addr.loopFlag = 1;
            endoff = pph->sustainend;
            loopstartoff = pph->sustainstart;
        } else {
            addr.loopFlag = 0;
            if (ppubv->samplerep == 18) {
                loopstartoff = snddrv.aramaddrdouble;
            } else {
                loopstartoff = snddrv.aramaddrhalf;
            }
            endoff = pph->totalframes - 1;
        }

        if (ppubv->samplerep == 18) {
            addr.format = 0;
            adpcm.gain = 0;
            adpcm.yn1 = 0;
            adpcm.yn2 = 0;
            startoff = *pph->paudioramaddr << 1;
            samplebytes = ((unsigned int)(pph->totalframes + 13) / 14 * 14 << 3) / 7;
            loopstartoff = loopstartoff * 8 / 7;
            endoff = endoff * 8 / 7;

        } else if (ppubv->samplerep == 7) {
            addr.format = 10;
            adpcm.gain = 0x800;
            samplebytes = pph->totalframes;
            startoff = (unsigned int)*pph->paudioramaddr >> 1;
        }

        pv->sampleaddr = startoff;

        if (SNDDRV_allocateaxvoice(voice) == 0) {
            return SNDERR_ALLOCATE;
        }

        for (i = 0; i < ppubv->numchan; i++) {
            Snd::Hal::SetPan(ppubv->voices[i]);
            Snd::Hal::SetVol(ppubv->voices[i]);
            SNDPLATFORM_setfxlevel(ppubv->voices[i], 0);
        }

        SNDPLATFORM_setpitch(voice);

        for (i = 0; i < pph->numchan; i++) {

            if (ppubv->samplerep == 18) {
                Snd::Util::MemCpy(&adpcm, pph->pcodebookdata[i], 32);
                adpcm.pred_scale = ((unsigned char *)pph->pcodebookdata[i])[32];
                if (pph->sustainend > 0) {
                    pls = (SNDDRVLOOPSTATE *)pph->ploopstatedata[i];
                    adpcmloop.loop_pred_scale = pls->predictorscale;
                    adpcmloop.loop_yn1 = pls->prevsample1;
                    adpcmloop.loop_yn2 = pls->prevsample2;
                }
            }

            startaddr = startoff + samplebytes * i;
            if (pph->sustainend > 0) {
                loopstartaddr = startaddr + loopstartoff;
            } else {
                loopstartaddr = loopstartoff;
            }
            endaddr = startaddr + endoff;
            endaddr -= 2;

            if (ppubv->samplerep == 18) {
                startaddr += 2;
                if ((loopstartaddr & 0xf) == 0) {
                    loopstartaddr += 2;
                } else if ((loopstartaddr & 0xf) == 1) {
                    loopstartaddr += 1;
                }
                if ((endaddr & 0xf) == 0) {
                    endaddr += 2;
                } else if ((endaddr & 0xf) == 1) {
                    endaddr += 1;
                }
            }

            AXSetVoiceSrcType(snddrv.voice[ppubv->voices[i]].paxvpb, 1);
            AXSetVoiceAdpcm(snddrv.voice[ppubv->voices[i]].paxvpb, &adpcm);
            AXSetVoiceAdpcmLoop(snddrv.voice[ppubv->voices[i]].paxvpb, &adpcmloop);

            addr.endAddressHi = endaddr >> 16;
            addr.endAddressLo = endaddr;
            addr.currentAddressHi = startaddr >> 16;
            addr.currentAddressLo = startaddr;
            addr.loopAddressHi = loopstartaddr >> 16;
            addr.loopAddressLo = loopstartaddr;

            AXSetVoiceAddr(snddrv.voice[ppubv->voices[i]].paxvpb, &addr);
            AXSetVoiceState(snddrv.voice[ppubv->voices[i]].paxvpb, 1);
            snddrv.voice[ppubv->voices[i]].state = 1;
        }

    } else {

        for (i = 0; i < ppubv->numchan; i++) {

            MIX_playinit(ppubv->voices[i] - sndgs.sso.set.voicesdsp, pph->samplerep, 1,
                         (char *)psampledata + pph->sampleoffset[i],
                         (char *)psampledata + pph->sampleoffset[0] + pph->loopoffset[i],
                         pph->ptimestretchdata[i], ppubv->numchan, ppubv->frames,
                         pph->sustainstart, pph->sustainend, 0, 0, pph->platformver, i);

            Snd::Hal::SetPan(ppubv->voices[i]);
        }

        SNDPLATFORM_setpitch(ppubv->voices[0]);
        SNDPLATFORM_timemult(ppubv->voices[0], timemult);
        SNDPLATFORM_lowpass(ppubv->voices[0], lowpasscutoff);
        SNDPLATFORM_highpass(ppubv->voices[0], highpasscutoff);

        for (i = 0; i < ppubv->numchan; i++) {
            MIX_play(ppubv->voices[i] - sndgs.sso.set.voicesdsp);
        }
    }
    return SND_OK;
}

int SNDDRV_getmastervoice(int voice) {
    CHANPUB *pv;

    voice += sndgs.sso.set.voicesdsp;
    pv = &sndgs.chan[voice];
    if (pv->masterchan == -1) {
        return voice;
    }
    return pv->masterchan;
}

int SNDDRV_getsamplechan(int voice) {
    CHANPUB *pv;
    int i;

    voice += sndgs.sso.set.voicesdsp;
    pv = &sndgs.chan[voice];
    if (pv->masterchan == -1) {
        return 0;
    }
    pv = &sndgs.chan[pv->masterchan];
    for (i = 1; i <= 5; i++) {
        if (pv->voices[i] == voice) {
            return i;
        }
    }
    return 0;
}

int SNDPLATFORM_packetoverhead() {
    return 0x25e4;
}

int SNDPLATFORM_packetplaycreate(int pktchan, void *pmem) {
    SNDDRVPKTCHAN *pc;
    int i;

    pc = (SNDDRVPKTCHAN *)pmem;
    memset(pc, 0, sizeof(SNDDRVPKTCHAN));
    pc->aramaddr[0] = SNDPLATFORM_memalloc(0, 0x4800);
    if (pc->aramaddr[0] == 0) {
        return SNDERR_ALLOCATE;
    }
    for (i = 0; i <= 5; i++) {
        pc->aramaddr[i] = pc->aramaddr[0] + i * 0xc00;
        pc->pxferbuf[i] = (char *)(((unsigned int)pc->xferbuf[i] + 31) & ~31);
    }
    pc->mastervoice = -1;
    snddrv.pktchan[pktchan] = pc;
    return 0;
}

int SNDPLATFORM_packetplay(int pktchan, int voice, int timemult, int lowpasscutoff,
                           int highpasscutoff, SNDSAMPLEFORMAT *pssf, SNDSAMPLEATTR *pssa) {
    SNDDRVPKTCHAN *pc;
    CHANPUB *ppubv;
    SNDDRVVOICE *pv;
    unsigned int loopstartaddr;
    unsigned int endaddr;
    int i;

    ppubv = &sndgs.chan[voice];
    pv = &snddrv.voice[voice];

    ppubv->samplerate = pssf->samplerate;
    ppubv->frames = 0;
    pv->pktchan = pktchan;
    ppubv->numchan = pssf->channels;

    if (ppubv->rendermode & 0x4) {

        for (i = 0; i < ppubv->numchan; i++) {
            MIXplayinitfn(ppubv->voices[i] - sndgs.sso.set.voicesdsp, ppubv->samplerep, 0, 0, 0,
                          pssa->ptsdata[i], ppubv->numchan, 0, -1, -1, 0, 0, pssa->platformver, i);
        }

        SNDPLATFORM_setpitch(ppubv->voices[0]);

        for (i = 0; i < ppubv->numchan; i++) {
            Snd::Hal::SetPan(ppubv->voices[i]);
            Snd::Hal::SetVol(ppubv->voices[i]);
        }

        SNDPLATFORM_timemult(ppubv->voices[0], timemult);
        SNDPLATFORM_lowpass(ppubv->voices[0], lowpasscutoff);
        SNDPLATFORM_highpass(ppubv->voices[0], highpasscutoff);

        for (i = 0; i < ppubv->numchan; i++) {
            MIXplayfn(ppubv->voices[i] - sndgs.sso.set.voicesdsp);
        }

    } else {

        pc = snddrv.pktchan[pktchan];

        for (i = 0; i <= 5; i++) {
            pc->framesinbuf[i] = 0;
        }
        pc->ppacket[0] = 0;
        pc->prebuffer = 2;
        pc->playbuf = 1;
        pc->mastervoice = voice;
        pc->numchannels = pssf->channels;

        SNDDRV_allocateaxvoice(voice);

        for (i = 0; i < pssf->channels; i++) {

            loopstartaddr = pc->aramaddr[i] / 2;
            endaddr = (pc->aramaddr[i] + 0xbfe) / 2;

            if (i == 0) {
                pc->aramaddrhalf = (loopstartaddr + endaddr) / 2;
            }

            memset(pc->pxferbuf[i], 0, 0x600);
            DCFlushRange(pc->pxferbuf[i], 0x600);
            SNDDRV_dmapost(1, pc->pxferbuf[i], (void *)pc->aramaddr[i], 0x600, -1);

            AXSetVoiceSrcType(snddrv.voice[ppubv->voices[i]].paxvpb, 1);
            snddrv.voice[ppubv->voices[i]].state = 1;
        }
    }
    return SND_OK;
}

int SNDPLATFORM_packetplaydestroy(int pktchan) {
    SNDDRVPKTCHAN *pc;
    int i;

    pc = snddrv.pktchan[pktchan];
    SNDPLATFORM_memfree(0, pc->aramaddr[0]);
    for (i = 0; i <= 5; i++) {
        pc->aramaddr[i] = 0;
    }
    snddrv.pktchan[pktchan] = 0;
    return 0;
}

int SNDPLATFORM_outputlatency() {
    return SNDERR_UNSUPPORTED;
}

void Snd::Hal::SetVolInternal(int voice) {
    CHANPUB *ppubv;
    SNDDRVVOICE *pHalVoice;
    int j;
    int nummixerchannels;
    float gain;

    ppubv = &sndgs.chan[voice];
    pHalVoice = &snddrv.voice[voice];

    if (ppubv->rendermode & 0x200) {
        gain = ppubv->finalvol * ppubv->drylevel;
        SNDDRV_DSPMixerSetVol(snddrv.voice[voice].paxvpb, (int)(gain * 258.0f));
    } else {
        gain = ppubv->finalvol * ppubv->drylevel * (1.0f / 127.0f);
        if (sndgs.sso.set.outputchannels == 1) {
            nummixerchannels = 2;
        } else {
            nummixerchannels = sndgs.sso.set.outputchannels;
        }
        for (j = 0; j < nummixerchannels; j++) {
            SNDMIX_setdrygain(voice - sndgs.sso.set.voicesdsp, j, gain * pHalVoice->balances[j]);
        }
    }
}

void Snd::Hal::SetDry(int voice) {
    CHANPUB *ppubv;

    ppubv = &sndgs.chan[voice];
    if (ppubv->rendermode & 0x4 || ppubv->rendermode & 0x200) {
        SetVol(voice);
    }
}

void Snd::Hal::SetPan(int voice) {
    CHANPUB *pVoice;
    SNDDRVVOICE *pHalVoice;
    int i;

    pVoice = &sndgs.chan[voice];
    pHalVoice = &snddrv.voice[voice];

    if (pVoice->isLfe) {
        for (i = 0; i < Snd::gTotalOutputChannels; i++) {
            pHalVoice->balances[i] = 0.0f;
        }
    } else if (Snd::gFoldDownTarget == 0) {
        pHalVoice->balances[0] = 0.7071068f;
        pHalVoice->balances[1] = 0.7071068f;
        pHalVoice->balances[2] = 0.0f;
        pHalVoice->balances[3] = 0.0f;
        pHalVoice->balances[4] = 0.0f;
        pHalVoice->balances[5] = 0.0f;
    } else {
        SNDI_aztospkrvol(pVoice->azimuth, pHalVoice->balances);
    }

    if ((pVoice->rendermode & 0x4) && sndgs.sso.set.outputchannels > 2 && pVoice->isLfe == 0) {
        int azimuth = pVoice->azimuth >> 8;

        pHalVoice->balances[0] = (float)SNDDRV_dolbypl2balances[azimuth][1] * (1.0f / 32767.0f);
        pHalVoice->balances[1] = (float)SNDDRV_dolbypl2balances[azimuth][3] * (1.0f / 32767.0f);
        pHalVoice->balances[2] = (float)SNDDRV_dolbypl2balances[azimuth][2] * (1.0f / 32767.0f);
        pHalVoice->balances[3] = (float)SNDDRV_dolbypl2balances[azimuth][0] * (1.0f / 32767.0f);
        pHalVoice->balances[5] = 0.0f;
    }

    if (pVoice->rendermode & 0x200) {
        SNDDRV_DSPMixerSetPan(pHalVoice->paxvpb, pHalVoice->balances, pVoice->azimuth,
                              pVoice->isLfe);
    } else {
        SetVol(voice);
    }
}

int SNDPLATFORM_setpitch(int voice) {
    CHANPUB *ppubv;
    SNDDRVVOICE *pv;
    float floatpitch;
    unsigned int pitch;
    int i;

    ppubv = &sndgs.chan[voice];

    if (ppubv->rendermode & 0x200) {

        pitch = (ppubv->samplerate << 12) / sndgs.sso.set.outputrate;
        pitch = (pitch * ppubv->finalpitch) >> 12;
        floatpitch = pitch * (1.0f / 4096.0f);

        for (i = 0; i < ppubv->numchan; i++) {
            if (snddrv.voice[ppubv->voices[i]].paxvpb == 0) {
                return SNDERR_INV_PARAM;
            }
            AXSetVoiceSrcRatio(snddrv.voice[ppubv->voices[i]].paxvpb, floatpitch);
        }

    } else {

        pitch = (ppubv->samplerate << 15) / sndgs.sso.set.outputrate;
        pitch = (pitch * ppubv->finalpitch) >> 11;

        for (i = 0; i < ppubv->numchan; i++) {
            MIXsetpitchfn(ppubv->voices[i] - sndgs.sso.set.voicesdsp, pitch);
        }
    }
    return 0;
}

int SNDPLATFORM_timemult(int voice, int timemult) {
    CHANPUB *ppubv;
    int i;

    ppubv = &sndgs.chan[voice];
    if (ppubv->rendermode & 0x4) {
        for (i = 0; i < ppubv->numchan; i++) {
            MIX_settimemult(ppubv->voices[i] - sndgs.sso.set.voicesdsp, timemult);
        }
    }
    return 0;
}

void SNDPLATFORM_lowpass(int voice, int cutofffreq) {
    CHANPUB *ppubv;
    int i;
    float leakage;

    ppubv = &sndgs.chan[voice];
    if (ppubv->rendermode & 0x4) {
        leakage = cutofffreq * (1.0f / ((float)sndgs.sso.set.outputrate * 0.5f));
        for (i = 0; i < ppubv->numchan; i++) {
            MIX_setlowpass(ppubv->voices[i] - sndgs.sso.set.voicesdsp, leakage);
        }
    }
}

void SNDPLATFORM_highpass(int voice, int highpasscutoff) {
    CHANPUB *ppubv;
    int i;

    ppubv = &sndgs.chan[voice];
    if (ppubv->rendermode & 0x4) {
        for (i = 0; i < ppubv->numchan; i++) {
            MIX_sethighpass(ppubv->voices[i] - sndgs.sso.set.voicesdsp, highpasscutoff);
        }
    }
}

int SNDPLATFORM_getcurframe(int voice) {
    SNDDRVVOICE *pv;
    CHANPUB *ppubv;
    int addr;

    ppubv = &sndgs.chan[voice];
    pv = &snddrv.voice[voice];
    if (ppubv->rendermode & 0x200) {
        if (pv->paxvpb == 0) {
            return SNDERR_INV_PARAM;
        }
        addr = (pv->paxvpb->pb.addr.currentAddressHi << 16) + pv->paxvpb->pb.addr.currentAddressLo;
        if (ppubv->samplerep == 18) {
            return ((addr - pv->sampleaddr) * 7) >> 3;
        }
        return addr - pv->sampleaddr;
    }
    return MIX_getframe(voice - sndgs.sso.set.voicesdsp);
}

int SNDPLATFORM_stop(int voice) {
    CHANPUB *ppubv;
    SNDDRVVOICE *pv;
    AXVPB *ppb;
    int i;

    ppubv = &sndgs.chan[voice];
    pv = &snddrv.voice[voice];
    if (ppubv->rendermode & 0x200) {
        if (pv->pktchan >= 0) {
            snddrv.pktchan[pv->pktchan]->mastervoice = -1;
            pv->pktchan = -1;
        }
        for (i = 0; i < ppubv->numchan; i++) {
            ppb = snddrv.voice[ppubv->voices[i]].paxvpb;
            if (ppb == 0) {
                return SNDERR_INV_PARAM;
            }
            AXSetVoiceState(ppb, 0);
            if (snddrv.voice[ppubv->voices[i]].state != 0) {
                SNDDRV_freeaxvoice(ppubv->voices[i], 0);
                snddrv.voice[ppubv->voices[i]].state = 0;
            }
        }
    } else {
        for (i = 0; i < ppubv->numchan; i++) {
            MIXstopfn(ppubv->voices[i] - sndgs.sso.set.voicesdsp);
            SNDVOICEI_free(ppubv->voices[i]);
        }
    }
    return 0;
}

int SNDPLATFORM_filteradd(int voice, SNDFILTERDEF *psfd) {
    CHANPUB *ppubv;
    int i;

    ppubv = &sndgs.chan[voice];
    if (ppubv->rendermode & 0x4) {
        for (i = 0; i < ppubv->numchan; i++) {
            MIX_filteradd(ppubv->voices[i] - sndgs.sso.set.voicesdsp, i, psfd);
        }
        return 0;
    }
    return SNDERR_FORMAT;
}

unsigned int SNDPLATFORM_memalloc(int playloc, int size) {
    size = (size + 31) & ~31;
    if (sndgs.sso.vec.aramalloc == 0) {
        size = SNDARAM_alloc(size);
    } else {
        size = sndgs.sso.vec.aramalloc(size);
    }
    return size;
}

unsigned int SNDPLATFORM_memfree(int playloc, unsigned int addr) {
    if (sndgs.sso.vec.aramfree == 0) {
        SNDARAM_free(addr);
    } else {
        sndgs.sso.vec.aramfree(addr);
    }
    return 0;
}

int SNDPLATFORM_memlimits(int startaddr, int endaddr) {
    int size;

    if (startaddr == -1) {
        startaddr = sndgs.sso.set.arampooladdr;
    }
    if (endaddr == -1) {
        endaddr = sndgs.sso.set.arampooladdr + sndgs.sso.set.arampoolsize;
    }
    size = endaddr - startaddr;
    SNDARAM_setpool(startaddr, size);
    return 0;
}

int SNDPLATFORM_memlargestunused(int *paddr) {
    unsigned int addr;
    int ret;

    ret = SNDARAM_largestfree(&addr);
    *paddr = addr;
    return ret;
}

int SNDPLATFORM_download(int playloc, void *psrc, void *pdst, int size) {
    unsigned long dlhandle;

    DCFlushRange(psrc, size);
    dlhandle = SNDDRV_dmapost(0, psrc, pdst, size, -1);
    return dlhandle;
}

int SNDPLATFORM_downloadcomplete(int dlhandle) {
    int i;
    int old;

    old = OSDisableInterrupts();
    for (i = 0; i <= 0x3f; i++) {
        if (snddrv.dmaslot[i].handle != -1 && snddrv.dmaslot[i].handle == dlhandle) {
            if (sndgs.audiotick - snddrv.dmaslot[i].tick > 200) {
                OSRestoreInterrupts(old);
                SNDDRV_restoredma(dlhandle);
            } else {
                OSRestoreInterrupts(old);
            }
            return 0;
        }
    }
    OSRestoreInterrupts(old);
    return 1;
}

void SNDI_mutexalloc() {
    OSInitMutex(&snddrv.mutex);
}

void SNDI_mutexfree() {
}

void SNDI_mutexlock() {
    OSLockMutex(&snddrv.mutex);
}

void SNDI_mutexunlock() {
    OSUnlockMutex(&snddrv.mutex);
}

int SNDPLATFORM_ModeSetUp(SNDSYSOPTS *pSndSysOpts, Snd::OutputMode outputmode) {
    switch (outputmode) {
    case Snd::OUTPUTMODE_MONO:
        pSndSysOpts->set.outputchannels = 1;
        break;
    case Snd::OUTPUTMODE_STEREO:
        pSndSysOpts->set.outputchannels = 2;
        break;
    case Snd::OUTPUTMODE_PROLOGIC2:
        pSndSysOpts->set.outputchannels = 4;
        break;
    }
    return 0;
}
