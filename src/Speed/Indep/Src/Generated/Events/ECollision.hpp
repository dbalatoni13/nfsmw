#ifndef GENERATED_EVENTS_ECOLLISION_H
#define GENERATED_EVENTS_ECOLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Sim/Collision.h"

// total size: 0x88
class ECollision : public Event {
  public:
    ECollision(COLLISION_INFO pInfo);

    COLLISION_INFO fInfo; // offset 0x8, size 0x80
};

#endif
