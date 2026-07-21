#ifndef ONLINE_BUDDYCORE_HPP
#define ONLINE_BUDDYCORE_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Online/LobbyChat.hpp"
#include "Speed/Indep/Src/Online/NetworkCore.hpp"

struct HLBApiRefT;
struct HLBBudT;
struct LobbyChalRefT;
struct LobbyApiMsgT;
struct BuddyCore;

char *GetLocalizedString(uint32 hash);
int FEngSNPrintf(char *buffer, int32 bufferSize, const char *format, ...);

enum eDialogTitle {
    dialog_none = 0,
    dialog_alert = 1,
    dialog_info = 2,
    dialog_confirmation = 3
};

enum eDialogFirstButtons {
    first_dialog_button1 = 0,
    first_dialog_button2 = 1,
    first_dialog_button3 = 2
};

struct DialogInterface {
    static void DismissDialog(int handle);
    static int ShowMessage(const char *fromPackage, const char *dialogPackage, eDialogTitle title,
                           const char *message);
    static int ShowOneButton(const char *fromPackage, const char *dialogPackage, eDialogTitle title,
                             uint32 buttonTextHash, uint32 buttonPressedMessage,
                             const char *format);
    static int ShowOneButton(const char *fromPackage, const char *dialogPackage, eDialogTitle title,
                             uint32 buttonTextHash, uint32 buttonPressedMessage, uint32 cancelMessage,
                             const char *format, ...);
    static int ShowTwoButtons(const char *fromPackage, const char *dialogPackage, eDialogTitle title,
                              uint32 button1TextHash, uint32 button2TextHash,
                              uint32 button1PressedMessage, uint32 button2PressedMessage,
                              uint32 cancelMessage, eDialogFirstButtons firstButton,
                              const char *format);
    static int ShowThreeButtons(const char *fromPackage, const char *dialogPackage,
                                eDialogTitle title, uint32 button1TextHash,
                                uint32 button2TextHash, uint32 button3TextHash,
                                uint32 button1PressedMessage, uint32 button2PressedMessage,
                                uint32 button3PressedMessage, uint32 cancelMessage,
                                eDialogFirstButtons firstButton, const char *format);
    static void SetBlurbIsUTF8();
};

struct BuddyApiMsgT {
    int iType;
    int iKind;
    int iCode;
    unsigned int uTimeStamp;
    unsigned int uUserFlags;
    char *pData;
};

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

enum HLBStatE {
    HLB_STAT_UNDEFINED = -1,
    HLB_STAT_DISC = 0,
    HLB_STAT_CHAT = 1,
    HLB_STAT_AWAY = 2,
    HLB_STAT_XA = 3,
    HLB_STAT_DND = 4,
    HLB_STAT_PASSIVE = 5
};

struct RichPresenceMsg {
    char IPaddress[32];
    uint32 eventHash;
    uint32 isRacing;
    char session[32];
    uint32 isSessionPassworded;
    char password[32];
    uint32 isSessionPrivate;

    RichPresenceMsg() {}
    inline void Default() {
        bMemSet(this, 0, sizeof(*this));
        bStrCpy(IPaddress, NetworkCore::MyIPAddressText());
    }
};

struct RichPresence {
  private:
    RichPresenceMsg mCurrentRichPresence;
    HLBStatE mBuddyState;

  public:
    RichPresence();
    ~RichPresence();
    static RichPresence *Instance();
    void Init();
    void SetEventHash(uint32 eventHash);
    void SetSession(const char *session);
    void SetBuddyState(HLBStatE buddyState);
    void SetIsRacing(bool isRacing);
    void SetSessionPasswordProtected(bool isPasswordProtected);
    void SetPassword(const char *password);
    void SetSessionPrivate(bool isPrivate);
    void UpdatePresence();
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
    void setNetworkCableUnpluggedFlag();
    void SetPresence(RichPresenceMsg richPresenceMsg, HLBStatE buddyState);
    RichPresenceMsg GetRichPresence(BuddySettings *buddySettings);
    void DisplayVOIPChatEnded();
    void DisplayDeclinedInviteInGame(const char *name);
    void DisplayDeclinedInvite(const char *name);
    void DisplayBlockedInvite(const char *name);
    void DisplayTimedOutInvite(const char *name);
    void DisplayRevokedInvite(const char *name);
    inline void DisplayBusyInvite(const char *name) {
        char sztemp[128];
        FEngSNPrintf(sztemp, sizeof(sztemp), GetLocalizedString(0x6c02f6e1), name);
        DialogInterface::ShowOneButton("", "", dialog_alert, 0x417b2601, 0xffffffff, 0xffffffff, sztemp);
    }
    LobbyChalRefT *GetLobbyChallengeApiRef();
    int GetNextAvailableVOIPSlot();
    bool DoIHaveAVOIPInviteFromThisBuddy(char *buddyName);
    VOIP_ChallengeInfo *GetVOIPChallengeInfo(char *buddyName);
    void RemoveVOIPChallenge(char *buddyName);
    void AddTempBuddy(char *name, int tempType);
    void CountBuddys();
    int GetNumRealBuddys();
    int GetNumberTempBuddys();
    int GetNumBlockedBuddys();
    int GetNumFriendRequests();
    int getBuddyCount();
    HLBBudT *getBuddyByIndex(int index);
    HLBBudT *getBuddyByName(const char *name);
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
