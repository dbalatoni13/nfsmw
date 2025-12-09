#ifndef EVENTS_ENISNITRO_H
#define EVENTS_ENISNITRO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ENISNitro : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        unsigned int fOn; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ENISNitro(unsigned int pOn, unsigned int phSimable);

    ~ENISNitro() override;

    const char *GetEventName() override {
        return "ENISNitro";
    }

  private:
    unsigned int fOn; // offset: 0x8, size 0x4

    unsigned int fhSimable; // offset: 0x10, size 0x4
};

void ENISNitro_MakeEvent_Callback(const void *staticData);

#endif
