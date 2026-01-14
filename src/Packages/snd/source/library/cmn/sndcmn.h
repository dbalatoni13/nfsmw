#ifndef SNDCMN_H
#define SNDCMN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

typedef struct TAGGEDPATCH {
    // total size: 0x8
    short id; // offset 0x0, size 0x2
    unsigned char platform; // offset 0x2, size 0x1
    unsigned char flags; // offset 0x3, size 0x1
    int hdrsize; // offset 0x4, size 0x4
} TAGGEDPATCH;

typedef struct BANKVER5 {
    // total size: 0x814
    int id; // offset 0x0, size 0x4
    unsigned char ver; // offset 0x4, size 0x1
    char resolved; // offset 0x5, size 0x1
    unsigned short numpatches; // offset 0x6, size 0x2
    int hdrsize; // offset 0x8, size 0x4
    int spusize; // offset 0xC, size 0x4
    int iopcpusize; // offset 0x10, size 0x4
    TAGGEDPATCH * patch[512]; // offset 0x14, size 0x800
} BANKVER5;

typedef struct BANKLIST {
    // total size: 0xC
    BANKVER5 * phdr; // offset 0x0, size 0x4
    void * pspuram; // offset 0x4, size 0x4
    signed char locked; // offset 0x8, size 0x1
    char pad[3]; // offset 0x9, size 0x3
} BANKLIST;

typedef struct FXVOLUME {
    // total size: 0x4
    float fxLevel; // offset 0x0, size 0x4
} FXVOLUME;
typedef struct ENVELOPE {
    // total size: 0x8
    int duration; // offset 0x0, size 0x4
    int targetvol; // offset 0x4, size 0x4
} ENVELOPE;
typedef struct SCALINGTABLE {
    // total size: 0x80
    signed char xlate[128]; // offset 0x0, size 0x80
} SCALINGTABLE;

typedef struct CHANPUB {
    // total size: 0x90
    int handle; // offset 0x0, size 0x4
    short voices[6]; // offset 0x4, size 0xC
    unsigned short patnum; // offset 0x10, size 0x2
    short bhandle; // offset 0x12, size 0x2
    unsigned int frames; // offset 0x14, size 0x4
    int sustainend; // offset 0x18, size 0x4
    unsigned short azimuth; // offset 0x1C, size 0x2
    short elevation; // offset 0x1E, size 0x2
    unsigned short samplerate; // offset 0x20, size 0x2
    unsigned char samplerep; // offset 0x22, size 0x1
    unsigned char numchan; // offset 0x23, size 0x1
    unsigned short rendermode; // offset 0x24, size 0x2
    unsigned char patchkey; // offset 0x26, size 0x1
    unsigned char ismaster; // offset 0x27, size 0x1
    short masterchan; // offset 0x28, size 0x2
    unsigned char priority; // offset 0x2A, size 0x1
    signed char builtinvol; // offset 0x2B, size 0x1
    unsigned int timestamp; // offset 0x2C, size 0x4
    float fadePer100Hz; // offset 0x30, size 0x4
    float fadeTargetVol; // offset 0x34, size 0x4
    float programmedVol; // offset 0x38, size 0x4
    int envpertick; // offset 0x3C, size 0x4
    int envvol; // offset 0x40, size 0x4
    int envduration; // offset 0x44, size 0x4
    float finalvol; // offset 0x48, size 0x4
    unsigned short builtinazimuth; // offset 0x4C, size 0x2
    unsigned char isLfe; // offset 0x4E, size 0x1
    char pad[1]; // offset 0x4F, size 0x1
    unsigned short azimuthOffsets[6]; // offset 0x50, size 0xC
    signed char numenvelopes; // offset 0x5C, size 0x1
    signed char curenvelope; // offset 0x5D, size 0x1
    signed char releaseenvelope; // offset 0x5E, size 0x1
    signed char drylevel; // offset 0x5F, size 0x1
    signed char progfxlevel; // offset 0x60, size 0x1
    char pad1[3]; // offset 0x61, size 0x3
    FXVOLUME *pFxVolume; // offset 0x64, size 0x4
    signed char pitchbend; // offset 0x68, size 0x1
    unsigned char vollfolength; // offset 0x69, size 0x1
    unsigned char pitchlfolength; // offset 0x6A, size 0x1
    unsigned char curvollfoentry; // offset 0x6B, size 0x1
    unsigned char curpitchlfoentry; // offset 0x6C, size 0x1
    signed char status; // offset 0x6D, size 0x1
    unsigned short timemult; // offset 0x6E, size 0x2
    ENVELOPE *paenvelope; // offset 0x70, size 0x4
    SCALINGTABLE *pvoltable; // offset 0x74, size 0x4
    SCALINGTABLE *pbendtable; // offset 0x78, size 0x4
    SCALINGTABLE *pvollfo; // offset 0x7C, size 0x4
    SCALINGTABLE *ppitchlfo; // offset 0x80, size 0x4
    short pitchlfodepth; // offset 0x84, size 0x2
    short bendrange; // offset 0x86, size 0x2
    short initialdetune; // offset 0x88, size 0x2
    unsigned short detunepitch; // offset 0x8A, size 0x2
    unsigned short progpitch; // offset 0x8C, size 0x2
    unsigned short finalpitch; // offset 0x8E, size 0x2
} CHANPUB;

