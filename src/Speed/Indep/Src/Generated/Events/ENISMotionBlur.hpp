#ifndef EVENTS_ENISMOTIONBLUR_H
#define EVENTS_ENISMOTIONBLUR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ENISMotionBlur : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fEnableMotionBlur; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ENISMotionBlur(int pEnableMotionBlur);

    ~ENISMotionBlur() override;

    const char *GetEventName() override {
        return "ENISMotionBlur";
    }

  private:
    int fEnableMotionBlur; // offset: 0x8, size 0x4
};

void ENISMotionBlur_MakeEvent_Callback(const void *staticData);

#endif
