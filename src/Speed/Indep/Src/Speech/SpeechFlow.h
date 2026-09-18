#ifndef SPEECH_SPEECHFLOW_H
#define SPEECH_SPEECHFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class EAXCop;

namespace Speech {

// total size: 0x10
// Decl: 30
class SpeechFlow {
  public:
    // Decl: 22
    enum State {
        kTransition = -1,
        kCullCheck = 0,
    };

    // Decl: 32
    SpeechFlow();

    virtual ~SpeechFlow();

    virtual void Update() = 0;
    virtual bool IsTransitionable() = 0;
    virtual void ChangeStateTo(int new_state);
    virtual int GetState() { return mState; }
    virtual void Reset() {
        ChangeStateTo(kTransition);
        mBusy = 0;
    }
    virtual bool IsBusy() { return mBusy != 0; }
    virtual void OnCopAdded(EAXCop *cop);
    virtual void OnCopRemoved(EAXCop *cop);

    int mState;     // offset 0x0, size 0x4
    int mLastState; // offset 0x4, size 0x4
    int mBusy;      // offset 0x8, size 0x4
};

}; // namespace Speech

#endif
