#ifndef GENERATED_MESSAGES_MUNSPAWNCOP_H
#define GENERATED_MESSAGES_MUNSPAWNCOP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x18
class MUnspawnCop : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MUnspawnCop);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MUnspawnCop");

        return k;
    }

    MUnspawnCop(HSIMABLE _CopHandle, int _Param) : Hermes::Message(_GetKind(), _GetSize(), 0), fCopHandle(_CopHandle), fParam(_Param) {}

  private:
    HSIMABLE fCopHandle; // offset 0x10, size 0x4
    int fParam;          // offset 0x14, size 0x4
};

#endif
