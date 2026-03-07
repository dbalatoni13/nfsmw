#ifndef GENERATED_MESSAGES_MPURSUITBREAKER_H
#define GENERATED_MESSAGES_MPURSUITBREAKER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MPursuitBreaker : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MPursuitBreaker);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MPursuitBreaker");

        return k;
    }

    MPursuitBreaker(bool _StartBreaker) : Hermes::Message(_GetKind(), _GetSize(), 0), fStartBreaker(_StartBreaker) {}

    ~MPursuitBreaker() {}

    bool GetStartBreaker() const {
        return fStartBreaker;
    }

    void SetStartBreaker(bool _StartBreaker) {
        fStartBreaker = _StartBreaker;
    }

  private:
    bool fStartBreaker; // offset 0x10, size 0x1
};

#endif
