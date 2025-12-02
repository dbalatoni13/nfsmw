#ifndef GENERATED_EVENTS_EENGINEBLOWN_H
#define GENERATED_EVENTS_EENGINEBLOWN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x88
class ESimulate : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        UTL::COM::IUnknown *fSim; // offset 0x4, size 0x4
    };

    ESimulate(UTL::COM::IUnknown *pSim);

    UTL::COM::IUnknown *fSim; // offset 0x8, size 0x4
};

#endif
