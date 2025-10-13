#ifndef GENERATED_EVENTS_EENGINEBLOWN_H
#define GENERATED_EVENTS_EENGINEBLOWN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Sim/Collision.h"

// total size: 0x88
class EEngineBlown : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        HSIMABLE fhSimable; // offset 0x4, size 0x4
    };

    EEngineBlown(HSIMABLE phSimable);

    HSIMABLE fhSimable; // offset 0x8, size 0x4
};

#endif
