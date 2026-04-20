#ifndef SPEECH_SPEECHFLOW_H
#define SPEECH_SPEECHFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class EAXCop;

namespace Speech {

class SpeechFlow {
  public:
    virtual ~SpeechFlow();
    virtual int GetState();
    virtual void ChangeStateTo(int new_state);
    virtual void Reset();
    virtual bool IsTransitionable();
    virtual void OnCopRemoved(EAXCop *cop);
    virtual void OnCopAdded(EAXCop *cop);
};

} // namespace Speech

#endif
