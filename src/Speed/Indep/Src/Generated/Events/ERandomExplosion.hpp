#ifndef EVENTS_ERANDOMEXPLOSION_H
#define EVENTS_ERANDOMEXPLOSION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class ERandomExplosion : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        int fClass; // offset: 0x4, size 0x4
        int fType;  // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ERandomExplosion(int pClass, int pType, WTrigger *pTrigger);

    ~ERandomExplosion() override;

    const char *GetEventName() override {
        return "ERandomExplosion";
    }

  private:
    int fClass; // offset: 0x8, size 0x4
    int fType;  // offset: 0xc, size 0x4

    WTrigger *fTrigger; // offset: 0x14, size 0x4
};

void ERandomExplosion_MakeEvent_Callback(const void *staticData);

#endif
