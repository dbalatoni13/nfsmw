#ifndef WORLD_WORLDCONN_H
#define WORLD_WORLDCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace WorldConn {

class Reference {
    // total size: 0x10
    unsigned int mWorldID; // offset 0x0, size 0x4
    const bMatrix4 *mMatrix; // offset 0x4, size 0x4
    const bVector3 *mVelocity; // offset 0x8, size 0x4
    const bVector3 *mAcceleration; // offset 0xC, size 0x4

public:
    Reference(unsigned int);
    void Set(unsigned int);

    const bMatrix4 *GetMatrix() const {
        return this->mMatrix;
    }
};

}

#endif
