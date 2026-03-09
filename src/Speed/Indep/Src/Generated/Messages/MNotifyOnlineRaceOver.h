#ifndef GENERATED_MESSAGES_MNOTIFYONLINERACEOVER_H
#define GENERATED_MESSAGES_MNOTIFYONLINERACEOVER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyOnlineRaceOver : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyOnlineRaceOver);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyOnlineRaceOver");

        return k;
    }

    MNotifyOnlineRaceOver(bool _ShowPostRace) : Hermes::Message(_GetKind(), _GetSize(), 0), fShowPostRace(_ShowPostRace) {}

    ~MNotifyOnlineRaceOver() {}

    bool GetShowPostRace() const {
        return fShowPostRace;
    }

    void SetShowPostRace(bool _ShowPostRace) {
        fShowPostRace = _ShowPostRace;
    }

  private:
    bool fShowPostRace; // offset 0x10, size 0x1
};

#endif
