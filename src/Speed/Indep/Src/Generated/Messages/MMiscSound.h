#ifndef GENERATED_MESSAGES_MMISCSOUND_H
#define GENERATED_MESSAGES_MMISCSOUND_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MMiscSound : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MMiscSound);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MMiscSound");

        return k;
    }

    MMiscSound(int _SoundID) : Hermes::Message(_GetKind(), _GetSize(), 0), fSoundID(_SoundID) {}

    ~MMiscSound() {}

    int GetSoundID() const {
        return fSoundID;
    }

    void SetSoundID(int _SoundID) {
        fSoundID = _SoundID;
    }

  private:
    int fSoundID; // offset 0x10, size 0x4
};

#endif
