#ifndef GENERATED_MESSAGES_MACCEPTENTERCAREEREVENT_H
#define GENERATED_MESSAGES_MACCEPTENTERCAREEREVENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MAcceptEnterCareerEvent : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MAcceptEnterCareerEvent);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MAcceptEnterCareerEvent");

        return k;
    }

    MAcceptEnterCareerEvent() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MAcceptEnterCareerEvent() {}
};

#endif
