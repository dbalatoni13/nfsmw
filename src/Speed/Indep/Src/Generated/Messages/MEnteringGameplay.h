#ifndef GENERATED_MESSAGES_MENTERINGGAMEPLAY_H
#define GENERATED_MESSAGES_MENTERINGGAMEPLAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MEnteringGameplay : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MEnteringGameplay);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MEnteringGameplay");

        return k;
    }

    MEnteringGameplay() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MEnteringGameplay() {}
};

#endif
