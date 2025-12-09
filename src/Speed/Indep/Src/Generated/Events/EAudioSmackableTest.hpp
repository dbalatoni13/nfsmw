#ifndef EVENTS_EAUDIOSMACKABLETEST_H
#define EVENTS_EAUDIOSMACKABLETEST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EAudioSmackableTest : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        float fTestRadius; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EAudioSmackableTest(float pTestRadius);

    ~EAudioSmackableTest() override;

    const char *GetEventName() override {
        return "EAudioSmackableTest";
    }

  private:
    float fTestRadius; // offset: 0x8, size 0x4
};

void EAudioSmackableTest_MakeEvent_Callback(const void *staticData);

#endif
