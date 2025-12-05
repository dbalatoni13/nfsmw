#ifndef EVENTS_ENISWOLRDGEOMETRY_H
#define EVENTS_ENISWOLRDGEOMETRY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class ENISWolrdGeometry : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        const char * fSceneryGroupName; // offset: 0x4, size 0x4
        int fEnable; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    ENISWolrdGeometry(const char * pSceneryGroupName, int pEnable);

    override virtual ~ENISWolrdGeometry();

    override virtual const char *GetEventName() {
        return "ENISWolrdGeometry";
    }

  private:
    const char * fSceneryGroupName; // offset: 0x8, size 0x4
    int fEnable; // offset: 0xc, size 0x4
};

void ENISWolrdGeometry_MakeEvent_Callback(const void *staticData);

#endif
