#ifndef GENERATED_MESSAGES_MNOTIFYRACETIMEEXPIRED_H
#define GENERATED_MESSAGES_MNOTIFYRACETIMEEXPIRED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MNotifyRaceTimeExpired : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyRaceTimeExpired);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyRaceTimeExpired");

        return k;
    }

    MNotifyRaceTimeExpired() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MNotifyRaceTimeExpired() {}
};

#endif
