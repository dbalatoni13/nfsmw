#ifndef DYNAMICS_INERTIA_H
#define DYNAMICS_INERTIA_H

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"

namespace Dynamics {
namespace Inertia {

// total size: 0xC
class Tensor : public UVector3 {
  public:
    const Tensor &operator=(const UMath::Vector3 &From) {
        x = From.x;
        y = From.y;
        z = From.z;
        return *this;
    }

    void GetInverseWorldTensor(const UMath::Matrix4 &orientation, UMath::Matrix4 &result) const {
        UMath::Matrix4 temp;
        UMath::Init(result, x > UMath::Epsilon ? 1.0f / x : x, y > UMath::Epsilon ? 1.0f / y : y, z > UMath::Epsilon ? 1.0f / z : z);
        UMath::Matrix4 orientationInv;
        UMath::Transpose(orientation, orientationInv);
        UMath::Mult(result, orientation, temp);
        UMath::Mult(orientationInv, temp, result);
    }
};

// total size: 0xC
class Box : public Tensor {
  public:
    Box(float mass, float width, float height, float length) {
        float x2 = width * width;
        float y2 = height * height;
        float z2 = length * length;

        x = (y2 + z2) / 1.2f;
        y = (x2 + z2) / 1.2f;
        z = (x2 + y2) / 1.2f;

        *this *= mass;
    }
};

}; // namespace Inertia
}; // namespace Dynamics

#endif
