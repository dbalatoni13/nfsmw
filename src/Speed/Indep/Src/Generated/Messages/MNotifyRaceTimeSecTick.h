#ifndef GENERATED_MESSAGES_MNOTIFYRACETIMESECTICK_H
#define GENERATED_MESSAGES_MNOTIFYRACETIMESECTICK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyRaceTimeSecTick : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyRaceTimeSecTick);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyRaceTimeSecTick");

        return k;
    }

    MNotifyRaceTimeSecTick(float _TimeElapsed) : Hermes::Message(_GetKind(), _GetSize(), 0), fTimeElapsed(_TimeElapsed) {}

    ~MNotifyRaceTimeSecTick() {}

    float GetTimeElapsed() const {
        return fTimeElapsed;
    }

    void SetTimeElapsed(float _TimeElapsed) {
        fTimeElapsed = _TimeElapsed;
    }

  private:
    float fTimeElapsed; // offset 0x10, size 0x4
};

#endif
