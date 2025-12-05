#ifndef EVENTS_EACCELERATE_H
#define EVENTS_EACCELERATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x28
class EAccelerate : public Event {
  public:
    // total size: 0x1c
    struct StaticData : public Event::StaticData {
        float fAccelerationX; // offset: 0x4, size 0x4
        float fAccelerationY; // offset: 0x8, size 0x4
        float fAccelerationZ; // offset: 0xc, size 0x4
        int fLocalToObject; // offset: 0x10, size 0x4
        int fLocalToTrigger; // offset: 0x14, size 0x4
        int fPALOnly; // offset: 0x18, size 0x4
    };

    // enum { kEventID = 0 };

    EAccelerate(float pAccelerationX, float pAccelerationY, float pAccelerationZ, int pLocalToObject, int pLocalToTrigger, int pPALOnly, unsigned int phSimable, WTrigger * pTrigger);

    override virtual ~EAccelerate();

    override virtual const char *GetEventName() {
        return "EAccelerate";
    }

  private:
    float fAccelerationX; // offset: 0x8, size 0x4
    float fAccelerationY; // offset: 0xc, size 0x4
    float fAccelerationZ; // offset: 0x10, size 0x4
    int fLocalToObject; // offset: 0x14, size 0x4
    int fLocalToTrigger; // offset: 0x18, size 0x4
    int fPALOnly; // offset: 0x1c, size 0x4

    unsigned int fhSimable; // offset: 0x24, size 0x4
    WTrigger * fTrigger; // offset: 0x28, size 0x4
};

void EAccelerate_MakeEvent_Callback(const void *staticData);

#endif
