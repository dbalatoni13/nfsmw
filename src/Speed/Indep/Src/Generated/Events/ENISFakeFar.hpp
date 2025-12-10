#ifndef EVENTS_ENISFAKEFAR_H
#define EVENTS_ENISFAKEFAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ENISFakeFar : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        float fFakeFar; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ENISFakeFar(float pFakeFar);

    ~ENISFakeFar() override;

    const char *GetEventName() override;

  private:
    float fFakeFar; // offset: 0x8, size 0x4
};

void ENISFakeFar_MakeEvent_Callback(const void *staticData);

#endif
