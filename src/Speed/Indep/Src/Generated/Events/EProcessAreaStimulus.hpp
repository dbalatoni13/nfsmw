#ifndef EVENTS_EPROCESSAREASTIMULUS_H
#define EVENTS_EPROCESSAREASTIMULUS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x28
class EProcessAreaStimulus : public Event {
  public:
    // total size: 0x14
    struct StaticData : public Event::StaticData {
        UCrc32 fSystemName; // offset: 0x4, size 0x4
        UCrc32 fStimulus;   // offset: 0x8, size 0x4
        UCrc32 fQueueMode;  // offset: 0xc, size 0x4
        float fRadius;      // offset: 0x10, size 0x4
    };

    // enum { kEventID = 0 };

    EProcessAreaStimulus(UCrc32 pSystemName, UCrc32 pStimulus, UCrc32 pQueueMode, float pRadius, UMath::Vector4 pPosition);

    ~EProcessAreaStimulus() override;

    const char *GetEventName() override {
        return "EProcessAreaStimulus";
    }

  private:
    UCrc32 fSystemName; // offset: 0x8, size 0x4
    UCrc32 fStimulus;   // offset: 0xc, size 0x4
    UCrc32 fQueueMode;  // offset: 0x10, size 0x4
    float fRadius;      // offset: 0x14, size 0x4

    UMath::Vector4 fPosition; // offset: 0x1c, size 0x10
};

void EProcessAreaStimulus_MakeEvent_Callback(const void *staticData);

#endif
