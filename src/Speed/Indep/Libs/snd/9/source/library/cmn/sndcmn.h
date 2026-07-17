#ifndef SNDCMN_H
#define SNDCMN_H

#include "./slinklist.h"
#include <snd/sndo.h>

// total size: 0xFC
// Decl: 470
struct SNDIPATCHHEADER {
    short masterranddetune;            // offset 0x0, size 0x2, Decl: 471
    signed char platformver;           // offset 0x2, size 0x1, Decl: 472
    signed char numchan;               // offset 0x3, size 0x1, Decl: 473
    short detune;                      // offset 0x4, size 0x2, Decl: 475
    short randdetune;                  // offset 0x6, size 0x2, Decl: 476
    signed char velmin;                // offset 0x8, size 0x1, Decl: 478
    signed char velmax;                // offset 0x9, size 0x1, Decl: 479
    signed char keymin;                // offset 0xA, size 0x1, Decl: 480
    signed char keymax;                // offset 0xB, size 0x1, Decl: 481
    signed char priority;              // offset 0xC, size 0x1, Decl: 483
    signed char basekey;               // offset 0xD, size 0x1, Decl: 484
    signed char releaseenvelope;       // offset 0xE, size 0x1, Decl: 485
    signed char numenvelopes;          // offset 0xF, size 0x1, Decl: 486
    signed char bendrange;             // offset 0x10, size 0x1
    signed char pan;                   // offset 0x11, size 0x1, Decl: 488
    signed char randpan;               // offset 0x12, size 0x1, Decl: 489
    unsigned char samplerep;           // offset 0x13, size 0x1, Decl: 490
    signed char vol;                   // offset 0x14, size 0x1, Decl: 491
    signed char randvol;               // offset 0x15, size 0x1, Decl: 493
    signed char fxlevel0;              // offset 0x16, size 0x1, Decl: 494
    signed char numuserdata;           // offset 0x17, size 0x1, Decl: 495
    short unsigned int rendermode;     // offset 0x18, size 0x2
    signed char initialenvvol;         // offset 0x1A, size 0x1, Decl: 496
    unsigned char lfovollength;        // offset 0x1B, size 0x1, Decl: 498
    signed char lfovolrand;            // offset 0x1C, size 0x1, Decl: 501
    unsigned char lfopitchlength;      // offset 0x1D, size 0x1, Decl: 502
    unsigned char lfopitchrand;        // offset 0x1E, size 0x1, Decl: 503
    signed char panmult;               // offset 0x1F, size 0x1, Decl: 504
    void *pvoltable;                   // offset 0x20, size 0x4, Decl: 506
    void *pbendtable;                  // offset 0x24, size 0x4
    void *penvelope;                   // offset 0x28, size 0x4, Decl: 507
    void *plfovol;                     // offset 0x2C, size 0x4, Decl: 508
    void *plfopitch;                   // offset 0x30, size 0x4, Decl: 509
    void *puserdata[4];                // offset 0x34, size 0x10, Decl: 510
    int userdatasize[4];               // offset 0x44, size 0x10, Decl: 511
    short lfopitchdepth;               // offset 0x54, size 0x2, Decl: 513
    unsigned short samplerate;         // offset 0x56, size 0x2, Decl: 514
    int totalframes;                   // offset 0x58, size 0x4, Decl: 516
    int sustainstart;                  // offset 0x5C, size 0x4, Decl: 517
    int sustainend;                    // offset 0x60, size 0x4, Decl: 518
    int *paudioramaddr;                // offset 0x64, size 0x4
    int sampleoffset[6];               // offset 0x68, size 0x18, Decl: 520
    int loopoffset[6];                 // offset 0x80, size 0x18, Decl: 522
    void *ptimestretchdata[6];         // offset 0x98, size 0x18, Decl: 525
    unsigned short timestretchsize[6]; // offset 0xB0, size 0xC, Decl: 526
    unsigned short azimuth[6];         // offset 0xBC, size 0xC, Decl: 529
    void *pcodebookdata[6];            // offset 0xC8, size 0x18
    void *ploopstatedata[6];           // offset 0xE0, size 0x18
    void *ptimbre;                     // offset 0xF8, size 0x4, Decl: 531
};

