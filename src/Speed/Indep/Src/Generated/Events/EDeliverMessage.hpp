#ifndef EVENTS_EDELIVERMESSAGE_H
#define EVENTS_EDELIVERMESSAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EDeliverMessage : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        Message *fMessage; // offset: 0x4, size 0x4
        UCrc32 fPort;      // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    EDeliverMessage(Message *pMessage, UCrc32 pPort);

    ~EDeliverMessage() override;

    const char *GetEventName() override;

  private:
    Message *fMessage; // offset: 0x8, size 0x4
    UCrc32 fPort;      // offset: 0xc, size 0x4
};

void EDeliverMessage_MakeEvent_Callback(const void *staticData);

#endif
