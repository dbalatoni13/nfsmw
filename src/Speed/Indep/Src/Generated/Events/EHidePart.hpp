#ifndef EVENTS_EHIDEPART_H
#define EVENTS_EHIDEPART_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EHidePart : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        UCrc32 fPartName; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EHidePart(UCrc32 pPartName, unsigned int phModel);

    override virtual ~EHidePart();

    override virtual const char *GetEventName() {
        return "EHidePart";
    }

  private:
    UCrc32 fPartName; // offset: 0x8, size 0x4

    unsigned int fhModel; // offset: 0x10, size 0x4
};

void EHidePart_MakeEvent_Callback(const void *staticData);

#endif