// total size: 0x8
// Decl: 538
typedef struct ENVELOPE {
    int duration;  // offset 0x0, size 0x4
    int targetvol; // offset 0x4, size 0x4
} ENVELOPE;

// total size: 0x80
// Decl: 545
typedef struct SCALINGTABLE {
    signed char xlate[128]; // offset 0x0, size 0x80
} SCALINGTABLE;

// total size: 0x8
// Decl: 553
typedef struct TAGGEDPATCH {
    short id;               // offset 0x0, size 0x2
    unsigned char platform; // offset 0x2, size 0x1
    unsigned char flags;    // offset 0x3, size 0x1
    int hdrsize;            // offset 0x4, size 0x4
} TAGGEDPATCH;

// total size: 0x814
// Decl: 564
typedef struct BANKVER5 {
    int id;                    // offset 0x0, size 0x4
    unsigned char ver;         // offset 0x4, size 0x1
    char resolved;             // offset 0x5, size 0x1
    unsigned short numpatches; // offset 0x6, size 0x2
    int hdrsize;               // offset 0x8, size 0x4
    int spusize;               // offset 0xC, size 0x4
    int iopcpusize;            // offset 0x10, size 0x4
    TAGGEDPATCH *patch[512];   // offset 0x14, size 0x800
} BANKVER5;

// total size: 0xC
// Decl: 577
typedef struct BANKLIST {
    BANKVER5 *phdr;     // offset 0x0, size 0x4
    void *pspuram;      // offset 0x4, size 0x4
    signed char locked; // offset 0x8, size 0x1
    char pad[3];        // offset 0x9, size 0x3
} BANKLIST;

// total size: 0x4
// Decl: 587
typedef struct FXVOLUME {
    float fxLevel; // offset 0x0, size 0x4
} FXVOLUME;

// total size: 0x90
// Decl: 598
typedef struct CHANPUB {
    int handle;                       // offset 0x0, size 0x4
    short voices[6];                  // offset 0x4, size 0xC
    unsigned short patnum;            // offset 0x10, size 0x2
    short bhandle;                    // offset 0x12, size 0x2
    unsigned int frames;              // offset 0x14, size 0x4
    int sustainend;                   // offset 0x18, size 0x4
    unsigned short azimuth;           // offset 0x1C, size 0x2
    short elevation;                  // offset 0x1E, size 0x2
    unsigned short samplerate;        // offset 0x20, size 0x2
    unsigned char samplerep;          // offset 0x22, size 0x1
    unsigned char numchan;            // offset 0x23, size 0x1
    unsigned short rendermode;        // offset 0x24, size 0x2
    unsigned char patchkey;           // offset 0x26, size 0x1
    unsigned char ismaster;           // offset 0x27, size 0x1
    short masterchan;                 // offset 0x28, size 0x2
    unsigned char priority;           // offset 0x2A, size 0x1
    signed char builtinvol;           // offset 0x2B, size 0x1
    unsigned int timestamp;           // offset 0x2C, size 0x4
    float fadePer100Hz;               // offset 0x30, size 0x4
    float fadeTargetVol;              // offset 0x34, size 0x4
    float programmedVol;              // offset 0x38, size 0x4
    int envpertick;                   // offset 0x3C, size 0x4
    int envvol;                       // offset 0x40, size 0x4
    int envduration;                  // offset 0x44, size 0x4
    float finalvol;                   // offset 0x48, size 0x4
    unsigned short builtinazimuth;    // offset 0x4C, size 0x2
    unsigned char isLfe;              // offset 0x4E, size 0x1
    char pad[1];                      // offset 0x4F, size 0x1
    unsigned short azimuthOffsets[6]; // offset 0x50, size 0xC
    signed char numenvelopes;         // offset 0x5C, size 0x1
    signed char curenvelope;          // offset 0x5D, size 0x1
    signed char releaseenvelope;      // offset 0x5E, size 0x1
    signed char drylevel;             // offset 0x5F, size 0x1
    signed char progfxlevel;          // offset 0x60, size 0x1
    char pad1[3];                     // offset 0x61, size 0x3
    FXVOLUME *pFxVolume;              // offset 0x64, size 0x4
    signed char pitchbend;            // offset 0x68, size 0x1
    unsigned char vollfolength;       // offset 0x69, size 0x1
    unsigned char pitchlfolength;     // offset 0x6A, size 0x1
    unsigned char curvollfoentry;     // offset 0x6B, size 0x1
    unsigned char curpitchlfoentry;   // offset 0x6C, size 0x1
    signed char status;               // offset 0x6D, size 0x1
    unsigned short timemult;          // offset 0x6E, size 0x2
    ENVELOPE *paenvelope;             // offset 0x70, size 0x4
    SCALINGTABLE *pvoltable;          // offset 0x74, size 0x4
    SCALINGTABLE *pbendtable;         // offset 0x78, size 0x4
    SCALINGTABLE *pvollfo;            // offset 0x7C, size 0x4
    SCALINGTABLE *ppitchlfo;          // offset 0x80, size 0x4
    short pitchlfodepth;              // offset 0x84, size 0x2
    short bendrange;                  // offset 0x86, size 0x2
    short initialdetune;              // offset 0x88, size 0x2
    unsigned short detunepitch;       // offset 0x8A, size 0x2
    unsigned short progpitch;         // offset 0x8C, size 0x2
    unsigned short finalpitch;        // offset 0x8E, size 0x2
} CHANPUB;

