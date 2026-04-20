#ifndef SPEECH_SPEECHFLOW_H
#define SPEECH_SPEECHFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class EAXCop;

namespace Speech {

class SpeechFlow {
  public:
    SpeechFlow();
    virtual ~SpeechFlow();

    virtual void Update();
    virtual bool IsTransitionable();
    virtual void ChangeStateTo(int new_state);

    virtual int GetState();
    virtual void Reset();
    virtual bool IsBusy();
    virtual void OnCopAdded(EAXCop *cop);
    virtual void OnCopRemoved(EAXCop *cop);

  protected:
    int mState;     // offset 0x0, size 0x4
    int mLastState; // offset 0x4, size 0x4
    int mBusy;      // offset 0x8, size 0x4
};

} // namespace Speech

#endif
