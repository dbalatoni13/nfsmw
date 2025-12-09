#ifndef EVENTS_EBAILPURSUIT_H
#define EVENTS_EBAILPURSUIT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EBailPursuit : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        int fPlayAudio;    // offset: 0x4, size 0x4
        int fDisperseCops; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    EBailPursuit(int pPlayAudio, int pDisperseCops);

    ~EBailPursuit() override;

    const char *GetEventName() override {
        return "EBailPursuit";
    }

  private:
    int fPlayAudio;    // offset: 0x8, size 0x4
    int fDisperseCops; // offset: 0xc, size 0x4
};

void EBailPursuit_MakeEvent_Callback(const void *staticData);

#endif
