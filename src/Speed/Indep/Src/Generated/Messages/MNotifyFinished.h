#ifndef GENERATED_MESSAGES_MNOTIFYFINISHED_H
#define GENERATED_MESSAGES_MNOTIFYFINISHED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyFinished : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyFinished);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyFinished");

        return k;
    }

    MNotifyFinished(GCollectionKey _Sender) : Hermes::Message(_GetKind(), _GetSize(), 0), fSender(_Sender) {}

    ~MNotifyFinished() {}

    GCollectionKey GetSender() const {
        return fSender;
    }

    void SetSender(GCollectionKey _Sender) {
        fSender = _Sender;
    }

  private:
    GCollectionKey fSender; // offset 0x10, size 0x4
};

#endif
