#ifndef PHYSICS_DYNAMICS_INERTIA_H
#define PHYSICS_DYNAMICS_INERTIA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UVector.h"

namespace Dynamics {
namespace Inertia {

class Tensor : public UVector3 {
    // total size: 0xC

  public:
    const Tensor &operator=(const UMath::Vector3 &From) {
        x = From.x;
        y = From.y;
        z = From.z;
        return *this;
    }
};

}; // namespace Inertia
}; // namespace Dynamics

#endif
