#ifndef EVENTS_EFADESCREENOFF_H
#define EVENTS_EFADESCREENOFF_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EFadeScreenOff : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fDurationScript; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EFadeScreenOff(int pDurationScript);

    ~EFadeScreenOff() override;

    const char *GetEventName() override {
        return "EFadeScreenOff";
    }

  private:
    int fDurationScript; // offset: 0x8, size 0x4
};

void EFadeScreenOff_MakeEvent_Callback(const void *staticData);

#endif
