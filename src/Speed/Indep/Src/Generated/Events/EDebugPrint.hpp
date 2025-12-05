#ifndef EVENTS_EDEBUGPRINT_H
#define EVENTS_EDEBUGPRINT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EDebugPrint : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        const char * fDebugMsg; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EDebugPrint(const char * pDebugMsg);

    override virtual ~EDebugPrint();

    override virtual const char *GetEventName() {
        return "EDebugPrint";
    }

  private:
    const char * fDebugMsg; // offset: 0x8, size 0x4
};

void EDebugPrint_MakeEvent_Callback(const void *staticData);

#endif
