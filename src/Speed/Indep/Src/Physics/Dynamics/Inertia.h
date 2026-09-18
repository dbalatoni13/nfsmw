#ifndef DYNAMICS_INERTIA_H
#define DYNAMICS_INERTIA_H

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"

namespace Dynamics {
namespace Inertia {

// total size: 0xC
class Tensor : public UVector3 {
  public:
    Tensor() {}

    Tensor(const UMath::Vector3 &From) : UVector3(From) {}

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


    void AddPointMass(const UMath::Vector3 &r, float mass) {
        UVector3 Ioff;
        float xx = r.x * r.x;
        float yy = r.y * r.y;
        float zz = r.z * r.z;

        Ioff.x = yy + zz;
        Ioff.y = xx + zz;
        Ioff.z = xx + yy;

        UMath::Scale(Ioff, mass, Ioff);
        UMath::Add(*this, Ioff, *this);
    }
};

// total size: 0xC
class ParallelAxis : public Tensor {
  public:
    ParallelAxis(float mass, float rx, float ry, float rz) {
        float x2 = rx * rx;
        float y2 = ry * ry;
        float z2 = rz * rz;

        x = y2 + z2;
        y = x2 + z2;
        z = x2 + y2;

        *this *= mass;
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
