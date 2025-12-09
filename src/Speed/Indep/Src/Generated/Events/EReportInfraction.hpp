#ifndef EVENTS_EREPORTINFRACTION_H
#define EVENTS_EREPORTINFRACTION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EReportInfraction : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        InfractionType fInfraction; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EReportInfraction(InfractionType pInfraction);

    ~EReportInfraction() override;

    const char *GetEventName() override {
        return "EReportInfraction";
    }

  private:
    InfractionType fInfraction; // offset: 0x8, size 0x4
};

void EReportInfraction_MakeEvent_Callback(const void *staticData);

#endif
