#ifndef GENERATED_MESSAGES_MNISCOMPLETE_H
#define GENERATED_MESSAGES_MNISCOMPLETE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNISComplete : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNISComplete);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNISComplete");

        return k;
    }

    MNISComplete(const char *_NISName) : Hermes::Message(_GetKind(), _GetSize(), 0), fNISName(_NISName) {}

  private:
    const char *fNISName; // offset 0x10, size 0x4
};

#endif
