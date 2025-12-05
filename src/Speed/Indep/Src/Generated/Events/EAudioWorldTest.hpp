#ifndef EVENTS_EAUDIOWORLDTEST_H
#define EVENTS_EAUDIOWORLDTEST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EAudioWorldTest : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {
    };

    // enum { kEventID = 0 };

    EAudioWorldTest();

    override virtual ~EAudioWorldTest();

    override virtual const char *GetEventName() {
        return "EAudioWorldTest";
    }
};

void EAudioWorldTest_MakeEvent_Callback(const void *staticData);

#endif
