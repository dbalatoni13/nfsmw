#ifndef GENERATED_MESSAGES_MCOUNTDOWNDONE_H
#define GENERATED_MESSAGES_MCOUNTDOWNDONE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MCountdownDone : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MCountdownDone);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MCountdownDone");

        return k;
    }

    MCountdownDone() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MCountdownDone() {}
};

#endif