// total size: 0x8
typedef struct SNDMEMREC {
    unsigned int addr; // offset 0x0, size 0x4
    int size;          // offset 0x4, size 0x4
} SNDMEMREC;

// total size: 0x18
typedef struct SNDMEMSTATE {
    char *pheap;          // offset 0x0, size 0x4
    SNDMEMREC *r;         // offset 0x4, size 0x4
    int heapsize;         // offset 0x8, size 0x4
    unsigned int endaddr; // offset 0xC, size 0x4
    int lowmark;          // offset 0x10, size 0x4
    int nummallocs;       // offset 0x14, size 0x4
} SNDMEMSTATE;

// total size: 0x1A
// Decl: 789
typedef struct SNDSYSCAP {
    unsigned short outputratemin;    // offset 0x0, size 0x2
    unsigned short outputratemax;    // offset 0x2, size 0x2
    unsigned char outputchannelsmin; // offset 0x4, size 0x1
    unsigned char outputchannelsmax; // offset 0x5, size 0x1
    unsigned char voicesmaincpumax;  // offset 0x6, size 0x1
    unsigned char voicesiopcpumax;   // offset 0x7, size 0x1
    unsigned char voicesspumax;      // offset 0x8, size 0x1
    unsigned char voicesds2dhwmax;   // offset 0x9, size 0x1
    unsigned char voicesdspmax;      // offset 0xA, size 0x1
    unsigned char eax;               // offset 0xB, size 0x1
    unsigned char numrendermodes;    // offset 0xC, size 0x1
    unsigned short rendermode[6];    // offset 0xE, size 0xC
} SNDSYSCAP;

