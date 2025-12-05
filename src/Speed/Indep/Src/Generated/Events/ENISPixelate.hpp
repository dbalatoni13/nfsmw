#ifndef EVENTS_ENISPIXELATE_H
#define EVENTS_ENISPIXELATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class ENISPixelate : public Event {
  public:
    // total size: 0x10
    struct StaticData : public Event::StaticData {
        int fEnable; // offset: 0x4, size 0x4
        float fWidth; // offset: 0x8, size 0x4
        float fHeight; // offset: 0xc, size 0x4
    };

    // enum { kEventID = 0 };

    ENISPixelate(int pEnable, float pWidth, float pHeight);

    override virtual ~ENISPixelate();

    override virtual const char *GetEventName() {
        return "ENISPixelate";
    }

  private:
    int fEnable; // offset: 0x8, size 0x4
    float fWidth; // offset: 0xc, size 0x4
    float fHeight; // offset: 0x10, size 0x4
};

void ENISPixelate_MakeEvent_Callback(const void *staticData);

#endif
