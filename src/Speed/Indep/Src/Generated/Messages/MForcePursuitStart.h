#ifndef GENERATED_MESSAGES_MFORCEPURSUITSTART_H
#define GENERATED_MESSAGES_MFORCEPURSUITSTART_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MForcePursuitStart : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MForcePursuitStart);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MForcePursuitStart");

        return k;
    }

    MForcePursuitStart(int _MinHeatLevel) : Hermes::Message(_GetKind(), _GetSize(), 0), fMinHeatLevel(_MinHeatLevel) {}

    int GetMinHeatLevel() const {
        return fMinHeatLevel;
    }

  private:
    int fMinHeatLevel; // offset 0x10, size 0x4
};

#endif
