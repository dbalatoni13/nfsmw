#ifndef EVENTS_ESHOWTIMEEXTENSION_H
#define EVENTS_ESHOWTIMEEXTENSION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EShowTimeExtension : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        IPlayer *fPlayer;  // offset: 0x4, size 0x4
        float fTimeToShow; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    EShowTimeExtension(IPlayer *pPlayer, float pTimeToShow);

    ~EShowTimeExtension() override;

    const char *GetEventName() override;

  private:
    IPlayer *fPlayer;  // offset: 0x8, size 0x4
    float fTimeToShow; // offset: 0xc, size 0x4
};

void EShowTimeExtension_MakeEvent_Callback(const void *staticData);

#endif
