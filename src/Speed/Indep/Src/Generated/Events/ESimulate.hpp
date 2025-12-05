#ifndef EVENTS_ESIMULATE_H
#define EVENTS_ESIMULATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ESimulate : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        UTL::COM::IUnknown *fSim; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ESimulate(UTL::COM::IUnknown *pSim);

    override virtual ~ESimulate();

    override virtual const char *GetEventName() {
        return "ESimulate";
    }

  private:
    UTL::COM::IUnknown *fSim; // offset: 0x8, size 0x4
};

void ESimulate_MakeEvent_Callback(const void *staticData);

#endif
