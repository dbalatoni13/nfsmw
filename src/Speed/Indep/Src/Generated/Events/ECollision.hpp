#ifndef EVENTS_ECOLLISION_H
#define EVENTS_ECOLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Sim/Collision.h"

// total size: 0x88
class ECollision : public Event {
  public:
    // enum { kEventID = 0 };

    ECollision(COLLISION_INFO pInfo);

    override virtual ~ECollision();

    override virtual const char *GetEventName() {
        return "ECollision";
    }

  private:
    COLLISION_INFO fInfo; // offset: 0xc, size 0x80
};

#endif
