#ifndef EVENTS_EJOINTDETACHED_H
#define EVENTS_EJOINTDETACHED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EJointDetached : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        HSIMABLE fhObject; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EJointDetached(HSIMABLE phObject);

    override virtual ~EJointDetached();

    override virtual const char *GetEventName() {
        return "EJointDetached";
    }

  private:
    HSIMABLE fhObject; // offset: 0x8, size 0x4
};

void EJointDetached_MakeEvent_Callback(const void *staticData);

#endif
