#include "./spchi.h"

// total size: 0x8
struct VOXINGAME {
    EventSpec lastEventSpec; // offset 0x0, size 0x4
    int numEventTimes;       // offset 0x4, size 0x4
};

// total size: 0x14
struct SPCH_Callbacks {
    int (*request)(SPCHType_SampleRequestData *);          // offset 0x0, size 0x4
    int (*testRule)(EventSpec *, int, int, int);           // offset 0x4, size 0x4
    void (*setRule)(EventSpec *, int, int, int);           // offset 0x8, size 0x4
    SPCHType_EventRuleResult (*eventRule)(EventSpec *);    // offset 0xC, size 0x4
    int (*reparm)(int, unsigned int *);                    // offset 0x10, size 0x4
};

extern void *(*gMemAlloc)(unsigned int);
extern void (*gMemFree)(void *);
extern int gSPCH_Initialized;
extern SPCH_Callbacks gCallbacks;
extern int (*gSPCH_AddEvent)(int, int, ...);
extern int gDataRate;
extern unsigned int gGameNum;
extern int gFilterSetting[8];
extern VOXINGAME gVoxInGame[8];
extern char spchlibauthor[0x3D];

void iSPCH_InitRandom(unsigned int gameSeed);
void iSPCH_InitSentenceChoice(void);
void SPCH_SetPreLoadTicks(int numTicks);
void iSPCH_InitEventDat(void);
void iSPCH_InitBanks(void);
void iSPCH_InitEventQueue(void);
int SPCH_AddEventV(int eventID, int eventDatID, ...);

void SPCH_SetMemCallbacks(void *(*memAlloc)(unsigned int), void (*memFree)(void *)) {
    gMemAlloc = memAlloc;
    gMemFree = memFree;
}

void *iSPCH_MemAlloc(unsigned int numBytes) {
    void *data;

    data = 0;
    if (gMemAlloc != 0) {
        data = gMemAlloc(numBytes);
    }
    return data;
}

void iSPCH_MemFree(void *data) {
    if (gMemFree != 0) {
        gMemFree(data);
    }
}

void iSPCH_InitInGame(void) {
    long i;

    for (i = 0; i < 8; i++) {
        gVoxInGame[i].lastEventSpec.eventID = -1;
        gVoxInGame[i].numEventTimes = 0;
    }
}

int SPCH_GetSampleDataRate(int sampleRate, int sampleBits, CompressionType type) {
    int bytesPerSec;

    bytesPerSec = sampleRate * sampleBits / 8;
    switch (type) {
    case kSPCH_Compression_None:
        break;
    case kSPCH_Compression_MicroTalk:
        bytesPerSec = bytesPerSec / 10;
        break;
    case kSPCH_Compression_XA:
        bytesPerSec = bytesPerSec * 2 / 7;
        break;
    }
    return bytesPerSec;
}

void SPCH_InitRuleCallbacks(int (*ruleTest)(EventSpec *, int, int, int),
                            void (*ruleSet)(EventSpec *, int, int, int)) {
    if (gSPCH_Initialized != 0x01789A34) {
        return;
    }
    gCallbacks.testRule = ruleTest;
    gCallbacks.setRule = ruleSet;
}

static void iSPCH_InitCallbacks(void) {
    gCallbacks.request = 0;
    gCallbacks.testRule = 0;
    gCallbacks.setRule = 0;
    gCallbacks.eventRule = 0;
    gCallbacks.reparm = 0;
}

void SPCH_InitReparmCallback(int (*reparmer)(int, unsigned int *)) {
    if (gSPCH_Initialized != 0x01789A34) {
        return;
    }
    gCallbacks.reparm = reparmer;
}

void SPCH_InitEventRuleCallback(SPCHType_EventRuleResult (*eventRuleTest)(EventSpec *)) {
    if (gSPCH_Initialized != 0x01789A34) {
        return;
    }
    gCallbacks.eventRule = eventRuleTest;
}

int SPCH_Init(int (*sampleRequest)(SPCHType_SampleRequestData *), unsigned int gameSeed,
              int sampleDataRate) {
    int i;

    spchlibauthor[0] = 'S';
    if (gMemAlloc == 0) {
        goto abort;
    }
    if (gMemFree == 0) {
        goto abort;
    }
    iSPCH_InitCallbacks();
    gCallbacks.request = sampleRequest;
    gGameNum = gameSeed;
    gDataRate = sampleDataRate;
    gSPCH_AddEvent = SPCH_AddEventV;
    iSPCH_InitRandom(gameSeed);
    iSPCH_InitSentenceChoice();
    SPCH_SetPreLoadTicks(0);
    for (i = 0; i < 8; i++) {
        gFilterSetting[i] = 0;
    }
    iSPCH_InitEventDat();
    iSPCH_InitInGame();
    iSPCH_InitBanks();
    iSPCH_InitEventQueue();
    gSPCH_Initialized = 0x01789A34;
    return 1;
abort:
    return 0;
}
