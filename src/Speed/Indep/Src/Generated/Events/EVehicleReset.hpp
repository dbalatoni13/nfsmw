#ifndef EVENTS_EVEHICLERESET_H
#define EVENTS_EVEHICLERESET_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x24
class EVehicleReset : public Event {
  public:
    // total size: 0x20
    struct StaticData : public Event::StaticData {
        unsigned int fhSimable; // offset: 0x4, size 0x4
        UMath::Vector3 fFrom;   // offset: 0x8, size 0xc
        UMath::Vector3 fTo;     // offset: 0x14, size 0xc
    };

    // enum { kEventID = 0 };

    EVehicleReset(unsigned int phSimable, UMath::Vector3 pFrom, UMath::Vector3 pTo);

    ~EVehicleReset() override;

    const char *GetEventName() override;

  private:
    unsigned int fhSimable; // offset: 0x8, size 0x4
    UMath::Vector3 fFrom;   // offset: 0xc, size 0xc
    UMath::Vector3 fTo;     // offset: 0x18, size 0xc
};

void EVehicleReset_MakeEvent_Callback(const void *staticData);

#endif
