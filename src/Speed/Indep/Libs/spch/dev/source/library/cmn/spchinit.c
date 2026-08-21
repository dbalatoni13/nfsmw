#include "spch/spch.h"

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
    int bytesPerSec;

    bytesPerSec = sampleRate * sampleBits;
    if (bytesPerSec < 0) {
        bytesPerSec += 7;
    }
    bytesPerSec = bytesPerSec >> 3;
    if (type == kSPCH_Compression_MicroTalk) {
        bytesPerSec = bytesPerSec / 10;
    } else if (type == kSPCH_Compression_XA) {
        bytesPerSec = bytesPerSec * 2 / 7;
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
