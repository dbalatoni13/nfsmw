#ifndef GENERATED_MESSAGES_MSTATEENTER_H
#define GENERATED_MESSAGES_MSTATEENTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MStateEnter : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MStateEnter);
    }

    static UCrc32 _GetKind();

    MStateEnter() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MStateEnter() {}
};

#endif
