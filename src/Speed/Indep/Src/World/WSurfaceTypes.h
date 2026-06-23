#ifndef WORLD_WSURFACETYPES_H
#define WORLD_WSURFACETYPES_H

#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"

class WSurface : public CARP::CollisionSurface {
  public:
    static void InitSystem();

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

    static const WSurface kNull;
};

#endif
