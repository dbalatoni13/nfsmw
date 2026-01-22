#ifndef AI_AIMATH_H
#define AI_AIMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

namespace AI {

namespace Math {

// total size: 0x10
class FloatSpring {
  public:
    FloatSpring(float spring_k, float damper_k) : mX(0.0f), mV(0.0f), mC(spring_k), mD(damper_k) {}

    float GetPosition() const {
        return mX;
    }

    void SetPosition(float x) {
        mX = x;
        mV = 0.0f;
    }

    float Integrate(float newvalue, float dT) {
        float v = mV;
        float dX = (-v * mD * dT);
        dX += (newvalue - mX) * mC * dT + v;

        mV = dX;
        mX += dX * dT;

        return dX;
    }

  private:
    float mX;       // offset 0x0, size 0x4
    float mV;       // offset 0x4, size 0x4
    const float mC; // offset 0x8, size 0x4
    const float mD; // offset 0xC, size 0x4
};

float AngleTo(const UMath::Vector3 &p0, const UMath::Vector3 &n0, const UMath::Vector3 &p1);
float TimeToImpactXZ(const UMath::Vector3 &pos0, const UMath::Vector3 &vel0, const float rad0, const UMath::Vector3 &pos1, const UMath::Vector3 &vel1,
                     const float rad1);
void PredictPosition(float predictTime, const UMath::Vector3 &position, const UMath::Vector3 &vfwd, float yaw, const UMath::Vector3 &linearVelocity,
                     const float angularVelocity, UMath::Vector3 &result);

inline void PredictPosition(float dT, const UMath::Vector3 &position, const UMath::Matrix4 &mat, const UMath::Vector3 &linearVelocity,
                            const UMath::Vector3 &angularVelocity, UMath::Vector3 &result) {
    float yaw = UMath::Atan2r(mat.v2.x, mat.v2.z);
    PredictPosition(dT, position, UMath::Vector4To3(mat.v2), yaw, linearVelocity, angularVelocity.y, result);
}

}; // namespace Math

}; // namespace AI

#endif
