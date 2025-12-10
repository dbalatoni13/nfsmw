#ifndef EVENTS_ETRIGGERMOMENTNIS_H
#define EVENTS_ETRIGGERMOMENTNIS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ETriggerMomentNIS : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        const char *fSceneName; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ETriggerMomentNIS(const char *pSceneName, unsigned int phModel);

    ~ETriggerMomentNIS() override;

    const char *GetEventName() override;

  private:
    const char *fSceneName; // offset: 0x8, size 0x4

    unsigned int fhModel; // offset: 0x10, size 0x4
};

void ETriggerMomentNIS_MakeEvent_Callback(const void *staticData);

#endif
