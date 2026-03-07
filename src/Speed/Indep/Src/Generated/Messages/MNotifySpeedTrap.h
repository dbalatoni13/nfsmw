#ifndef GENERATED_MESSAGES_MNOTIFYSPEEDTRAP_H
#define GENERATED_MESSAGES_MNOTIFYSPEEDTRAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x1c
class MNotifySpeedTrap : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifySpeedTrap);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifySpeedTrap");

        return k;
    }

    MNotifySpeedTrap(GCollectionKey _TrapActivity, HSIMABLE _Racer, float _SpeedKmh)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fTrapActivity(_TrapActivity), fRacer(_Racer), fSpeedKmh(_SpeedKmh) {}

    ~MNotifySpeedTrap() {}

    GCollectionKey GetTrapActivity() const {
        return fTrapActivity;
    }

    void SetTrapActivity(GCollectionKey _TrapActivity) {
        fTrapActivity = _TrapActivity;
    }

    HSIMABLE GetRacer() const {
        return fRacer;
    }

    void SetRacer(HSIMABLE _Racer) {
        fRacer = _Racer;
    }

    float GetSpeedKmh() const {
        return fSpeedKmh;
    }

    void SetSpeedKmh(float _SpeedKmh) {
        fSpeedKmh = _SpeedKmh;
    }

  private:
    GCollectionKey fTrapActivity; // offset 0x10, size 0x4
    HSIMABLE fRacer;              // offset 0x14, size 0x4
    float fSpeedKmh;              // offset 0x18, size 0x4
};

#endif
