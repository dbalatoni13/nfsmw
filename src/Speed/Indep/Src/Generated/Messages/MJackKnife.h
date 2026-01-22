#ifndef GENERATED_MESSAGES_MJACKKNIFE_H
#define GENERATED_MESSAGES_MJACKKNIFE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MJackKnife : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MJackKnife);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MJackKnife");

        return k;
    }

    MJackKnife() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MJackKnife() {}
};

#endif
