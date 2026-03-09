#ifndef GENERATED_MESSAGES_MNOTIFYVEHICLEDESTROYED_H
#define GENERATED_MESSAGES_MNOTIFYVEHICLEDESTROYED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyVehicleDestroyed : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyVehicleDestroyed);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyVehicleDestroyed");

        return k;
    }

    MNotifyVehicleDestroyed(HSIMABLE _Racer) : Hermes::Message(_GetKind(), _GetSize(), 0), fRacer(_Racer) {}

    ~MNotifyVehicleDestroyed() {}

    HSIMABLE GetRacer() const {
        return fRacer;
    }

    void SetRacer(HSIMABLE _Racer) {
        fRacer = _Racer;
    }

  private:
    HSIMABLE fRacer; // offset 0x10, size 0x4
};

#endif
