#ifndef EVENTS_EQUITDEMO_H
#define EVENTS_EQUITDEMO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EQuitDemo : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        DemoDiscEndReason fEndDemoReason; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EQuitDemo(DemoDiscEndReason pEndDemoReason);

    ~EQuitDemo() override;

    const char *GetEventName() override {
        return "EQuitDemo";
    }

  private:
    DemoDiscEndReason fEndDemoReason; // offset: 0x8, size 0x4
};

void EQuitDemo_MakeEvent_Callback(const void *staticData);

#endif