// total size: 0x9C
// Decl: 796
typedef struct SNDSYSSET {
    int packetbufsize;                      // offset 0x0, size 0x4
    unsigned int randomseed;                // offset 0x4, size 0x4
    unsigned short maxbanks;                // offset 0x8, size 0x2
    unsigned short outputrate;              // offset 0xA, size 0x2
    unsigned short outputrateiopcpu;        // offset 0xC, size 0x2
    unsigned char voicesmaincpu;            // offset 0xE, size 0x1
    unsigned char voicesiopcpu;             // offset 0xF, size 0x1
    unsigned char voicesspu;                // offset 0x10, size 0x1
    unsigned char voicesds2dhw;             // offset 0x11, size 0x1
    unsigned char voicesdsp;                // offset 0x12, size 0x1
    char compatibility;                     // offset 0x13, size 0x1
    float updateperiod;                     // offset 0x14, size 0x4
    unsigned char stealequalpriorityvoices; // offset 0x18, size 0x1
    unsigned char usesse;                   // offset 0x19, size 0x1
    unsigned char useeax;                   // offset 0x1A, size 0x1
    unsigned char loadiopmodules;           // offset 0x1B, size 0x1
    unsigned char resamplequality;          // offset 0x1C, size 0x1
    unsigned char iopcpuresamplequality;    // offset 0x1D, size 0x1
    unsigned char routemaincpufxtoiopcpu;   // offset 0x1E, size 0x1
    unsigned char routespufxtoiopcpu;       // offset 0x1F, size 0x1
    unsigned char emulationsubtype;         // offset 0x20, size 0x1
    unsigned char initaram;                 // offset 0x21, size 0x1
    unsigned char maxstreams;               // offset 0x22, size 0x1
    unsigned char outputchannels;           // offset 0x23, size 0x1
    unsigned char outputlfe;                // offset 0x24, size 0x1
    unsigned char outputspdifmode;          // offset 0x25, size 0x1
    unsigned char dtsquality;               // offset 0x26, size 0x1
    unsigned char sndheapthreshold;         // offset 0x27, size 0x1
    unsigned char numrendermodes;           // offset 0x28, size 0x1
    unsigned char useASIOdriver;            // offset 0x29, size 0x1
    unsigned short rendermode[6];           // offset 0x2A, size 0xC
    unsigned int arampooladdr;              // offset 0x38, size 0x4
    int arampoolsize;                       // offset 0x3C, size 0x4
    void *fastramaddr;                      // offset 0x40, size 0x4
    int fastramsize;                        // offset 0x44, size 0x4
    char *iopmodulepath;                    // offset 0x48, size 0x4
    char *ASIOconfig;                       // offset 0x4C, size 0x4
    void *snd2asio;                         // offset 0x50, size 0x4
    unsigned short virtualspkrcfg3d[6][6];  // offset 0x54, size 0x48
} SNDSYSSET;

// total size: 0x14
// Decl: 812
typedef struct SNDSYSVEC {
    unsigned int (*aramalloc)(unsigned int); // offset 0x0, size 0x4
    void (*aramfree)(unsigned int);          // offset 0x4, size 0x4
    void (*abortmsg)(char *);                // offset 0x8, size 0x4
    void (*profileenter)(const char *);      // offset 0xC, size 0x4
    void (*profileleave)(const char *);      // offset 0x10, size 0x4
} SNDSYSVEC;

// total size: 0xCC
// Decl: 818
typedef struct SNDSYSOPTS {
    SNDSYSCAP cap; // offset 0x0, size 0x1A
    SNDSYSSET set; // offset 0x1C, size 0x9C
    SNDSYSVEC vec; // offset 0xB8, size 0x14
} SNDSYSOPTS;

// total size: 0x244
// Decl: 825
typedef struct SNDGLOBALSTATE {
    SNDSYSOPTS sso;                                          // offset 0x0, size 0xCC
    SNDSYSSET prevset;                                       // offset 0xCC, size 0x9C
    signed char installed;                                   // offset 0x168, size 0x1
    volatile signed char incritical;                         // offset 0x169, size 0x1
    signed char numserverclients100hz;                       // offset 0x16A, size 0x1
    signed char numserverclients;                            // offset 0x16B, size 0x1
    short voicestotal;                                       // offset 0x16C, size 0x2
    signed char numuserdataclients;                          // offset 0x16E, size 0x1
    unsigned char cputypes;                                  // offset 0x16F, size 0x1
    volatile unsigned int audiotick;                         // offset 0x170, size 0x4
    void (*serverclient100hz[6])();                          // offset 0x174, size 0x18
    void (*serverclient[6])();                               // offset 0x18C, size 0x18
    void (*userdataclient[4])(SNDUSERDATACBINFO *);          // offset 0x1A4, size 0x10
    void (*aemsrestore)();                                   // offset 0x1B4, size 0x4
    int (*aemsstopmodulebanks)();                            // offset 0x1B8, size 0x4
    int (*aemsstreamrestore)();                              // offset 0x1BC, size 0x4
    int (*eventrestore)();                                   // offset 0x1C0, size 0x4
    int (*bankremove)(int);                                  // offset 0x1C4, size 0x4
    int (*midirestore)();                                    // offset 0x1C8, size 0x4
    int (*streamrestore)();                                  // offset 0x1CC, size 0x4
    int (*aemsstreampurge)();                                // offset 0x1D0, size 0x4
    CHANPUB *chan;                                           // offset 0x1D4, size 0x4
    BANKLIST *banklist;                                      // offset 0x1D8, size 0x4
    SNDMEMSTATE *mm;                                         // offset 0x1DC, size 0x4
    unsigned short srcchancfg3d[6][6];                       // offset 0x1E0, size 0x48
    void (*profileenter)();                                  // offset 0x228, size 0x4
    void (*profileleave)();                                  // offset 0x22C, size 0x4
    volatile unsigned int servertick;                        // offset 0x230, size 0x4
    volatile short currenttimerhz;                           // offset 0x234, size 0x2
    volatile short servicesskipped;                          // offset 0x236, size 0x2
    char aborted;                                            // offset 0x238, size 0x1
    char logmixing;                                          // offset 0x239, size 0x1
    void (*logcallbackfn)(int, int, unsigned short, void *); // offset 0x23C, size 0x4
    char debugpad[2];                                        // offset 0x240, size 0x2
} SNDGLOBALSTATE;

