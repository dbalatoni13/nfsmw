#ifndef EVENTS_ENISCARDAMAGERESET_H
#define EVENTS_ENISCARDAMAGERESET_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ENISCarDamageReset : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    ENISCarDamageReset(unsigned int phSimable);

    ~ENISCarDamageReset() override;

    const char *GetEventName() override {
        return "ENISCarDamageReset";
    }

  private:
    unsigned int fhSimable; // offset: 0xc, size 0x4
};

void ENISCarDamageReset_MakeEvent_Callback(const void *staticData);

#endif
