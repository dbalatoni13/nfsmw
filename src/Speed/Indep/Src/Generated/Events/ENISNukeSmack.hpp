#ifndef EVENTS_ENISNUKESMACK_H
#define EVENTS_ENISNUKESMACK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x18
class ENISNukeSmack : public Event {
  public:
    // total size: 0x14
    struct StaticData : public Event::StaticData {
        float fPositionX; // offset: 0x4, size 0x4
        float fPositionY; // offset: 0x8, size 0x4
        float fPositionZ; // offset: 0xc, size 0x4
        float fRadius;    // offset: 0x10, size 0x4
    };

    // enum { kEventID = 0 };

    ENISNukeSmack(float pPositionX, float pPositionY, float pPositionZ, float pRadius);

    ~ENISNukeSmack() override;

    const char *GetEventName() override {
        return "ENISNukeSmack";
    }

  private:
    float fPositionX; // offset: 0x8, size 0x4
    float fPositionY; // offset: 0xc, size 0x4
    float fPositionZ; // offset: 0x10, size 0x4
    float fRadius;    // offset: 0x14, size 0x4
};

void ENISNukeSmack_MakeEvent_Callback(const void *staticData);

#endif
