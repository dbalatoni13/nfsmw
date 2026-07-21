#include "VoiceCore.hpp"

VoiceCore::PushToTalk::PushToTalk() { mIsPolling = false; }

void VoiceCore::PushToTalk::StartPolling() {
    if (!mIsPolling) {
        for (int i = 0; i < 2; i++) {
            mIsPressed[i] = false;
            mActionQ[i] = new ActionQueue(i, 0x6689019e, "VOIP", false);
            mActionQ[i]->Enable(true);
        }
        mIsPolling = true;
    }
}
