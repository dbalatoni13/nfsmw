#ifndef GENERATED_MESSAGES_MNOTIFYRACEPLACEMENT_H
#define GENERATED_MESSAGES_MNOTIFYRACEPLACEMENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x1c
class MNotifyRacePlacement : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyRacePlacement);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyRacePlacement");

        return k;
    }

    MNotifyRacePlacement(GCollectionKey _Activity, HSIMABLE _Racer, int _Placing)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fActivity(_Activity), fRacer(_Racer), fPlacing(_Placing) {}

    ~MNotifyRacePlacement() {}

    GCollectionKey GetActivity() const {
        return fActivity;
    }

    void SetActivity(GCollectionKey _Activity) {
        fActivity = _Activity;
    }

    HSIMABLE GetRacer() const {
        return fRacer;
    }

    void SetRacer(HSIMABLE _Racer) {
        fRacer = _Racer;
    }

    int GetPlacing() const {
        return fPlacing;
    }

    void SetPlacing(int _Placing) {
        fPlacing = _Placing;
    }

  private:
    GCollectionKey fActivity; // offset 0x10, size 0x4
    HSIMABLE fRacer;          // offset 0x14, size 0x4
    int fPlacing;             // offset 0x18, size 0x4
};

#endif