// total size: 0x8
// Decl: 761
typedef struct SNDLINKNODE {
    SNDLINKNODE *pnext; // offset 0x0, size 0x4
    SNDLINKNODE *pprev; // offset 0x4, size 0x4
} SNDLINKNODE;

// total size: 0xC
// Decl: 767
typedef struct SNDLINKLIST {
    SNDLINKNODE *phead; // offset 0x0, size 0x4
    SNDLINKNODE *ptail; // offset 0x4, size 0x4
    int items;          // offset 0x8, size 0x4
} SNDLINKLIST;

namespace Snd {

// total size: 0x10
// Decl: 1244
typedef struct VariableTimerClient {
    CListDNode ln;               // offset 0x0, size 0x8
    void (*pClientFunc)(void *); // offset 0x8, size 0x4
    void *pClientData;           // offset 0xC, size 0x4
} VariableTimerClient;

// total size: 0x1
// Decl: 1268
struct Util {
    static void ReallocBuf(void **buffer, int *curSize, int newMinSize, int sizeofDataType);

    static unsigned short Az360To65536(float azimuth) {}

    static float Az65536To360(unsigned short azimuth) {}

    static void FastVol(struct CHANPUB *pVoice) {}

    static void AddVariableTimerClient(VariableTimerClient *pClient) {}

    static void RemoveVariableTimerClient(VariableTimerClient *pClient) {}
};

// total size: 0x28
// Decl: 64
typedef struct SNDSTREAMREQUEST {
    SNDLINKNODE node;                        // offset 0x0, size 0x8
    volatile int streamrequestid;            // offset 0x8, size 0x4
    volatile int sndrequesthandle;           // offset 0xC, size 0x4
    volatile unsigned int avgdatarate;       // offset 0x10, size 0x4
    volatile unsigned int currentframe;      // offset 0x14, size 0x4
    volatile int totalframes;                // offset 0x18, size 0x4
    volatile unsigned int outstandingframes; // offset 0x1C, size 0x4
    volatile int holdtime;                   // offset 0x20, size 0x4
    unsigned char continuation;              // offset 0x24, size 0x1
    volatile unsigned char isgeneric;        // offset 0x25, size 0x1
    char pad[2];                             // offset 0x26, size 0x2
} SNDSTREAMREQUEST;

// total size: 0x4
typedef struct SNDSTREAMFXVOLUME {
    float fxLevel; // offset 0x0, size 0x4
} SNDSTREAMFXVOLUME;

// total size: 0x18
typedef struct Fader {
    VariableTimerClient variableTimerClient; // offset 0x0, size 0x10
    float targetVol;                         // offset 0x10, size 0x4
    float incrementPerUpdate;                // offset 0x14, size 0x4
} Fader;

// total size: 0x24
typedef struct StreamSourceChannelState {
    float azimuth;               // offset 0x0, size 0x4
    unsigned char isDirect;      // offset 0x4, size 0x1
    unsigned char sourceChannel; // offset 0x5, size 0x1
    char pad[2];                 // offset 0x6, size 0x2
    float vol;                   // offset 0x8, size 0x4
    Fader fader;                 // offset 0xC, size 0x18
} StreamSourceChannelState;

}; // namespace Snd

typedef int STREAMHANDLE;
typedef int STREAMREQUESTID;

