#ifndef EVENTS_ENISCONSTRAINT_H
#define EVENTS_ENISCONSTRAINT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ENISConstraint : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        float fDegrees; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ENISConstraint(float pDegrees, unsigned int phSimable);

    override virtual ~ENISConstraint();

    override virtual const char *GetEventName() {
        return "ENISConstraint";
    }

  private:
    float fDegrees; // offset: 0x8, size 0x4

    unsigned int fhSimable; // offset: 0x10, size 0x4
};

void ENISConstraint_MakeEvent_Callback(const void *staticData);

#endif
