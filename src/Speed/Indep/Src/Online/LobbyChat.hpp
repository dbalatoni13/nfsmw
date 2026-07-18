#ifndef ONLINE_LOBBY_CHAT_HPP
#define ONLINE_LOBBY_CHAT_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

struct LobbyApiMsgT;
struct LobbyApiPlayT;
struct LobbyApiUserSetT;
typedef void (*CommandCBFunc)(LobbyApiMsgT *, void *);

namespace LobbyChatN {
enum CBReason {
    REASON_INVALID = -1,
    REASON_NEW_INVITE = 0,
    REASON_CANCELLED_INVITE = 1,
    REASON_ACCEPTED_INVITE = 2,
    REASON_DECLINED_INVITE = 3,
    REASON_DECLINED_INVITE_AND_BLOCKED = 4,
    REASON_DECLINE_INVITE_LIST_FULL = 5,
    REASON_DECLINE_PLAYER_IN_GAME = 6,
    REASON_INVITE_TIMEOUT = 7
};

enum InviteResponse {
    RESPONSE_ACCEPT = 0,
    RESPONSE_DECLINE = 1,
    RESPONSE_DECLINE_BLOCK = 2,
    RESPONSE_DECLINE_INVITE_LIST_FULL = 3,
    RESPONSE_DECLINE_PLAYER_IN_GAME = 4,
    NUM_RESPONSES = 5
};

enum InviteError {
    IERR_NOT_CONNECTED = -1,
    IERR_OUT_OF_MEMORY = -2,
    IERR_NOT_IN_GAME = -11,
    IERR_CANT_SELF_INVITE = -12,
    IERR_PLAYER_IN_GAME = -13,
    IERR_ALREADY_PENDING = -14,
    IERR_CANT_FIND_INVITE = -15,
    IERR_TOO_MANY_INVITES = -16,
    IERR_INVALID_PARAM = -20
};
}

typedef void (*LobbyChatCBFunc)(void *, char *, bool, bool, bool);
typedef void (*InviteCBFunc)(const char *, int, LobbyChatN::CBReason, void *);

struct Invite : bTNode<Invite> {
    Invite(const char *playerName, LobbyApiPlayT &game, LobbyApiUserSetT &session, float expireInSeconds);
    Invite(const char *playerName, const char *gameDetails, float expireInSeconds);
    ~Invite();

    int gameIdent;
    char player[20];
    char *gameSettings;
    Timer expireTime;
};

struct LobbyChat {
    LobbyChat()
        : userChatCB(nullptr),      //
          pChatWindow(nullptr),     //
          inviteCB(nullptr),        //
          inviteCBContext(nullptr) {}
    ~LobbyChat() { Reset(); }

    static LobbyChat &Instance();
    int32 SendChatMessage(const char *text, const char *toPersona, CommandCBFunc callback, void *context);
    void SetChatCallback(void *chatWindow, LobbyChatCBFunc callback);
    void SetInviteCallback(InviteCBFunc callback, void *context);
    int32 SendGameInvite(const char *toPlayer);
    int32 CancelInvite(const char *toPlayer);
    int32 CancelAllInvites();
    int32 RespondToInvite(const char *fromPlayer, int gameIdent, LobbyChatN::InviteResponse response);

  private:
    int32 Init();
    void Reset();
    int32 SendCancelInvite(Invite *node);
    int32 CancelAllInvites_HaveMutex();
    int32 RespondToInvite_HaveMutex(const char *fromPlayer, int gameIdent, LobbyChatN::InviteResponse response);
    static int InviteTimeoutFunc(void *context);
    friend int32 LobbyInit();
    friend void LobbyDisconnect();

    LobbyChatCBFunc userChatCB;
    void *pChatWindow;
    bTList<Invite> sentInvites;
    bTList<Invite> receivedInvites;
    InviteCBFunc inviteCB;
    void *inviteCBContext;
    static int maxActiveInvites;
};

#endif
