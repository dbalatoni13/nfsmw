#ifndef EVENTS_EREPORTINFRACTION_H
#define EVENTS_EREPORTINFRACTION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"

// total size: 0xc
class EReportInfraction : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        GInfractionManager::InfractionType fInfraction; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EReportInfraction(GInfractionManager::InfractionType pInfraction);

    ~EReportInfraction() override;

    const char *GetEventName() const override;

  private:
    GInfractionManager::InfractionType fInfraction; // offset: 0x8, size 0x4
};

void EReportInfraction_MakeEvent_Callback(const void *staticData);

#endif
