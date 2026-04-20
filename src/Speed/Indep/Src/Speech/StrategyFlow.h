#ifndef SPEECH_STRATEGYFLOW_H
#define SPEECH_STRATEGYFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SpeechFlow.h"

namespace Speech {

class StrategyFlow : public SpeechFlow {
  public:
    StrategyFlow();
    virtual ~StrategyFlow();
    virtual void Update();
    virtual bool IsTransitionable();
    virtual void Reset();
};

} // namespace Speech

#endif
