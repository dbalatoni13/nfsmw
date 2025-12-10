#ifndef EVENTS_ENISWORLDANIMTRIGGER_H
#define EVENTS_ENISWORLDANIMTRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x18
class ENISWorldAnimTrigger : public Event {
  public:
    // total size: 0x14
    struct StaticData : public Event::StaticData {
        const char *fAnimTreeName; // offset: 0x4, size 0x4
        float fTimeSet;            // offset: 0x8, size 0x4
        int fAnimPause;            // offset: 0xc, size 0x4
        int fAnimHide;             // offset: 0x10, size 0x4
    };

    // enum { kEventID = 0 };

    ENISWorldAnimTrigger(const char *pAnimTreeName, float pTimeSet, int pAnimPause, int pAnimHide);

    ~ENISWorldAnimTrigger() override;

    const char *GetEventName() override;

  private:
    const char *fAnimTreeName; // offset: 0x8, size 0x4
    float fTimeSet;            // offset: 0xc, size 0x4
    int fAnimPause;            // offset: 0x10, size 0x4
    int fAnimHide;             // offset: 0x14, size 0x4
};

void ENISWorldAnimTrigger_MakeEvent_Callback(const void *staticData);

#endif
