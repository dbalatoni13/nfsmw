#ifndef INPUT_COMMON_FFBTYPES_H
#define INPUT_COMMON_FFBTYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace RealInput {
class Effect;
}

struct EffectBinary {
    float Time; // offset 0x0, size 0x4
    float MaxTime; // offset 0x4, size 0x4

    void Clear() {
        this->MaxTime = 0.0f;
        this->Time = 0.0f;
    }

    float Run(float milliseconds) {
        if (this->Time > 0.0f && this->MaxTime > 0.0f) {
            this->Time -= milliseconds * 0.001f;
            return 1.0f;
        }
        return 0.0f;
    }
};

struct InputEffectState {
    EffectBinary CollisionNoise; // offset 0x0, size 0x8
    float On;                    // offset 0x8, size 0x4
    float Mag_right;             // offset 0xC, size 0x4
    bool Enabled;                // offset 0x10, size 0x1

    void Run(float ms);
    void Push(struct RealInput::Effect *effect);
};

#endif
