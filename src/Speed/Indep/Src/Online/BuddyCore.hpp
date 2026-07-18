#ifndef ONLINE_BUDDYCORE_HPP
#define ONLINE_BUDDYCORE_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct HLBApiRefT;
struct HLBBudT;

struct InvitedPlayers {
    bool hasAccepted;
    char name[16];
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

  public:
    static BuddyCore *instance();
    HLBApiRefT *getApiRef() { return HLBud; }
    void disconnect();
    void pause();
    int resume();
};

#endif
