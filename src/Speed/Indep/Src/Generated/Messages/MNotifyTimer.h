#ifndef GENERATED_MESSAGES_MNOTIFYTIMER_H
#define GENERATED_MESSAGES_MNOTIFYTIMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyTimer : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyTimer);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyTimer");

        return k;
    }

    MNotifyTimer(const char *_Name) : Hermes::Message(_GetKind(), _GetSize(), 0), fName(_Name) {}

    ~MNotifyTimer() {}

    const char *GetName() const {
        return fName;
    }

    void SetName(const char *_Name) {
        fName = _Name;
    }

  private:
    const char *fName; // offset 0x10, size 0x4
};

#endif
