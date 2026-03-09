#ifndef GENERATED_MESSAGES_MNOTIFYCELLCALLSTARTED_H
#define GENERATED_MESSAGES_MNOTIFYCELLCALLSTARTED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MNotifyCellCallStarted : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyCellCallStarted);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyCellCallStarted");

        return k;
    }

    MNotifyCellCallStarted() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MNotifyCellCallStarted() {}
};

#endif
