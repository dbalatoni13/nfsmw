#ifndef EVENTS_EKILLJOINT_H
#define EVENTS_EKILLJOINT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EKillJoint : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EKillJoint(unsigned int phSimable);

    ~EKillJoint() override;

    const char *GetEventName() override;

  private:
    unsigned int fhSimable; // offset: 0xc, size 0x4
};

void EKillJoint_MakeEvent_Callback(const void *staticData);

#endif
