#ifndef SPEECH_OBSERVER_H
#define SPEECH_OBSERVER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SpeechFlow.h"

namespace Speech {

class Observer : public SpeechFlow {
  public:
    Observer();
    virtual ~Observer();
    virtual void Update();
    virtual bool IsTransitionable();
    virtual void Reset();
    virtual void Observe(int currobsrvation, int speaker, float score);
    virtual bool WeatherExists();
};

} // namespace Speech


#endif
