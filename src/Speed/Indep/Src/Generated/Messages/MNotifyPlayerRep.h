#ifndef GENERATED_MESSAGES_MNOTIFYPLAYERREP_H
#define GENERATED_MESSAGES_MNOTIFYPLAYERREP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x18
class MNotifyPlayerRep : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyPlayerRep);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyPlayerRep");

        return k;
    }

    MNotifyPlayerRep(HSIMABLE _Racer, int _RepPoints) : Hermes::Message(_GetKind(), _GetSize(), 0), fRacer(_Racer), fRepPoints(_RepPoints) {}

    ~MNotifyPlayerRep() {}

    HSIMABLE GetRacer() const {
        return fRacer;
    }

    void SetRacer(HSIMABLE _Racer) {
        fRacer = _Racer;
    }

    int GetRepPoints() const {
        return fRepPoints;
    }

    void SetRepPoints(int _RepPoints) {
        fRepPoints = _RepPoints;
    }

  private:
    HSIMABLE fRacer; // offset 0x10, size 0x4
    int fRepPoints;  // offset 0x14, size 0x4
};

#endif
