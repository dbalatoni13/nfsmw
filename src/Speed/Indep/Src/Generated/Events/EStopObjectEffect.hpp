#ifndef EVENTS_ESTOPOBJECTEFFECT_H
#define EVENTS_ESTOPOBJECTEFFECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EStopObjectEffect : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        UCrc32 fID; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EStopObjectEffect(UCrc32 pID, unsigned int phModel);

    ~EStopObjectEffect() override;

    const char *GetEventName() override {
        return "EStopObjectEffect";
    }

  private:
    UCrc32 fID; // offset: 0x8, size 0x4

    unsigned int fhModel; // offset: 0x10, size 0x4
};

void EStopObjectEffect_MakeEvent_Callback(const void *staticData);

#endif
