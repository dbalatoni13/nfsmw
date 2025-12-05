#ifndef EVENTS_EREPORTMILESTONEATSTAKE_H
#define EVENTS_EREPORTMILESTONEATSTAKE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EReportMilestoneAtStake : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        GMilestone * fMilestonePtr; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EReportMilestoneAtStake(GMilestone * pMilestonePtr);

    override virtual ~EReportMilestoneAtStake();

    override virtual const char *GetEventName() {
        return "EReportMilestoneAtStake";
    }

  private:
    GMilestone * fMilestonePtr; // offset: 0x8, size 0x4
};

void EReportMilestoneAtStake_MakeEvent_Callback(const void *staticData);

#endif
