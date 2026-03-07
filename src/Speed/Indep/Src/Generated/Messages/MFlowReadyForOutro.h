#ifndef GENERATED_MESSAGES_MFLOWREADYFOROUTRO_H
#define GENERATED_MESSAGES_MFLOWREADYFOROUTRO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MFlowReadyForOutro : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MFlowReadyForOutro);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MFlowReadyForOutro");

        return k;
    }

    MFlowReadyForOutro() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MFlowReadyForOutro() {}
};

#endif
