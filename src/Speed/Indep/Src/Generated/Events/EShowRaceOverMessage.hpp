#ifndef EVENTS_ESHOWRACEOVERMESSAGE_H
#define EVENTS_ESHOWRACEOVERMESSAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EShowRaceOverMessage : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        IPlayer * fPlayer; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EShowRaceOverMessage(IPlayer * pPlayer);

    override virtual ~EShowRaceOverMessage();

    override virtual const char *GetEventName() {
        return "EShowRaceOverMessage";
    }

  private:
    IPlayer * fPlayer; // offset: 0x8, size 0x4
};

void EShowRaceOverMessage_MakeEvent_Callback(const void *staticData);

#endif
