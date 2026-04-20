#ifndef SPEECH_MUSICFLOW_H
#define SPEECH_MUSICFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SpeechFlow.h"

namespace Speech {

class MusicFlow : public SpeechFlow {
  public:
    MusicFlow();
    virtual ~MusicFlow();
    virtual void Update();
    virtual bool IsTransitionable();
    virtual void Reset();
    virtual void Reacquire();
};

} // namespace Speech

#endif
