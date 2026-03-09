#ifndef GENERATED_MESSAGES_MENTERRACEOVERFLOW_H
#define GENERATED_MESSAGES_MENTERRACEOVERFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MEnterRaceOverFlow : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MEnterRaceOverFlow);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MEnterRaceOverFlow");

        return k;
    }

    MEnterRaceOverFlow() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MEnterRaceOverFlow() {}
};

#endif
