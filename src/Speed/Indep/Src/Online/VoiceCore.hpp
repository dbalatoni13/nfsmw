#ifndef ONLINE_VOICE_CORE_HPP
#define ONLINE_VOICE_CORE_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

struct VoipRefT;
struct ConnectionCore;
struct BuddyCore;
struct LobbyGames;
class NetworkCore;

struct channel_type {
    bool assigned;
    bool muted;
    char persona_name[20];
};

class VoiceCore {
  public:
    class PushToTalk {
      public:
        PushToTalk();
        void StartPolling();
        void StopPolling();
        bool IsPressed();

      private:
        bool mIsPolling;
        bool mIsPressed[2];
        ActionQueue *mActionQ[2];
    };

    static VoiceCore &instance();
    static void Construct();
    bool IsStarted();
    void Update();
    bool IsHeadsetConnected();
    bool IsHeadsetSending();
    bool IsConnected(int channel);
    bool IsConnected(const char *persona);
    bool IsActive(int channel);
    bool IsSpeaking(int channel);
    bool IsSpeaking(const char *persona);
    void SetMicStatus(bool enable);
    void SetPlaybackVolume(float volume);
    void SetSpeakerState(bool enable);
    void SetSpeakerVolume(float volume);
    void SetHeadsetPort(int port);
    bool GetSpeakerState();
    float GetSpeakerVolume();
    float GetPlaybackVolume();
    void ToggleMuted(const char *persona);
    void ClearMuteRecords();
    void ApplyMuteRecords();
    bool IsMuted(int channel);
    bool IsMuted(const char *persona);
    bool IsInVOIPChat(const char *name, int *channel);
    char *GetChannelPersona(int channel);

  private:
    VoiceCore();
    ~VoiceCore();
    void Startup();
    void Shutdown();
    void AddPlayer(int channel, const char *persona);
    int AddPlayer(const char *persona);
    void RemovePlayer(int channel);
    void RemovePlayer(const char *name);
    void RemoveAllPlayers();
    bool IsAssigned(int channel);
    void SetMuted(int channel, bool mute);
    bool IsMicOn();
    void SetMicState(bool enable);
    void _AddPlayer(int channel, const char *persona);
    int _AddPlayer(const char *persona);
    void _RemovePlayer(int channel);
    void _RemoveAllPlayers();
    bool _IsInVOIPChat(const char *name, int *channel);
    bool _IsMuted(int channel);
    bool _IsMuted(const char *persona);
    void _ToggleMuted(const char *persona);
    bool _IsSpeaking(int channel);
    bool _IsConnected(int channel);
    bool _IsHeadsetConnected();
    bool _IsHeadsetSending();
    void _SetSpeakerState(bool enable);
    void _SetSpeakerVolume(float volume);
    void _SetPlaybackVolume(float volume);

    static int num_channels;
    PushToTalk mPTT;
    bool mTransmit;
    channel_type channels[4];
    channel_type mute_record[4];
    VoipRefT *VoipRef;
    bool headset_state_at_last_shutdown;
    bool headset_ready_remember;
    bool speaker_state_remember;
    float speaker_volume_remember;
    float playback_volume_remember;
    bool all_muted;
    Timer startup_timer;
    int last_remote_status[4];
    static VoiceCore *mInstance;

    friend struct ConnectionCore;
    friend struct BuddyCore;
    friend struct LobbyGames;
    friend class NetworkCore;
};

#endif
