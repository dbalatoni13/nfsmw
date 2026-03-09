#ifndef GENERATED_MESSAGES_MNOTIFYRACETIME_H
#define GENERATED_MESSAGES_MNOTIFYRACETIME_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x1c
class MNotifyRaceTime : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyRaceTime);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyRaceTime");

        return k;
    }

    MNotifyRaceTime(float _TimeElapsed, bool _TimeIsLimited, float _TimeRemaining)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fTimeElapsed(_TimeElapsed), fTimeIsLimited(_TimeIsLimited), fTimeRemaining(_TimeRemaining) {}

    ~MNotifyRaceTime() {}

    float GetTimeElapsed() const {
        return fTimeElapsed;
    }

    void SetTimeElapsed(float _TimeElapsed) {
        fTimeElapsed = _TimeElapsed;
    }

    bool GetTimeIsLimited() const {
        return fTimeIsLimited;
    }

    void SetTimeIsLimited(bool _TimeIsLimited) {
        fTimeIsLimited = _TimeIsLimited;
    }

    float GetTimeRemaining() const {
        return fTimeRemaining;
    }

    void SetTimeRemaining(float _TimeRemaining) {
        fTimeRemaining = _TimeRemaining;
    }

  private:
    float fTimeElapsed;   // offset 0x10, size 0x4
    bool fTimeIsLimited;  // offset 0x14, size 0x1
    float fTimeRemaining; // offset 0x18, size 0x4
};

#endif
