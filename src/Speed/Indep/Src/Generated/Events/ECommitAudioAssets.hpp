#ifndef EVENTS_ECOMMITAUDIOASSETS_H
#define EVENTS_ECOMMITAUDIOASSETS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class ECommitAudioAssets : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    ECommitAudioAssets();

    ~ECommitAudioAssets() override;

    const char *GetEventName() override {
        return "ECommitAudioAssets";
    }
};

void ECommitAudioAssets_MakeEvent_Callback(const void *staticData);

#endif
