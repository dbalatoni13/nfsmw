#ifndef EVENTS_ERESETSYSTEM_H
#define EVENTS_ERESETSYSTEM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class EResetSystem : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        UCrc32 fSystemName; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EResetSystem(UCrc32 pSystemName, unsigned int pEventSeqEngine, unsigned int pEventSeqSystem);

    ~EResetSystem() override;

    const char *GetEventName() override {
        return "EResetSystem";
    }

  private:
    UCrc32 fSystemName; // offset: 0x8, size 0x4

    unsigned int fEventSeqEngine; // offset: 0x10, size 0x4
    unsigned int fEventSeqSystem; // offset: 0x14, size 0x4
};

void EResetSystem_MakeEvent_Callback(const void *staticData);

#endif
