#ifndef EVENTS_ESETCOPAUTOSPAWNMODE_H
#define EVENTS_ESETCOPAUTOSPAWNMODE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ESetCopAutoSpawnMode : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fAutoSpawn; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ESetCopAutoSpawnMode(int pAutoSpawn);

    ~ESetCopAutoSpawnMode() override;

    const char *GetEventName() override;

  private:
    int fAutoSpawn; // offset: 0x8, size 0x4
};

void ESetCopAutoSpawnMode_MakeEvent_Callback(const void *staticData);

#endif
