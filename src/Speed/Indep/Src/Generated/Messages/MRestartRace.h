#ifndef GENERATED_MESSAGES_MRESTARTRACE_H
#define GENERATED_MESSAGES_MRESTARTRACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MRestartRace : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MRestartRace);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MRestartRace");

        return k;
    }

    MRestartRace() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MRestartRace() {}
};

#endif
