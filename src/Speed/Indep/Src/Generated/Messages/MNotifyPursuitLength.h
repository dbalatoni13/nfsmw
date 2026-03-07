#ifndef GENERATED_MESSAGES_MNOTIFYPURSUITLENGTH_H
#define GENERATED_MESSAGES_MNOTIFYPURSUITLENGTH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x18
class MNotifyPursuitLength : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyPursuitLength);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyPursuitLength");

        return k;
    }

    MNotifyPursuitLength(HSIMABLE _Perpetrator, float _PursuitLength)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fPerpetrator(_Perpetrator), fPursuitLength(_PursuitLength) {}

    ~MNotifyPursuitLength() {}

    HSIMABLE GetPerpetrator() const {
        return fPerpetrator;
    }

    void SetPerpetrator(HSIMABLE _Perpetrator) {
        fPerpetrator = _Perpetrator;
    }

    float GetPursuitLength() const {
        return fPursuitLength;
    }

    void SetPursuitLength(float _PursuitLength) {
        fPursuitLength = _PursuitLength;
    }

  private:
    HSIMABLE fPerpetrator; // offset 0x10, size 0x4
    float fPursuitLength;  // offset 0x14, size 0x4
};

#endif
