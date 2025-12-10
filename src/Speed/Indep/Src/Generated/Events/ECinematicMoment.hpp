#ifndef EVENTS_ECINEMATICMOMENT_H
#define EVENTS_ECINEMATICMOMENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class ECinematicMoment : public Event {
  public:
    // total size: 0x10
    struct StaticData : public Event::StaticData {
        const char *fGenericGroupName; // offset: 0x4, size 0x4
        const char *fGenericTrackName; // offset: 0x8, size 0x4
        float fEventRadius;            // offset: 0xc, size 0x4
    };

    // enum { kEventID = 0 };

    ECinematicMoment(const char *pGenericGroupName, const char *pGenericTrackName, float pEventRadius);

    ~ECinematicMoment() override;

    const char *GetEventName() override;

  private:
    const char *fGenericGroupName; // offset: 0x8, size 0x4
    const char *fGenericTrackName; // offset: 0xc, size 0x4
    float fEventRadius;            // offset: 0x10, size 0x4
};

void ECinematicMoment_MakeEvent_Callback(const void *staticData);

#endif