// total size: 0x214
typedef struct SNDSTREAMCHANNEL {
    STREAMHANDLE streamhandle;                           // offset 0x0, size 0x4
    volatile int shandle;                                // offset 0x4, size 0x4
    volatile int packetinstancehandle;                   // offset 0x8, size 0x4
    volatile int lastsndrequesthandle;                   // offset 0xC, size 0x4
    volatile signed char state;                          // offset 0x10, size 0x1
    volatile signed char fromtap;                        // offset 0x11, size 0x1
    unsigned char useOldAzimuthBehaviour;                // offset 0x12, size 0x1
    char pad[1];                                         // offset 0x13, size 0x1
    SNDSAMPLEFORMAT cursf;                               // offset 0x14, size 0x4
    SNDSAMPLEFORMAT newsf;                               // offset 0x18, size 0x4
    SNDSAMPLEATTR cursa;                                 // offset 0x1C, size 0x68
    SNDSAMPLEATTR newsa;                                 // offset 0x84, size 0x68
    SNDPLAYOPTS po;                                      // offset 0xEC, size 0x18
    Snd::SNDSTREAMFXVOLUME *pFxVolume;                   // offset 0x104, size 0x4
    SNDFILTERDEF filter[1];                              // offset 0x108, size 0x18
    SNDLINKLIST allocatedrequests;                       // offset 0x120, size 0xC
    SNDLINKLIST freerequests;                            // offset 0x12C, size 0xC
    volatile Snd::SNDSTREAMREQUEST *parsingrequest;      // offset 0x138, size 0x4
    Snd::StreamSourceChannelState sourceChannelState[6]; // offset 0x13C, size 0xD8
} SNDSTREAMCHANNEL;

// total size: 0x80
typedef struct SNDSTREAMSTATE {
    SNDSTREAMCHANNEL *pssc[32]; // offset 0x0, size 0x80
} SNDSTREAMSTATE;

extern SNDGLOBALSTATE sndgs; // Decl: 861

#include "sndenum.h"

namespace Snd {

extern void (*gMutexLockFn)();
extern void (*gMutexUnlockFn)();

// TODO: move?
class Hal {
public:
    static int SetPresetFx(void *pData, FxPreset fxPreset);

    static int SetPresetFxEx(void *pData, void *pFxPresetData);

    static unsigned int SetCustomFx(void *pData, void *pFxDefinition);

    static unsigned int Reset(void *pData);

    static int SetFxOutputLevel(void *pData, Channel outputChannel);

    static void SetVol(int voice);

    static void SetDry(int voice);

    static void SetPan(int voice);

private:
    static void SetVolInternal(int voice); // guess
};

// TODO: move?
extern FoldDownTarget gFoldDownTarget;

}; // namespace Snd

// Decl: 1041
inline int SNDI_ftoiround(float val) {
    int result;

    if (val >= 0.0f) {
        result = static_cast<int>(val + 0.5f);
    } else {
        result = static_cast<int>(val - 0.5f);
    }

    return result;
}

// 1202
inline int SNDI_ftoifast(float val) {
    return SNDI_ftoiround(val * 127.0f);
}

// 1610
static inline int SNDI_clipint32(int val, int minval, int maxval) {
    if (val < minval) return minval;
    if (val > maxval) return maxval;
    return val;
}

// sst.c
SNDSTREAMCHANNEL *SNDSTRMI_getstreamptr(int sndstreamhandle);

// salloc.c
int SNDVOICEI_get(int handle);

// sballoc.c
TAGGEDPATCH *SNDBANKI_getppatch(BANKVER5 *pb, int patnum);

// spatkey.c
int iSNDpatchkey(int chan, int *psetchan);

// spat2hdr.c
void SNDI_patchtohdr(void *pbank, TAGGEDPATCH *ptp, SNDSAMPLEFORMAT *pssf, SNDSAMPLEATTR *pssa, SNDSAMPLEDESC *pssd, unsigned char *isgeneric);

// smemman.c
void SNDMEMI_free(void *paddr);

// sbvalid.c
int SNDBANKI_valid(int bhandle);

// sgetpvol.c
int SNDCTRL_getprogvol(int shandle);

// sstgetpv.c
int SNDSTRM_getprogvol(int sndstreamhandle);

// stimerem.c
int SNDtimeremaining(int shandle);

#endif
