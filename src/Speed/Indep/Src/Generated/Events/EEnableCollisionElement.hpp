#ifndef EVENTS_EENABLECOLLISIONELEMENT_H
#define EVENTS_EENABLECOLLISIONELEMENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EEnableCollisionElement : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        int fEnable; // offset: 0x4, size 0x4
        CollisionObject * fColElement; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    EEnableCollisionElement(int pEnable, CollisionObject * pColElement);

    override virtual ~EEnableCollisionElement();

    override virtual const char *GetEventName() {
        return "EEnableCollisionElement";
    }

  private:
    int fEnable; // offset: 0x8, size 0x4
    CollisionObject * fColElement; // offset: 0xc, size 0x4
};

void EEnableCollisionElement_MakeEvent_Callback(const void *staticData);

#endif
