#ifndef PHYSICS_DYNAMICS_INERTIA_H
#define PHYSICS_DYNAMICS_INERTIA_H

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
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

    void GetInverseWorldTensor(const UMath::Matrix4 &orientation, UMath::Matrix4 &result) const {
        UMath::Matrix4 temp;
        UMath::Init(result, x > FLOAT_EPSILON ? 1.0f / x : x, y > FLOAT_EPSILON ? 1.0f / y : y, z > FLOAT_EPSILON ? 1.0f / z : z);
        UMath::Matrix4 orientationInv;
        UMath::Transpose(orientation, orientationInv);
        UMath::Mult(result, orientation, temp);
        UMath::Mult(orientationInv, temp, result);
    }
};

class Box : public Tensor {
    // total size: 0xC

  public:
    Box(float mass, float width, float height, float length) {
        float x2 = width * width;
        float y2 = height * height;
        float z2 = length * length;

        x = (y2 + z2) * mass;
        y = (x2 + z2) * mass;
        z = (x2 + y2) * mass;

        *this *= 1000000.0f;
    }
};

}; // namespace Inertia
}; // namespace Dynamics

#endif
