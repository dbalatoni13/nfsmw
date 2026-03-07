#ifndef GENERATED_MESSAGES_MJUMPCUT_H
#define GENERATED_MESSAGES_MJUMPCUT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MJumpCut : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MJumpCut);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MJumpCut");

        return k;
    }

    MJumpCut(unsigned int _AnchorWorldID) : Hermes::Message(_GetKind(), _GetSize(), 0), fAnchorWorldID(_AnchorWorldID) {}

    ~MJumpCut() {}

    unsigned int GetAnchorWorldID() const {
        return fAnchorWorldID;
    }

    void SetAnchorWorldID(unsigned int _AnchorWorldID) {
        fAnchorWorldID = _AnchorWorldID;
    }

  private:
    unsigned int fAnchorWorldID; // offset 0x10, size 0x4
};

#endif
