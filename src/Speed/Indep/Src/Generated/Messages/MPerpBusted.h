#ifndef GENERATED_MESSAGES_MPERPBUSTED_H
#define GENERATED_MESSAGES_MPERPBUSTED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MPerpBusted : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MPerpBusted);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MPerpBusted");

        return k;
    }

    MPerpBusted(HSIMABLE _Perpetrator) : Hermes::Message(_GetKind(), _GetSize(), 0), fPerpetrator(_Perpetrator) {}

    ~MPerpBusted() {}

    HSIMABLE GetPerpetrator() const {
        return fPerpetrator;
    }

    void SetPerpetrator(HSIMABLE _Perpetrator) {
        fPerpetrator = _Perpetrator;
    }

  private:
    HSIMABLE fPerpetrator; // offset 0x18, size 0x4
};

#endif
