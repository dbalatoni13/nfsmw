#ifndef SPEECH_PURSUITFLOW_H
#define SPEECH_PURSUITFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SpeechFlow.h"

namespace Speech {

class PursuitFlow : public SpeechFlow {
  public:
    PursuitFlow();
    virtual ~PursuitFlow();
    virtual void Update();
    virtual bool IsTransitionable();
    virtual void Reset();
    virtual void Reacquire();
    virtual void OnCopRemoved(EAXCop *cop);
};

} // namespace Speech


#endif
