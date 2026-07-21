#include "VoiceCore.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

extern "C" {
VoipRefT *VoipStartup(int maxPeers);
void VoipShutdown(VoipRefT *voip);
void VoipSetLocalUser(VoipRefT *voip, const char *name);
}

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

void VoiceCore::Startup() {
    VoipRef = VoipStartup(4);
    if (!VoipRef) {
        return;
    }
    bMemSet(mute_record, 0, sizeof(mute_record));
    bMemSet(channels, 0, sizeof(channels));
    headset_ready_remember = false;
    all_muted = false;
    startup_timer = RealTimer;
    VoipSetLocalUser(VoipRef, FEDatabase->OnlineSettings.GetLobbyPersona());
    _SetPlaybackVolume(playback_volume_remember);
    SetMicState(false);
}

void VoiceCore::Shutdown() {
    if (VoipRef) {
        headset_state_at_last_shutdown = _IsHeadsetConnected();
        _RemoveAllPlayers();
        VoipShutdown(VoipRef);
        mPTT.StopPolling();
        VoipRef = nullptr;
    }
}

inline void VoiceCore::AddPlayer(int channel, const char *persona) {
    _AddPlayer(channel, persona);
}

int VoiceCore::AddPlayer(const char *persona) { return _AddPlayer(persona); }

inline void VoiceCore::RemovePlayer(int channel) { _RemovePlayer(channel); }

void VoiceCore::RemovePlayer(const char *name) {
    int channel;
    if (_IsInVOIPChat(name, &channel)) {
        _RemovePlayer(channel);
    }
}
