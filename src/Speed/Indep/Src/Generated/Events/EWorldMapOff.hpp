#ifndef EVENTS_EWORLDMAPOFF_H
#define EVENTS_EWORLDMAPOFF_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EWorldMapOff : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    EWorldMapOff();

    override virtual ~EWorldMapOff();

    override virtual const char *GetEventName() {
        return "EWorldMapOff";
    }
};

void EWorldMapOff_MakeEvent_Callback(const void *staticData);

#endif
