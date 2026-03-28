#ifndef WORLD_WSURFACETYPES_H
#define WORLD_WSURFACETYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"

class WSurface : public CollisionSurface {
  public:
    WSurface() {
        fSurface = 0;
        fFlags = 0;
    }
};

#endif
