#ifndef GENERATED_MESSAGES_MNOTIFYMOVIEFINISHED_H
#define GENERATED_MESSAGES_MNOTIFYMOVIEFINISHED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MNotifyMovieFinished : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyMovieFinished);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyMovieFinished");

        return k;
    }

    MNotifyMovieFinished() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MNotifyMovieFinished() {}
};

#endif
