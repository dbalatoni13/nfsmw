#ifndef GENERATED_MESSAGES_MENTERPOSTRACEFLOW_H
#define GENERATED_MESSAGES_MENTERPOSTRACEFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MEnterPostRaceFlow : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MEnterPostRaceFlow);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MEnterPostRaceFlow");

        return k;
    }

    MEnterPostRaceFlow() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MEnterPostRaceFlow() {}
};

#endif