typedef struct SNDUSERDATACBINFO {
    // total size: 0x14
    int type; // offset 0x0, size 0x4
    void * pdata; // offset 0x4, size 0x4
    int size; // offset 0x8, size 0x4
    int shandle; // offset 0xC, size 0x4
    int sndstrmrequest; // offset 0x10, size 0x4
} SNDUSERDATACBINFO;

typedef struct SNDMEMREC {
    // total size: 0x8
    unsigned int addr; // offset 0x0, size 0x4
    int size; // offset 0x4, size 0x4
} SNDMEMREC;

typedef struct SNDMEMSTATE {
    // total size: 0x18
    char * pheap; // offset 0x0, size 0x4
    SNDMEMREC * r; // offset 0x4, size 0x4
    int heapsize; // offset 0x8, size 0x4
    unsigned int endaddr; // offset 0xC, size 0x4
    int lowmark; // offset 0x10, size 0x4
    int nummallocs; // offset 0x14, size 0x4
} SNDMEMSTATE;

typedef struct SNDSYSCAP {
    // total size: 0x1A
    unsigned short outputratemin; // offset 0x0, size 0x2
    unsigned short outputratemax; // offset 0x2, size 0x2
    unsigned char outputchannelsmin; // offset 0x4, size 0x1
    unsigned char outputchannelsmax; // offset 0x5, size 0x1
    unsigned char voicesmaincpumax; // offset 0x6, size 0x1
    unsigned char voicesiopcpumax; // offset 0x7, size 0x1
    unsigned char voicesspumax; // offset 0x8, size 0x1
    unsigned char voicesds2dhwmax; // offset 0x9, size 0x1
    unsigned char voicesdspmax; // offset 0xA, size 0x1
    unsigned char eax; // offset 0xB, size 0x1
    unsigned char numrendermodes; // offset 0xC, size 0x1
    unsigned short rendermode[6]; // offset 0xE, size 0xC
} SNDSYSCAP;

