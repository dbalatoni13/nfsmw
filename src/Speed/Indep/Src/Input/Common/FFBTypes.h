#ifndef INPUT_COMMON_FFBTYPES_H
#define INPUT_COMMON_FFBTYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct EffectBinary {
    float Time; // offset 0x0, size 0x4
    float MaxTime; // offset 0x4, size 0x4

    // void UMath::Clear() {}

    // float Run(float milliseconds) {}
};

struct InputEffectState {
    EffectBinary CollisionNoise; // offset 0x0, size 0x8
    float On;                    // offset 0x8, size 0x4

    // void Run(float ms) {}

    // void Push(struct rw::core::controller::IDeviceEffect * effect) ;

    float Mag_right;
    bool Enabled;
};

#endif
