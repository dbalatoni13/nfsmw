#ifndef GENERATED_MESSAGES_MNOTIFYRACEABANDONED_H
#define GENERATED_MESSAGES_MNOTIFYRACEABANDONED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MNotifyRaceAbandoned : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyRaceAbandoned);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyRaceAbandoned");

        return k;
    }

    MNotifyRaceAbandoned() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MNotifyRaceAbandoned() {}
};

#endif
