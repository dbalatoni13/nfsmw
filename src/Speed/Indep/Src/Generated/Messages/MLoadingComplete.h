#ifndef GENERATED_MESSAGES_MLOADINGCOMPLETE_H
#define GENERATED_MESSAGES_MLOADINGCOMPLETE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MLoadingComplete : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MLoadingComplete);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MLoadingComplete");

        return k;
    }

    MLoadingComplete() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MLoadingComplete() {}
};

#endif
