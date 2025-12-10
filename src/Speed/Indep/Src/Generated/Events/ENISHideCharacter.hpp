#ifndef EVENTS_ENISHIDECHARACTER_H
#define EVENTS_ENISHIDECHARACTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ENISHideCharacter : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        const char *fAnimModelName; // offset: 0x4, size 0x4
        int fAnimShow;              // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ENISHideCharacter(const char *pAnimModelName, int pAnimShow);

    ~ENISHideCharacter() override;

    const char *GetEventName() override;

  private:
    const char *fAnimModelName; // offset: 0x8, size 0x4
    int fAnimShow;              // offset: 0xc, size 0x4
};

void ENISHideCharacter_MakeEvent_Callback(const void *staticData);

#endif
