#ifndef EVENTS_EDEBUGSCREENMESSAGE_H
#define EVENTS_EDEBUGSCREENMESSAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x18
class EDebugScreenMessage : public Event {
  public:
    // total size: 0x14
    struct StaticData : public Event::StaticData {
        const char * fDebugMsg; // offset: 0x4, size 0x4
        float fDuration; // offset: 0x8, size 0x4
        int fX; // offset: 0xc, size 0x4
        int fY; // offset: 0x10, size 0x4
    };

    // enum { kEventID = 0 };

    EDebugScreenMessage(const char * pDebugMsg, float pDuration, int pX, int pY);

    override virtual ~EDebugScreenMessage();

    override virtual const char *GetEventName() {
        return "EDebugScreenMessage";
    }

  private:
    const char * fDebugMsg; // offset: 0x8, size 0x4
    float fDuration; // offset: 0xc, size 0x4
    int fX; // offset: 0x10, size 0x4
    int fY; // offset: 0x14, size 0x4
};

void EDebugScreenMessage_MakeEvent_Callback(const void *staticData);

#endif
