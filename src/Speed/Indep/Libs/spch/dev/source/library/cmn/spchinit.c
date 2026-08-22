#include "spch/spch.h"

extern char spchlibauthor[];
extern unsigned int gGameNum;
extern int gFilterSetting[8];
extern void iSPCH_InitRandom(unsigned int gameSeed);
extern void iSPCH_InitSentenceChoice();
extern void SPCH_SetPreLoadTicks(int ticks);
extern int SPCH_AddEventV(int eventID, int numArgs, ...);
extern void iSPCH_InitEventDat();
extern void iSPCH_InitBanks();
extern void iSPCH_InitEventQueue();

void SPCH_SetMemCallbacks(MemAllocFuncPtr memAlloc, MemFreeFuncPtr memFree) {
    gMemAlloc = memAlloc;
    gMemFree = memFree;
}

void *iSPCH_MemAlloc(unsigned int numBytes) {
    void *data = 0;

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

void iSPCH_InitInGame() {
    long i;

    i = 0;
    do {
        gVoxInGame[i].lastEventSpec.eventID = 0xFFFF;
        gVoxInGame[i].numEventTimes = 0;
        i++;
    } while (i < 8);
}

int SPCH_GetSampleDataRate(int sampleRate, int sampleBits, CompressionType type) {
    int bytesPerSec = sampleRate * sampleBits;

    if (bytesPerSec < 0) {
        bytesPerSec += 7;
    }
    bytesPerSec >>= 3;
    switch (type) {
    case kSPCH_Compression_MicroTalk:
        bytesPerSec = bytesPerSec / 10;
        break;
    case kSPCH_Compression_XA:
        bytesPerSec = bytesPerSec * 2 / 7;
        break;
    }
    return bytesPerSec;
}

void SPCH_InitRuleCallbacks(TestSentenceRuleFuncPtr ruleTest, SetSentenceRuleFuncPtr ruleSet) {
    if (gSPCH_Initialized != 0x1789A34) {
        goto abort;
    }
    gCallbacks.testRule = ruleTest;
    gCallbacks.setRule = ruleSet;
abort:
    ;
}

static void iSPCH_InitCallbacks() {
    gCallbacks.request = 0;
    gCallbacks.testRule = 0;
    gCallbacks.setRule = 0;
    gCallbacks.eventRule = 0;
    gCallbacks.reparm = 0;
}

void SPCH_InitReparmCallback(ReparmFuncPtr reparmer) {
    if (gSPCH_Initialized != 0x1789A34) {
        goto abort;
    }
    gCallbacks.reparm = reparmer;
abort:
    ;
}

void SPCH_InitEventRuleCallback(EventRuleFuncPtr eventRuleTest) {
    if (gSPCH_Initialized != 0x1789A34) {
        goto abort;
    }
    gCallbacks.eventRule = eventRuleTest;
abort:
    ;
}

int SPCH_Init(int (*sampleRequest)(SPCHType_SampleRequestData *), unsigned int gameSeed, int sampleDataRate) {
    int i;

    spchlibauthor[0] = 'S';
    if (gMemAlloc == 0 || gMemFree == 0) {
        goto abort;
    }
    iSPCH_InitCallbacks();
    gCallbacks.request = sampleRequest;
    gSPCH_AddEvent = SPCH_AddEventV;
    gDataRate = sampleDataRate;
    gGameNum = gameSeed;
    iSPCH_InitRandom(gameSeed);
    iSPCH_InitSentenceChoice();
    SPCH_SetPreLoadTicks(0);
    i = 0;
    do {
        gFilterSetting[i] = 0;
        i++;
    } while (i < 8);
    iSPCH_InitEventDat();
    iSPCH_InitInGame();
    iSPCH_InitBanks();
    iSPCH_InitEventQueue();
    gSPCH_Initialized = 0x1789A34;
    return 1;
abort:
    return 0;
}
