#ifndef EVENTS_EQUITTOFE_H
#define EVENTS_EQUITTOFE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EQuitToFE : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        eGarageType fGarageType;      // offset: 0x4, size 0x4
        const char *fFirstScreenName; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    EQuitToFE(eGarageType pGarageType, const char *pFirstScreenName);

    ~EQuitToFE() override;

    const char *GetEventName() override {
        return "EQuitToFE";
    }

  private:
    eGarageType fGarageType;      // offset: 0x8, size 0x4
    const char *fFirstScreenName; // offset: 0xc, size 0x4
};

void EQuitToFE_MakeEvent_Callback(const void *staticData);

#endif
