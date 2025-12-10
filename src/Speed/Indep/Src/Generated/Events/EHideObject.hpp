#ifndef EVENTS_EHIDEOBJECT_H
#define EVENTS_EHIDEOBJECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EHideObject : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EHideObject(unsigned int phModel);

    ~EHideObject() override;

    const char *GetEventName() override;

  private:
    unsigned int fhModel; // offset: 0xc, size 0x4
};

void EHideObject_MakeEvent_Callback(const void *staticData);

#endif
