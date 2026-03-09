#ifndef GENERATED_MESSAGES_MDECLINEENTERCAREEREVENT_H
#define GENERATED_MESSAGES_MDECLINEENTERCAREEREVENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MDeclineEnterCareerEvent : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MDeclineEnterCareerEvent);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MDeclineEnterCareerEvent");

        return k;
    }

    MDeclineEnterCareerEvent() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MDeclineEnterCareerEvent() {}
};

#endif
