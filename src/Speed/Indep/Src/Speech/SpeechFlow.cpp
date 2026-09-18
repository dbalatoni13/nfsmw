#include "SpeechFlow.h"

namespace Speech {

SpeechFlow::SpeechFlow() : mState(kCullCheck), mLastState(kTransition), mBusy(0) {}

void SpeechFlow::ChangeStateTo(int new_state) {
    if (new_state != mState) {
        mLastState = mState;
    }
    mState = new_state;
}

SpeechFlow::~SpeechFlow() {}

void SpeechFlow::OnCopRemoved(EAXCop *cop) {}

void SpeechFlow::OnCopAdded(EAXCop *cop) {}

}; // namespace Speech
