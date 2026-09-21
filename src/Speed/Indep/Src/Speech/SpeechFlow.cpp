#include "Speed/Indep/Src/Speech/SpeechFlow.h"

namespace Speech {

SpeechFlow::SpeechFlow()
    : mState(0),      //
      mLastState(-1), //
      mBusy(0) {}

SpeechFlow::~SpeechFlow() {}

void SpeechFlow::ChangeStateTo(int new_state) {
    if (new_state != this->mState) {
        this->mLastState = this->mState;
    }
    this->mState = new_state;
}

void SpeechFlow::OnCopAdded(EAXCop *cop) {}

void SpeechFlow::OnCopRemoved(EAXCop *cop) {}

}; // namespace Speech
