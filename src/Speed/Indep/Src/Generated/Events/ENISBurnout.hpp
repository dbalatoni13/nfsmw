#ifndef EVENTS_ENISBURNOUT_H
#define EVENTS_ENISBURNOUT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ENISBurnout : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        float fSpeed; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ENISBurnout(float pSpeed, unsigned int phSimable);

    override virtual ~ENISBurnout();

    override virtual const char *GetEventName() {
        return "ENISBurnout";
    }

  private:
    float fSpeed; // offset: 0x8, size 0x4

    unsigned int fhSimable; // offset: 0x10, size 0x4
};

void ENISBurnout_MakeEvent_Callback(const void *staticData);

#endif
