#include "Speed/Indep/Src/Speech/SpeechFlow.h"

namespace Speech {

SpeechFlow::SpeechFlow()
    : mState(0), //
      mLastState(-1), //
      mBusy(0) {}

SpeechFlow::~SpeechFlow() {}

void SpeechFlow::ChangeStateTo(int new_state) {
    if (new_state != mState) {
        mLastState = mState;
    }
    mState = new_state;
}

int SpeechFlow::GetState() {
    return mState;
}

bool SpeechFlow::IsBusy() {
    return mBusy != 0;
}

void SpeechFlow::OnCopAdded(EAXCop *) {}

void SpeechFlow::OnCopRemoved(EAXCop *) {}

} // namespace Speech
