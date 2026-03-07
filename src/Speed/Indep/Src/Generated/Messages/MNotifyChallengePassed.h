#ifndef GENERATED_MESSAGES_MNOTIFYCHALLENGEPASSED_H
#define GENERATED_MESSAGES_MNOTIFYCHALLENGEPASSED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyChallengePassed : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyChallengePassed);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyChallengePassed");

        return k;
    }

    MNotifyChallengePassed(const char *_ChallengeName) : Hermes::Message(_GetKind(), _GetSize(), 0), fChallengeName(_ChallengeName) {}

    ~MNotifyChallengePassed() {}

    const char *GetChallengeName() const {
        return fChallengeName;
    }

    void SetChallengeName(const char *_ChallengeName) {
        fChallengeName = _ChallengeName;
    }

  private:
    const char *fChallengeName; // offset 0x10, size 0x4
};

#endif
