#ifndef ONLINE_BUDDYCORE_HPP
#define ONLINE_BUDDYCORE_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct HLBApiRefT;
struct HLBBudT;
struct LobbyChalRefT;

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
    HLBApiRefT *getApiRef() { return HLBud; }
    void disconnect();
    void pause();
    int resume();
    void clearEAMStatusIcons();
    int getBuddyCount();
    HLBBudT *getBuddyByIndex(int index);
};

#endif
