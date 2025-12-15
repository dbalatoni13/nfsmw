#ifndef GENERATED_MESSAGES_MNOTIFYSIMTICK_H
#define GENERATED_MESSAGES_MNOTIFYSIMTICK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x18
class MNotifySimTick : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifySimTick);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifySimTick");

        return k;
    }

    MNotifySimTick(float _SimTime, float _TimeStep) : Hermes::Message(_GetKind(), _GetSize(), 0), fSimTime(_SimTime), fTimeStep(_TimeStep) {}

    ~MNotifySimTick() {}

  private:
    float fSimTime;  // offset 0x10, size 0x4
    float fTimeStep; // offset 0x14, size 0x4
};

#endif
