#ifndef GENERATED_MESSAGES_MSETCOPAUTOSPAWNMODE_H
#define GENERATED_MESSAGES_MSETCOPAUTOSPAWNMODE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MSetCopAutoSpawnMode : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MSetCopAutoSpawnMode);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MSetCopAutoSpawnMode");

        return k;
    }

    MSetCopAutoSpawnMode(bool _AutoSpawnMode) : Hermes::Message(_GetKind(), _GetSize(), 0), fAutoSpawnMode(_AutoSpawnMode) {}

  private:
    bool fAutoSpawnMode; // offset 0x10, size 0x1
};

#endif
