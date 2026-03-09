#ifndef GENERATED_MESSAGES_MGENERIC_H
#define GENERATED_MESSAGES_MGENERIC_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MGeneric : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MGeneric);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MGeneric");

        return k;
    }

    MGeneric() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MGeneric() {}
};

#endif
