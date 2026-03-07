#ifndef GENERATED_MESSAGES_MBREAKERSTOPCOPS_H
#define GENERATED_MESSAGES_MBREAKERSTOPCOPS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x24
class MBreakerStopCops : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MBreakerStopCops);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MBreakerStopCops");

        return k;
    }

    MBreakerStopCops(UMath::Vector3 _InitialPos, float _Duration, float _Radius)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fInitialPos(_InitialPos), fDuration(_Duration), fRadius(_Radius) {}

    ~MBreakerStopCops() {}

    UMath::Vector3 GetInitialPos() const {
        return fInitialPos;
    }

    void SetInitialPos(UMath::Vector3 _InitialPos) {
        fInitialPos = _InitialPos;
    }

    float GetDuration() const {
        return fDuration;
    }

    void SetDuration(float _Duration) {
        fDuration = _Duration;
    }

    float GetRadius() const {
        return fRadius;
    }

    void SetRadius(float _Radius) {
        fRadius = _Radius;
    }

  private:
    UMath::Vector3 fInitialPos; // offset 0x10, size 0xc
    float fDuration;            // offset 0x1c, size 0x4
    float fRadius;              // offset 0x20, size 0x4
};

#endif
