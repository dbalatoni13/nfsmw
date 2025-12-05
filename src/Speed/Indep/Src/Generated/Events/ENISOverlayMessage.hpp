#ifndef EVENTS_ENISOVERLAYMESSAGE_H
#define EVENTS_ENISOVERLAYMESSAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ENISOverlayMessage : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        const char * fOverlayName; // offset: 0x4, size 0x4
        const char * fOverlayMessage; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ENISOverlayMessage(const char * pOverlayName, const char * pOverlayMessage);

    override virtual ~ENISOverlayMessage();

    override virtual const char *GetEventName() {
        return "ENISOverlayMessage";
    }

  private:
    const char * fOverlayName; // offset: 0x8, size 0x4
    const char * fOverlayMessage; // offset: 0xc, size 0x4
};

void ENISOverlayMessage_MakeEvent_Callback(const void *staticData);

#endif
