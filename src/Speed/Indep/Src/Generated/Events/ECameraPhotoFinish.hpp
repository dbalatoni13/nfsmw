#ifndef EVENTS_ECAMERAPHOTOFINISH_H
#define EVENTS_ECAMERAPHOTOFINISH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class ECameraPhotoFinish : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    ECameraPhotoFinish();

    ~ECameraPhotoFinish() override;

    const char *GetEventName() override {
        return "ECameraPhotoFinish";
    }
};

void ECameraPhotoFinish_MakeEvent_Callback(const void *staticData);

#endif
