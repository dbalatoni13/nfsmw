#ifndef EVENTS_EAWARDUPGRADE_H
#define EVENTS_EAWARDUPGRADE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EAwardUpgrade : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        unsigned int fUpgradeKey; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EAwardUpgrade(unsigned int pUpgradeKey);

    ~EAwardUpgrade() override;

    const char *GetEventName() override;

  private:
    unsigned int fUpgradeKey; // offset: 0x8, size 0x4
};

void EAwardUpgrade_MakeEvent_Callback(const void *staticData);

#endif
