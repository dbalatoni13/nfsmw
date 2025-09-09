#ifndef AI_AIMATH_H
#define AI_AIMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace AI {

namespace Math {

struct FloatSpring {
    // total size: 0x10
    float mX;       // offset 0x0, size 0x4
    float mV;       // offset 0x4, size 0x4
    const float mC; // offset 0x8, size 0x4
    const float mD; // offset 0xC, size 0x4
};

}; // namespace Math

}; // namespace AI

#endif
