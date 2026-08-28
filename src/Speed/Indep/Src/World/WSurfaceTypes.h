#ifndef WORLD_WSURFACETYPES_H
#define WORLD_WSURFACETYPES_H

#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"

class WSurface : public CARP::CollisionSurface {
  public:
    WSurface() {
        this->fSurface = 0;
        this->fFlags = 0;
    }

    WSurface(const CollisionSurface &surface) {
        this->fSurface = surface.fSurface;
        this->fFlags = surface.fFlags;
    }

    WSurface(unsigned char surface, unsigned char flags) {
        this->fSurface = surface;
        this->fFlags = flags;
    }

    static void InitSystem();

    const SimSurface &GetSimSurface() const {
        return SimSurface::kNull;
    }

    unsigned int Surface() const {
        return this->fSurface;
    }

    unsigned char &FlagsRef() {
        return this->fFlags;
    }

    unsigned char Flags() const {
        return this->fFlags;
    }

    bool HasFlag(unsigned char flag) const {
        return (this->fFlags & flag) != 0;
    }

    static const WSurface kNull;
};

#endif