typedef struct SNDSYSSET {
    // total size: 0x9C
    int packetbufsize; // offset 0x0, size 0x4
    unsigned int randomseed; // offset 0x4, size 0x4
    unsigned short maxbanks; // offset 0x8, size 0x2
    unsigned short outputrate; // offset 0xA, size 0x2
    unsigned short outputrateiopcpu; // offset 0xC, size 0x2
    unsigned char voicesmaincpu; // offset 0xE, size 0x1
    unsigned char voicesiopcpu; // offset 0xF, size 0x1
    unsigned char voicesspu; // offset 0x10, size 0x1
    unsigned char voicesds2dhw; // offset 0x11, size 0x1
    unsigned char voicesdsp; // offset 0x12, size 0x1
    char compatibility; // offset 0x13, size 0x1
    float updateperiod; // offset 0x14, size 0x4
    unsigned char stealequalpriorityvoices; // offset 0x18, size 0x1
    unsigned char usesse; // offset 0x19, size 0x1
    unsigned char useeax; // offset 0x1A, size 0x1
    unsigned char loadiopmodules; // offset 0x1B, size 0x1
    unsigned char resamplequality; // offset 0x1C, size 0x1
    unsigned char iopcpuresamplequality; // offset 0x1D, size 0x1
    unsigned char routemaincpufxtoiopcpu; // offset 0x1E, size 0x1
    unsigned char routespufxtoiopcpu; // offset 0x1F, size 0x1
    unsigned char emulationsubtype; // offset 0x20, size 0x1
    unsigned char initaram; // offset 0x21, size 0x1
    unsigned char maxstreams; // offset 0x22, size 0x1
    unsigned char outputchannels; // offset 0x23, size 0x1
    unsigned char outputlfe; // offset 0x24, size 0x1
    unsigned char outputspdifmode; // offset 0x25, size 0x1
    unsigned char dtsquality; // offset 0x26, size 0x1
    unsigned char sndheapthreshold; // offset 0x27, size 0x1
    unsigned char numrendermodes; // offset 0x28, size 0x1
    unsigned char useASIOdriver; // offset 0x29, size 0x1
    unsigned short rendermode[6]; // offset 0x2A, size 0xC
    unsigned int arampooladdr; // offset 0x38, size 0x4
    int arampoolsize; // offset 0x3C, size 0x4
    void * fastramaddr; // offset 0x40, size 0x4
    int fastramsize; // offset 0x44, size 0x4
    char * iopmodulepath; // offset 0x48, size 0x4
    char * ASIOconfig; // offset 0x4C, size 0x4
    void * snd2asio; // offset 0x50, size 0x4
    unsigned short virtualspkrcfg3d[6][6]; // offset 0x54, size 0x48
} SNDSYSSET;

typedef struct SNDSYSVEC {
    // total size: 0x14
    unsigned int (* aramalloc)(unsigned int); // offset 0x0, size 0x4
    void (* aramfree)(unsigned int); // offset 0x4, size 0x4
    void (* abortmsg)(char *); // offset 0x8, size 0x4
    void (* profileenter)(const char *); // offset 0xC, size 0x4
    void (* profileleave)(const char *); // offset 0x10, size 0x4
} SNDSYSVEC;

typedef struct SNDSYSOPTS {
    // total size: 0xCC
    SNDSYSCAP cap; // offset 0x0, size 0x1A
    SNDSYSSET set; // offset 0x1C, size 0x9C
    SNDSYSVEC vec; // offset 0xB8, size 0x14
} SNDSYSOPTS;

typedef struct SNDGLOBALSTATE {
    // total size: 0x244
    SNDSYSOPTS sso; // offset 0x0, size 0xCC
    SNDSYSSET prevset; // offset 0xCC, size 0x9C
    signed char installed; // offset 0x168, size 0x1
    volatile signed char incritical; // offset 0x169, size 0x1
    signed char numserverclients100hz; // offset 0x16A, size 0x1
    signed char numserverclients; // offset 0x16B, size 0x1
    short voicestotal; // offset 0x16C, size 0x2
    signed char numuserdataclients; // offset 0x16E, size 0x1
    unsigned char cputypes; // offset 0x16F, size 0x1
    volatile unsigned int audiotick; // offset 0x170, size 0x4
    void (* serverclient100hz[6])(); // offset 0x174, size 0x18
    void (* serverclient[6])(); // offset 0x18C, size 0x18
    void (* userdataclient[4])(SNDUSERDATACBINFO *); // offset 0x1A4, size 0x10
    void (* aemsrestore)(); // offset 0x1B4, size 0x4
    int (* aemsstopmodulebanks)(); // offset 0x1B8, size 0x4
    int (* aemsstreamrestore)(); // offset 0x1BC, size 0x4
    int (* eventrestore)(); // offset 0x1C0, size 0x4
    int (* bankremove)(int); // offset 0x1C4, size 0x4
    int (* midirestore)(); // offset 0x1C8, size 0x4
    int (* streamrestore)(); // offset 0x1CC, size 0x4
    int (* aemsstreampurge)(); // offset 0x1D0, size 0x4
    CHANPUB * chan; // offset 0x1D4, size 0x4
    BANKLIST * banklist; // offset 0x1D8, size 0x4
    SNDMEMSTATE * mm; // offset 0x1DC, size 0x4
    unsigned short srcchancfg3d[6][6]; // offset 0x1E0, size 0x48
    void (* profileenter)(); // offset 0x228, size 0x4
    void (* profileleave)(); // offset 0x22C, size 0x4
    volatile unsigned int servertick; // offset 0x230, size 0x4
    volatile short currenttimerhz; // offset 0x234, size 0x2
    volatile short servicesskipped; // offset 0x236, size 0x2
    char aborted; // offset 0x238, size 0x1
    char logmixing; // offset 0x239, size 0x1
    void (* logcallbackfn)(int, int, unsigned short, void *); // offset 0x23C, size 0x4
    char debugpad[2]; // offset 0x240, size 0x2
} SNDGLOBALSTATE;

