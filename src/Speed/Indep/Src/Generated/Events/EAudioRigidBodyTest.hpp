#ifndef EVENTS_EAUDIORIGIDBODYTEST_H
#define EVENTS_EAUDIORIGIDBODYTEST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EAudioRigidBodyTest : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EAudioRigidBodyTest();

    ~EAudioRigidBodyTest() override;

    const char *GetEventName() override;
};

void EAudioRigidBodyTest_MakeEvent_Callback(const void *staticData);

#endif
