#ifndef EVENTS_ENISREATTACH_H
#define EVENTS_ENISREATTACH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ENISReattach : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fRepairDamage; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ENISReattach(int pRepairDamage, unsigned int phSimable);

    ~ENISReattach() override;

    const char *GetEventName() override;

  private:
    int fRepairDamage; // offset: 0x8, size 0x4

    unsigned int fhSimable; // offset: 0x10, size 0x4
};

void ENISReattach_MakeEvent_Callback(const void *staticData);

#endif
