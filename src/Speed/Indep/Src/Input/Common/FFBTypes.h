#ifndef INPUT_COMMON_FFBTYPES_H
#define INPUT_COMMON_FFBTYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace RealInput {
struct Effect;
}

struct EffectBinary {
    float Time; // offset 0x0, size 0x4
    float MaxTime; // offset 0x4, size 0x4

    // void UMath::Clear() {}

    // float Run(float milliseconds) {}
};

struct InputEffectState {
    EffectBinary CollisionNoise; // offset 0x0, size 0x8
    float On;                    // offset 0x8, size 0x4

    float Mag_right; // offset 0xC, size 0x4
    bool Enabled;    // offset 0x10, size 0x1 (4 bytes of storage)

    void Run(float ms);

    void Push(RealInput::Effect *effect);
};

#endif