typedef struct SNDSAMPLEFORMAT {
    // total size: 0x4
    unsigned short samplerate; // offset 0x0, size 0x2
    unsigned char channels; // offset 0x2, size 0x1
    unsigned char samplerep; // offset 0x3, size 0x1
} SNDSAMPLEFORMAT;

typedef struct SNDSAMPLEATTR {
    // total size: 0x68
    short detune; // offset 0x0, size 0x2
    signed char priority; // offset 0x2, size 0x1
    signed char vol; // offset 0x3, size 0x1
    signed char pan; // offset 0x4, size 0x1
    signed char fxlevel0; // offset 0x5, size 0x1
    signed char bendrange; // offset 0x6, size 0x1
    unsigned char platformver; // offset 0x7, size 0x1
    unsigned short rendermode; // offset 0x8, size 0x2
    char padchar[2]; // offset 0xA, size 0x2
    unsigned short azimuth[6]; // offset 0xC, size 0xC
    void * ptsdata[6]; // offset 0x18, size 0x18
    int tsdatasize[6]; // offset 0x30, size 0x18
    void * puserdata[4]; // offset 0x48, size 0x10
    int userdatasize[4]; // offset 0x58, size 0x10
} SNDSAMPLEATTR;

typedef struct SNDSAMPLEDESC {
    // total size: 0x1C
    unsigned int totalframes; // offset 0x0, size 0x4
    void * psamples[6]; // offset 0x4, size 0x18
} SNDSAMPLEDESC;

typedef struct SNDPLAYOPTS {
    // total size: 0x18
    signed char vol; // offset 0x0, size 0x1
    signed char bend; // offset 0x1, size 0x1
    signed char keynum; // offset 0x2, size 0x1
    signed char velocity; // offset 0x3, size 0x1
    signed char drylevel; // offset 0x4, size 0x1
    signed char fxlevel0; // offset 0x5, size 0x1
    char pad[2]; // offset 0x6, size 0x2
    unsigned short azimuth; // offset 0x8, size 0x2
    short elevation; // offset 0xA, size 0x2
    unsigned short pitchmult; // offset 0xC, size 0x2
    unsigned short timemult; // offset 0xE, size 0x2
    unsigned short tempomult; // offset 0x10, size 0x2
    unsigned short pad2; // offset 0x12, size 0x2
    unsigned short lowpasscutoff; // offset 0x14, size 0x2
    unsigned short highpasscutoff; // offset 0x16, size 0x2
} SNDPLAYOPTS;

typedef struct SNDSTREAMFXVOLUME {
    // total size: 0x4
    float fxLevel; // offset 0x0, size 0x4
} SNDSTREAMFXVOLUME;

typedef struct SNDFILTERDEF {
    // total size: 0x18
    int id; // offset 0x0, size 0x4
    int statesize; // offset 0x4, size 0x4
    int priority; // offset 0x8, size 0x4
    void (* filterinit)(void *, int, int); // offset 0xC, size 0x4
    int (* filter)(void *, int, void *, void *, int); // offset 0x10, size 0x4
    void (* filterrestore)(void *); // offset 0x14, size 0x4
} SNDFILTERDEF;

typedef struct SNDLINKLIST {
    // total size: 0xC
    struct SNDLINKNODE * phead; // offset 0x0, size 0x4
    struct SNDLINKNODE * ptail; // offset 0x4, size 0x4
    int items; // offset 0x8, size 0x4
} SNDLINKLIST;

typedef struct SNDLINKNODE {
    // total size: 0x8
    struct SNDLINKNODE * pnext; // offset 0x0, size 0x4
    struct SNDLINKNODE * pprev; // offset 0x4, size 0x4
} SNDLINKNODE;

