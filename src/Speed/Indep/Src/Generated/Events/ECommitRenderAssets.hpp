#ifndef EVENTS_ECOMMITRENDERASSETS_H
#define EVENTS_ECOMMITRENDERASSETS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class ECommitRenderAssets : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    ECommitRenderAssets();

    ~ECommitRenderAssets() override;

    const char *GetEventName() override {
        return "ECommitRenderAssets";
    }
};

void ECommitRenderAssets_MakeEvent_Callback(const void *staticData);

#endif
