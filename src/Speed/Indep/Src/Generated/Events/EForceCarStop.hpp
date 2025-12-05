#ifndef EVENTS_EFORCECARSTOP_H
#define EVENTS_EFORCECARSTOP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EForceCarStop : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fStopInstantly; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EForceCarStop(int pStopInstantly, unsigned int phSimable);

    override virtual ~EForceCarStop();

    override virtual const char *GetEventName() {
        return "EForceCarStop";
    }

  private:
    int fStopInstantly; // offset: 0x8, size 0x4

    unsigned int fhSimable; // offset: 0x10, size 0x4
};

void EForceCarStop_MakeEvent_Callback(const void *staticData);

#endif
