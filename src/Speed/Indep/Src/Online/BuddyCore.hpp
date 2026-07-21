#ifndef ONLINE_BUDDYCORE_HPP
#define ONLINE_BUDDYCORE_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Online/LobbyChat.hpp"

struct HLBApiRefT;
struct HLBBudT;
struct LobbyChalRefT;
struct LobbyApiMsgT;
struct BuddyApiMsgT;
struct BuddyCore;

template <void (BuddyCore::*Callback)(int, int)> struct buddy_op_callback;
template <void (BuddyCore::*Callback)(BuddyApiMsgT *)> struct buddy_msg_callback;
template <void (BuddyCore::*Callback)(LobbyApiMsgT *)> struct challenge_op_callback;
template <void (BuddyCore::*Callback)(const char *)> struct buddy_print_callback;

typedef void (*BuddyConnectCallback)(int status, void *context);

enum state_type {
    disconnected = 0,
    connected = 1
};

struct InvitedPlayers {
    bool hasAccepted;
    char name[16];

    InvitedPlayers() { bMemSet(&hasAccepted, 0, sizeof(bool)); }
};

struct VOIP_ChallengeInfo {
    bool isActive;
    char hostName[192];
    char challengerName[32];
    char gameRoomName[128];

    VOIP_ChallengeInfo() { bMemSet(&isActive, 0, sizeof(bool)); }
};

struct BuddySettings {
    int gameIdent;
    int BuddyWantsToPlay;
    int IWannaPlayBuddy;
    int voipState;
    int canVoiceChat;
    int isTemporary;
    int isRealBuddy;
    int IsWannaBeMyBuddy;
    int IsIWannaBeHisBuddy;
    int isTempBuddy;
    int isInGroup;
    int buddyState;
    int isBlocked;
    int isPassive;
    int IsAvailableForChat;
    int IsSameProduct;
    int IsJoinable;
    int IsNoReplyBud;
    int unreadMsgCount;
    int totalMsgCount;
    unsigned int gameInviteFlags;
    char *gameBuddyIsPlaying;
    HLBBudT *pBuddy;
    char buddyName[32];
    char presence[256];
    char presenceExtra[256];
};

struct BuddyCore {
    InvitedPlayers m_invitedPlayers[5];
    char savedNames[10][16];

  private:
    void connectchanged(int op, int status);
    static void gameinvite(HLBApiRefT *api, HLBBudT *bud, int action, void *context);
    void debugoutput(const char *str);
    void buddyListChangedCallback(int op, int opStatus);
    void messageCallback(BuddyApiMsgT *pMsg);
    void LobbyChallengeCB(LobbyApiMsgT *pMesg);
    static void InviteCB(const char *fromPlayer, int gameIdent, LobbyChatN::CBReason reason, void *context);

    template <void (BuddyCore::*Callback)(int, int)> friend struct buddy_op_callback;
    template <void (BuddyCore::*Callback)(BuddyApiMsgT *)> friend struct buddy_msg_callback;
    template <void (BuddyCore::*Callback)(LobbyApiMsgT *)> friend struct challenge_op_callback;
    template <void (BuddyCore::*Callback)(const char *)> friend struct buddy_print_callback;

    HLBApiRefT *HLBud;
    state_type state;
    BuddyConnectCallback connectcallback;
    void *connectcontext;
    LobbyChalRefT *m_lobbyChalRefT;
    int m_lastVOIPstate;
    VOIP_ChallengeInfo m_voipChallenge[6];
    int m_realBuddys;
    int m_tempBuddys;
    int m_blockedBuddys;
    int m_friendRequestBuddys;
    bool m_InVoiceChat;
    int m_myVoiceChatChannel;
    int m_buddyVoiceChatChannel;
    bool m_networkCableUnplugged;
    bool m_paused;

  public:
    BuddyCore();
    static BuddyCore *instance();
    int startconnect(const char *name, const char *passwd, BuddyConnectCallback callback, void *context);
    HLBApiRefT *getApiRef() { return HLBud; }
    void disconnect();
    void handledisconnect();
    void doprocessing();
    void pause();
    int resume();
    void initVoiceAndPresence();
    void clearEAMStatusIcons();
    void DisplayVOIPChatEnded();
    int getBuddyCount();
    HLBBudT *getBuddyByIndex(int index);
};

template <void (BuddyCore::*Callback)(int, int)> struct buddy_op_callback {
    static void invoke(HLBApiRefT *, int op, int status, void *context) {
        (static_cast<BuddyCore *>(context)->*Callback)(op, status);
    }
};

template <void (BuddyCore::*Callback)(BuddyApiMsgT *)> struct buddy_msg_callback {
    static void invoke(HLBApiRefT *, BuddyApiMsgT *message, void *context) {
        (static_cast<BuddyCore *>(context)->*Callback)(message);
    }
};

template <void (BuddyCore::*Callback)(LobbyApiMsgT *)> struct challenge_op_callback {
    static void invoke(LobbyChalRefT *, LobbyApiMsgT *message, void *context) {
        (static_cast<BuddyCore *>(context)->*Callback)(message);
    }
};

template <void (BuddyCore::*Callback)(const char *)> struct buddy_print_callback {
    static void invoke(void *context, const char *str) {
        (static_cast<BuddyCore *>(context)->*Callback)(str);
    }
};

#endif
