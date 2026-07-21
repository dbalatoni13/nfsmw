#include "VoiceCore.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"

VoiceCore *VoiceCore::mInstance;

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

void VoiceCore::PushToTalk::StopPolling() {
    if (mIsPolling) {
        for (int i = 0; i < 2; i++) {
            delete mActionQ[i];
        }
        mIsPolling = false;
    }
}

bool VoiceCore::PushToTalk::IsPressed() {
    bool ispressed = false;
    StartPolling();
    for (int port = 0; port < 2; port++) {
        while (!mActionQ[port]->IsEmpty()) {
            ActionRef aRef = mActionQ[port]->GetAction();
            if (aRef.ID() == 0x34) {
                mIsPressed[port] = aRef.Data() > 0.5f;
            }
            mActionQ[port]->PopAction();
        }
        if (mIsPressed[port]) {
            ispressed = true;
        }
    }
    return ispressed;
}

VoiceCore::VoiceCore() {
    headset_state_at_last_shutdown = true;
    speaker_volume_remember = 0.7f;
    playback_volume_remember = 0.7f;
    mTransmit = false;
    VoipRef = nullptr;
    headset_ready_remember = false;
    speaker_state_remember = false;
    all_muted = false;
    startup_timer.UnSet();
}

inline VoiceCore::~VoiceCore() {}

void VoiceCore::Construct() { mInstance = new ("VoiceCore", 0) VoiceCore; }
