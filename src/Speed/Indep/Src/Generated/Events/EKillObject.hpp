#ifndef EVENTS_EKILLOBJECT_H
#define EVENTS_EKILLOBJECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EKillObject : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EKillObject(unsigned int phSimable, unsigned int phModel);

    ~EKillObject() override;

    const char *GetEventName() override {
        return "EKillObject";
    }

  private:
    unsigned int fhSimable; // offset: 0xc, size 0x4
    unsigned int fhModel;   // offset: 0x10, size 0x4
};

void EKillObject_MakeEvent_Callback(const void *staticData);

#endif
