#ifndef AI_AIMATH_H
#define AI_AIMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace AI {

namespace Math {

// total size: 0x10
class FloatSpring {
  public:
    FloatSpring(float spring_k, float damper_k) : mX(0.0f), mV(0.0f), mC(spring_k), mD(damper_k) {}

  private:
    float mX;       // offset 0x0, size 0x4
    float mV;       // offset 0x4, size 0x4
    const float mC; // offset 0x8, size 0x4
    const float mD; // offset 0xC, size 0x4
};

}; // namespace Math

}; // namespace AI

#endif
