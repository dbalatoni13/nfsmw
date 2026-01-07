#ifndef GENERATED_MESSAGES_MSETCOPSENABLED_H
#define GENERATED_MESSAGES_MSETCOPSENABLED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MSetCopsEnabled : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MSetCopsEnabled);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MSetCopsEnabled");

        return k;
    }

    MSetCopsEnabled(bool _CopsEnabled) : Hermes::Message(_GetKind(), _GetSize(), 0), fCopsEnabled(_CopsEnabled) {}

  private:
    bool fCopsEnabled; // offset 0x10, size 0x1
};

#endif
