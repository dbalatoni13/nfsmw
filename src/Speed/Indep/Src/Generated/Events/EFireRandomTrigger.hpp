#ifndef EVENTS_EFIRERANDOMTRIGGER_H
#define EVENTS_EFIRERANDOMTRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x2c
class EFireRandomTrigger : public Event {
  public:
    // total size: 0x24
    struct StaticData : public Event::StaticData {
        Trigger *fTrigger1; // offset: 0x4, size 0x4
        float fChance1;     // offset: 0x8, size 0x4
        Trigger *fTrigger2; // offset: 0xc, size 0x4
        float fChance2;     // offset: 0x10, size 0x4
        Trigger *fTrigger3; // offset: 0x14, size 0x4
        float fChance3;     // offset: 0x18, size 0x4
        Trigger *fTrigger4; // offset: 0x1c, size 0x4
        float fChance4;     // offset: 0x20, size 0x4
    };

    // enum { kEventID = 0 };

    EFireRandomTrigger(Trigger *pTrigger1, float pChance1, Trigger *pTrigger2, float pChance2, Trigger *pTrigger3, float pChance3, Trigger *pTrigger4,
                       float pChance4, unsigned int phSimable);

    ~EFireRandomTrigger() override;

    const char *GetEventName() override {
        return "EFireRandomTrigger";
    }

  private:
    Trigger *fTrigger1; // offset: 0x8, size 0x4
    float fChance1;     // offset: 0xc, size 0x4
    Trigger *fTrigger2; // offset: 0x10, size 0x4
    float fChance2;     // offset: 0x14, size 0x4
    Trigger *fTrigger3; // offset: 0x18, size 0x4
    float fChance3;     // offset: 0x1c, size 0x4
    Trigger *fTrigger4; // offset: 0x20, size 0x4
    float fChance4;     // offset: 0x24, size 0x4

    unsigned int fhSimable; // offset: 0x2c, size 0x4
};

void EFireRandomTrigger_MakeEvent_Callback(const void *staticData);

#endif
