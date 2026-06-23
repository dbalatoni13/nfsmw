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
        UMath::Init(result, this->x > UMath::Epsilon ? 1.0f / this->x : this->x, this->y > UMath::Epsilon ? 1.0f / this->y : this->y,
                    this->z > UMath::Epsilon ? 1.0f / this->z : this->z);
        UMath::Matrix4 orientationInv;
        UMath::Matrix4 temp;
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

        x = (y2 + z2) / 12.0f;
        y = (x2 + z2) / 12.0f;
        z = (x2 + y2) / 12.0f;

        *this *= mass;
    }
};

}; // namespace Inertia
}; // namespace Dynamics

#endif
