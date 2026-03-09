#ifndef GENERATED_MESSAGES_MPURSUITOVER_H
#define GENERATED_MESSAGES_MPURSUITOVER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MPursuitOver : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MPursuitOver);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MPursuitOver");

        return k;
    }

    MPursuitOver(HSIMABLE _Perpetrator) : Hermes::Message(_GetKind(), _GetSize(), 0), fPerpetrator(_Perpetrator) {}

    ~MPursuitOver() {}

    HSIMABLE GetPerpetrator() const {
        return fPerpetrator;
    }

    void SetPerpetrator(HSIMABLE _Perpetrator) {
        fPerpetrator = _Perpetrator;
    }

  private:
    HSIMABLE fPerpetrator; // offset 0x10, size 0x4
};

#endif
