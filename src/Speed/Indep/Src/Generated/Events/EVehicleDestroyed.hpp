#ifndef GENERATED_EVENTS_EVEHICLEDESTROYED_H
#define GENERATED_EVENTS_EVEHICLEDESTROYED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x88
class EVehicleDestroyed : public Event {
  public:
    EVehicleDestroyed(HSIMABLE phSimable);

    HSIMABLE fhSimable; // offset 0x8, size 0x80
};

#endif
