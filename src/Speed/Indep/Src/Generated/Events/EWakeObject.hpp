#ifndef EVENTS_EWAKEOBJECT_H
#define EVENTS_EWAKEOBJECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EWakeObject : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EWakeObject(unsigned int phModel);

    ~EWakeObject() override;

    const char *GetEventName() override {
        return "EWakeObject";
    }

  private:
    unsigned int fhModel; // offset: 0xc, size 0x4
};

void EWakeObject_MakeEvent_Callback(const void *staticData);

#endif
