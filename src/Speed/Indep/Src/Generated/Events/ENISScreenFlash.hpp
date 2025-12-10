#ifndef EVENTS_ENISSCREENFLASH_H
#define EVENTS_ENISSCREENFLASH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ENISScreenFlash : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        float fLength; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ENISScreenFlash(float pLength);

    ~ENISScreenFlash() override;

    const char *GetEventName() override;

  private:
    float fLength; // offset: 0x8, size 0x4
};

void ENISScreenFlash_MakeEvent_Callback(const void *staticData);

#endif
