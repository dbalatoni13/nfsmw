#ifndef EVENTS_ESHOCKOBJECT_H
#define EVENTS_ESHOCKOBJECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EShockObject : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        float fAmount; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EShockObject(float pAmount, unsigned int phSimable);

    override virtual ~EShockObject();

    override virtual const char *GetEventName() {
        return "EShockObject";
    }

  private:
    float fAmount; // offset: 0x8, size 0x4

    unsigned int fhSimable; // offset: 0x10, size 0x4
};

void EShockObject_MakeEvent_Callback(const void *staticData);

#endif