typedef struct SNDSTREAMREQUEST {
    // total size: 0x28
    struct SNDLINKNODE node; // offset 0x0, size 0x8
    volatile int streamrequestid; // offset 0x8, size 0x4
    volatile int sndrequesthandle; // offset 0xC, size 0x4
    volatile unsigned int avgdatarate; // offset 0x10, size 0x4
    volatile unsigned int currentframe; // offset 0x14, size 0x4
    volatile int totalframes; // offset 0x18, size 0x4
    volatile unsigned int outstandingframes; // offset 0x1C, size 0x4
    volatile int holdtime; // offset 0x20, size 0x4
    unsigned char continuation; // offset 0x24, size 0x1
    volatile unsigned char isgeneric; // offset 0x25, size 0x1
    char pad[2]; // offset 0x26, size 0x2
} SNDSTREAMREQUEST;

typedef struct CListDNode {
    // total size: 0x8
    struct CListDNode * pnext; // offset 0x0, size 0x4
    struct CListDNode * pprev; // offset 0x4, size 0x4
} CListDNode;

typedef struct VariableTimerClient {
    // total size: 0x10
    CListDNode ln; // offset 0x0, size 0x8
    void (* pClientFunc)(void *); // offset 0x8, size 0x4
    void * pClientData; // offset 0xC, size 0x4
} VariableTimerClient;

typedef struct Fader {
    // total size: 0x18
    VariableTimerClient variableTimerClient; // offset 0x0, size 0x10
    float targetVol; // offset 0x10, size 0x4
    float incrementPerUpdate; // offset 0x14, size 0x4
} Fader;

typedef struct StreamSourceChannelState {
    // total size: 0x24
    float azimuth; // offset 0x0, size 0x4
    unsigned char isDirect; // offset 0x4, size 0x1
    unsigned char sourceChannel; // offset 0x5, size 0x1
    char pad[2]; // offset 0x6, size 0x2
    float vol; // offset 0x8, size 0x4
    Fader fader; // offset 0xC, size 0x18
} StreamSourceChannelState;

typedef struct SNDSTREAMCHANNEL {
    // total size: 0x214
    int streamhandle; // offset 0x0, size 0x4
    volatile int shandle; // offset 0x4, size 0x4
    volatile int packetinstancehandle; // offset 0x8, size 0x4
    volatile int lastsndrequesthandle; // offset 0xC, size 0x4
    volatile signed char state; // offset 0x10, size 0x1
    volatile signed char fromtap; // offset 0x11, size 0x1
    unsigned char useOldAzimuthBehaviour; // offset 0x12, size 0x1
    char pad[1]; // offset 0x13, size 0x1
    SNDSAMPLEFORMAT cursf; // offset 0x14, size 0x4
    SNDSAMPLEFORMAT newsf; // offset 0x18, size 0x4
    SNDSAMPLEATTR cursa; // offset 0x1C, size 0x68
    SNDSAMPLEATTR newsa; // offset 0x84, size 0x68
    SNDPLAYOPTS po; // offset 0xEC, size 0x18
    SNDSTREAMFXVOLUME * pFxVolume; // offset 0x104, size 0x4
    SNDFILTERDEF filter[1]; // offset 0x108, size 0x18
    SNDLINKLIST allocatedrequests; // offset 0x120, size 0xC
    SNDLINKLIST freerequests; // offset 0x12C, size 0xC
    volatile SNDSTREAMREQUEST * parsingrequest; // offset 0x138, size 0x4
    StreamSourceChannelState sourceChannelState[6]; // offset 0x13C, size 0xD8
} SNDSTREAMCHANNEL;

extern SNDGLOBALSTATE sndgs;

static inline int SNDI_ftoiround(float val) {
    int result;

    if (val >= 0.0f) { 
        result = val + 0.5f;
    } else {
        result = val - 0.5f;
    }

    return result;
}

static inline int SNDI_ftoifast(float val) {
    return SNDI_ftoiround(val * 127.0f);
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

// sserver.c
void SNDSYS_entercritical();
void SNDSYS_leavecritical();




// sgetpvol.c
int SNDCTRL_getprogvol(int shandle);

// sstgetpv.c
int SNDSTRM_getprogvol(int sndstreamhandle);

// stimerem.c
int SNDtimeremaining(int shandle);

#endif
