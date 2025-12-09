#ifndef EVENTS_EPLAYENDNIS_H
#define EVENTS_EPLAYENDNIS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EPlayEndNIS : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        const char *fSceneName; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EPlayEndNIS(const char *pSceneName);

    ~EPlayEndNIS() override;

    const char *GetEventName() override {
        return "EPlayEndNIS";
    }

  private:
    const char *fSceneName; // offset: 0x8, size 0x4
};

void EPlayEndNIS_MakeEvent_Callback(const void *staticData);

#endif
