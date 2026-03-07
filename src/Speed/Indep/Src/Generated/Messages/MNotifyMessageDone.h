#ifndef GENERATED_MESSAGES_MNOTIFYMESSAGEDONE_H
#define GENERATED_MESSAGES_MNOTIFYMESSAGEDONE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MNotifyMessageDone : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyMessageDone);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyMessageDone");

        return k;
    }

    MNotifyMessageDone() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MNotifyMessageDone() {}
};

#endif
