#ifndef GENERATED_MESSAGES_MREQROADBLOCK_H
#define GENERATED_MESSAGES_MREQROADBLOCK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MReqRoadBlock : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MReqRoadBlock);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MReqRoadBlock");

        return k;
    }

    MReqRoadBlock(int _Data) : Hermes::Message(_GetKind(), _GetSize(), 0), fData(_Data) {}

  private:
    int fData; // offset 0x10, size 0x4
};

#endif
