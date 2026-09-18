#ifndef MAIN_EVENTDYNAMICDATA_H
#define MAIN_EVENTDYNAMICDATA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// TODO move?
// total size: 0x64
struct EventDynamicData {
    void Clear() {
        bMemSet(this, 0, sizeof(EventDynamicData));
    }

    UMath::Vector4 fPosition;        // offset 0x0, size 0x10
    UMath::Vector4 fVector;          // offset 0x10, size 0x10
    UMath::Vector4 fVelocity;        // offset 0x20, size 0x10
    UMath::Vector4 fAngularVelocity; // offset 0x30, size 0x10
    struct WTrigger *fTrigger;       // offset 0x40, size 0x4
    int fTriggerStimulus;            // offset 0x44, size 0x4
    uintptr_t fhSimable;             // offset 0x48, size 0x4
    uintptr_t fhActivity;            // offset 0x4C, size 0x4
    unsigned int fWorldID;           // offset 0x50, size 0x4
    uintptr_t fhModel;               // offset 0x54, size 0x4
    unsigned int fEventSeqEngine;    // offset 0x58, size 0x4
    unsigned int fEventSeqSystem;    // offset 0x5C, size 0x4
    unsigned int fEventSeqState;     // offset 0x60, size 0x4
};

extern EventDynamicData gEventDynamicData;

#endif
