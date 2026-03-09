#ifndef GENERATED_MESSAGES_MSTATEEXIT_H
#define GENERATED_MESSAGES_MSTATEEXIT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MStateExit : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MStateExit);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MStateExit");

        return k;
    }

    MStateExit() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MStateExit() {}
};

#endif
