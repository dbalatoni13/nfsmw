#ifndef EVENTS_EDAMAGELIGHTS_H
#define EVENTS_EDAMAGELIGHTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EDamageLights : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        UCrc32 fName; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EDamageLights(UCrc32 pName, unsigned int phSimable);

    override virtual ~EDamageLights();

    override virtual const char *GetEventName() {
        return "EDamageLights";
    }

  private:
    UCrc32 fName; // offset: 0x8, size 0x4

    unsigned int fhSimable; // offset: 0x10, size 0x4
};

void EDamageLights_MakeEvent_Callback(const void *staticData);

#endif
