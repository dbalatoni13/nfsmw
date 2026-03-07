#ifndef GENERATED_MESSAGES_MNOTIFYMUSICFLOW_H
#define GENERATED_MESSAGES_MNOTIFYMUSICFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyMusicFlow : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyMusicFlow);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyMusicFlow");

        return k;
    }

    MNotifyMusicFlow(int _Part) : Hermes::Message(_GetKind(), _GetSize(), 0), fPart(_Part) {}

    ~MNotifyMusicFlow() {}

    int GetPart() const {
        return fPart;
    }

    void SetPart(int _Part) {
        fPart = _Part;
    }

  private:
    int fPart; // offset 0x10, size 0x4
};

#endif
