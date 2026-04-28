#ifndef WORLD_WSURFACETYPES_H
#define WORLD_WSURFACETYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"

class WSurface : public CollisionSurface {
  public:
    static void InitSystem();
    static WSurface kNull;

    WSurface() {
        fSurface = 0;
        fFlags = 0;
    }
    WSurface(const CollisionSurface &surface) {
        fSurface = surface.fSurface;
        fFlags = surface.fFlags;
    }
    WSurface(unsigned char surface, unsigned char flags) {
        fSurface = surface;
        fFlags = flags;
    }

    unsigned int Surface() const {
        return fSurface;
    }

    unsigned char &FlagsRef() {
        return fFlags;
    }

    unsigned char Flags() const {
        return fFlags;
    }

    bool HasFlag(unsigned char flag) const {
        return (fFlags & flag) != 0;
    }
};

#endif
