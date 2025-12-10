#ifndef EVENTS_EHIDERACEOVERMESSAGE_H
#define EVENTS_EHIDERACEOVERMESSAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EHideRaceOverMessage : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        IPlayer *fPlayer; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EHideRaceOverMessage(IPlayer *pPlayer);

    ~EHideRaceOverMessage() override;

    const char *GetEventName() override;

  private:
    IPlayer *fPlayer; // offset: 0x8, size 0x4
};

void EHideRaceOverMessage_MakeEvent_Callback(const void *staticData);

#endif
