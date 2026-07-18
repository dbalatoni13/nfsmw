#include "LobbyChat.hpp"
#include "BuddyCore.hpp"

extern "C" {
void TagFieldSetFlags(char *buffer, int bufferSize, const char *name, int flags);
uint32 TagFieldGetFlags(const char *field, uint32 defaultValue);
BuddySettings EA_Messenger_GetBuddySettingsByName(const char *name);
}

char *GetLocalizedString(uint32 hash);

int LobbyChat::maxActiveInvites = 5;

LobbyChat &LobbyChat::Instance() {
    static LobbyChat theLobbyChat;
    return theLobbyChat;
}

int32 LobbyChat::SendChatMessage(const char *text, const char *toPersona, CommandCBFunc callback, void *context) {
    lobbyMutex.Lock("LobbyChat::SendChatMessage");
    if (bStrLen(text) == 0) {
        lobbyMutex.Unlock("LobbyChat::SendChatMessage");
        return -1;
    }

    char data[512] = "";
    TagFieldSetString(data, sizeof(data), "TEXT", text);
    if (toPersona) {
        TagFieldSetString(data, sizeof(data), "PRIV", toPersona);
    } else {
        TagFieldSetFlags(data, sizeof(data), "ATTR", 0x80);
    }
    int32 rc = LobbyCore::Instance().QueueCommand('mesg', data, LobbyCore::DefaultCB, nullptr, callback, context, false);
    lobbyMutex.Unlock("LobbyChat::SendChatMessage");
    return rc;
}

void LobbyChat::SetChatCallback(void *chatWindow, LobbyChatCBFunc callback) {
    pChatWindow = chatWindow;
    userChatCB = callback;
}

void LobbyChat::SetInviteCallback(InviteCBFunc callback, void *context) {
    inviteCB = callback;
    inviteCBContext = context;
}

int32 LobbyChat::SendGameInvite(const char *toPlayer) {
    lobbyMutex.Lock("LobbyChat::SendGameInvite");

    LobbyApiPlayT *myGame = LobbyGames::Instance().GetMyGame();
    GameSession *mySession = LobbyGameSessions::Instance().GetMySession();
    char *myPersona = FEDatabase->OnlineSettings.GetLobbyPersona();
    if (!toPlayer || !*toPlayer) {
        lobbyMutex.Unlock("LobbyChat::SendGameInvite");
        return LobbyChatN::IERR_INVALID_PARAM;
    }
    if (bStrICmp(myPersona, toPlayer) == 0) {
        lobbyMutex.Unlock("LobbyChat::SendGameInvite");
        return LobbyChatN::IERR_CANT_SELF_INVITE;
    }
    if (myGame) {
        for (int i = 0; i < myGame->iCount; i++) {
            if (bStrICmp(toPlayer, myGame->aOpponents[i].strPers) == 0) {
                lobbyMutex.Unlock("LobbyChat::SendGameInvite");
                return LobbyChatN::IERR_PLAYER_IN_GAME;
            }
        }
    }
    for (Invite *node = sentInvites.GetHead(); node != sentInvites.EndOfList(); node = node->GetNext()) {
        if (bStrICmp(node->player, toPlayer) == 0) {
            lobbyMutex.Unlock("LobbyChat::SendGameInvite");
            return LobbyChatN::IERR_ALREADY_PENDING;
        }
    }
    if (sentInvites.CountElements() == maxActiveInvites) {
        lobbyMutex.Unlock("LobbyChat::SendGameInvite");
        return LobbyChatN::IERR_TOO_MANY_INVITES;
    }

    bool firstElement = sentInvites.IsEmpty() && receivedInvites.IsEmpty();
    Invite *node = new ("LobbyChat::SendInvite", 0) Invite(toPlayer, *myGame, *mySession, 120.0f);
    if (node) {
        sentInvites.AddTail(node);
        if (!firstElement || NetworkCore::Instance().RegisterProcessingFunc(InviteTimeoutFunc, this) != -1) {
            char buf[512] = "";
            TagFieldSetString(buf, sizeof(buf), "TEXT", node->gameSettings);
            TagFieldSetString(buf, sizeof(buf), "PRIV", toPlayer);
            TagFieldSetFlags(buf, sizeof(buf), "ATTR", 0x30020);
            int32 rc = LobbyCore::Instance().QueueCommand('mesg', buf, LobbyCore::DefaultCB, nullptr, nullptr, nullptr, false);
            lobbyMutex.Unlock("LobbyChat::SendGameInvite");
            return rc;
        }
        delete sentInvites.Remove(node);
    }
    lobbyMutex.Unlock("LobbyChat::SendGameInvite");
    return LobbyChatN::IERR_OUT_OF_MEMORY;
}

int32 LobbyChat::CancelInvite(const char *toPlayer) {
    if (!toPlayer || !*toPlayer) {
        return LobbyChatN::IERR_INVALID_PARAM;
    }

    lobbyMutex.Lock("LobbyChat::CancelInvite");
    for (Invite *node = sentInvites.GetHead(); node != sentInvites.EndOfList(); node = node->GetNext()) {
        if (bStrICmp(toPlayer, node->player) == 0) {
            sentInvites.Remove(node);
            int32 rc = SendCancelInvite(node);
            delete node;
            lobbyMutex.Unlock("LobbyChat::CancelInvite");
            return rc;
        }
    }
    lobbyMutex.Unlock("LobbyChat::CancelInvite");
    return 0;
}

int32 LobbyChat::CancelAllInvites() {
    lobbyMutex.Lock("LobbyChat::CancelAllInvites");
    int32 rc = CancelAllInvites_HaveMutex();
    lobbyMutex.Unlock("LobbyChat::CancelAllInvites");
    return rc;
}

int32 LobbyChat::RespondToInvite(const char *fromPlayer, int gameIdent, LobbyChatN::InviteResponse response) {
    lobbyMutex.Lock("LobbyChat::RespondToInvite");
    int32 rc = RespondToInvite_HaveMutex(fromPlayer, gameIdent, response);
    lobbyMutex.Unlock("LobbyChat::RespondToInvite");
    return rc;
}

Invite *LobbyChat::GetReceivedInvite(const char *fromPlayer, int gameIdent) {
    lobbyMutex.Lock("LobbyChat::GetReceivedInvite");
    Invite *node;
    for (node = receivedInvites.GetHead(); node != receivedInvites.EndOfList(); node = node->GetNext()) {
        if (gameIdent == node->gameIdent && bStrCmp(fromPlayer, node->player) == 0) {
            break;
        }
    }
    Invite *rc = nullptr;
    if (node != receivedInvites.EndOfList()) {
        rc = node;
    }
    lobbyMutex.Unlock("LobbyChat::GetReceivedInvite");
    return rc;
}

Invite *LobbyChat::GetReceivedInvite(uint32 index) {
    lobbyMutex.Lock("LobbyChat::GetReceivedInvite");
    uint32 i = 0;
    Invite *node = receivedInvites.GetHead();
    while (node != receivedInvites.EndOfList() && i != index) {
        node = node->GetNext();
        i++;
    }
    Invite *rc = node;
    if (node == receivedInvites.EndOfList()) {
        rc = nullptr;
    }
    lobbyMutex.Unlock("LobbyChat::GetReceivedInvite");
    return rc;
}

Invite *LobbyChat::GetSentInvite(const char *toPlayer) {
    lobbyMutex.Lock("LobbyChat::GetSentInvite");
    Invite *node;
    for (node = sentInvites.GetHead(); node != sentInvites.EndOfList(); node = node->GetNext()) {
        if (bStrCmp(toPlayer, node->player) == 0) {
            break;
        }
    }
    Invite *rc = nullptr;
    if (node != sentInvites.EndOfList()) {
        rc = node;
    }
    lobbyMutex.Unlock("LobbyChat::GetSentInvite");
    return rc;
}

Invite *LobbyChat::GetSentInvite(uint32 index) {
    lobbyMutex.Lock("LobbyChat::GetSentInvite");
    uint32 i = 0;
    Invite *node = sentInvites.GetHead();
    while (node != sentInvites.EndOfList() && i != index) {
        node = node->GetNext();
        i++;
    }
    Invite *rc = node;
    if (node == sentInvites.EndOfList()) {
        rc = nullptr;
    }
    lobbyMutex.Unlock("LobbyChat::GetSentInvite");
    return rc;
}

inline int LobbyChat::GetNumSentInvites() { return sentInvites.CountElements(); }

inline int LobbyChat::GetNumReceivedInvites() { return receivedInvites.CountElements(); }

inline void LobbyChat::AbortCommand() { LobbyCore::Instance().AbortCommand(nullptr, true); }

int32 LobbyChat::Init() {
    int32 rc = -1;
    if (LobbyCore::Instance().pLobbyRef) {
        if (LobbyApiSetCallback(LobbyCore::Instance().pLobbyRef, LOBBYAPI_CBTYPE_CHAT, GlobalChatCB, this) < 0) {
            Reset();
        } else {
            rc = 0;
        }
    }
    return rc;
}

void LobbyChat::Reset() {
    CancelAllInvites_HaveMutex();
    Invite *node = receivedInvites.GetHead();
    while (node != receivedInvites.EndOfList()) {
        if (RespondToInvite(node->player, node->gameIdent, LobbyChatN::RESPONSE_DECLINE) < 0) {
            delete receivedInvites.RemoveHead();
        }
        node = receivedInvites.GetHead();
    }
    NetworkCore::Instance().UnregisterProcessingFunc(InviteTimeoutFunc, this);
    if (LobbyCore::Instance().pLobbyRef) {
        LobbyApiSetCallback(LobbyCore::Instance().pLobbyRef, LOBBYAPI_CBTYPE_CHAT, nullptr, nullptr);
    }
    SetChatCallback(nullptr, nullptr);
    SetInviteCallback(nullptr, nullptr);
}

void LobbyChat::GlobalChatCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
    LobbyChat *lobbyChat = static_cast<LobbyChat *>(pData);
    char desc[64] = "";
    char attrib[256] = "";
    char fmt[256] = "";
    bool pvt_msg = false;
    char text[1024] = "";
    uint32 flags;

    TagFieldGetString(TagFieldFind(pMsg->pData, "N"), desc, sizeof(desc), "");
    if (desc[0] == 'T' && desc[1] == 'o' && desc[2] == ' ') {
        desc[0] = '-';
        desc[1] = '>';
    }

    flags = TagFieldGetFlags(TagFieldFind(pMsg->pData, "F"), 0);
    if (flags & 0x20000) {
        TagFieldGetString(TagFieldFind(pMsg->pData, "T"), text, sizeof(text), "");
        lobbyChat->ProcessInvite(desc, text);
        return;
    }
    if (flags & 0x08000000) {
        lobbyChat->ProcessCancelInvite(desc, TagFieldGetNumber(TagFieldFind(pMsg->pData, "T"), 0));
        return;
    }
    if (flags & 0x74000040) {
        lobbyChat->ProcessInviteResponse(desc, TagFieldGetNumber(TagFieldFind(pMsg->pData, "T"), 0), flags);
        return;
    }

    TagFieldGetString(TagFieldFind(pMsg->pData, "T"), text, sizeof(text), "");
    BuddySettings buddySettings;
    if (!BuddyCore::instance()->getApiRef() ||
        (!(buddySettings = EA_Messenger_GetBuddySettingsByName(desc)).pBuddy || !buddySettings.isBlocked)) {
        if (pMsg->kind == 'chat') {
            if (pMsg->code & 0x08000000) {
                bSPrintf(fmt, "%s %s", desc, text);
            } else {
                bSPrintf(fmt, "%s%s> %s", desc, attrib, text);
            }
        } else if (pMsg->kind == 'priv') {
            if (desc[0] == '-' && desc[1] == '>' && desc[2] == ' ') {
                bSPrintf(fmt, "%s> %s", GetLocalizedString(0xda2b33af), text);
            } else {
                bSPrintf(fmt, "%s%s %s> %s", desc, attrib, GetLocalizedString(0xda2b33af), text);
            }
            pvt_msg = true;
        }

        if (!lobbyChat->IsServerStatusMessage(desc, text)) {
            if (bStrLen(fmt) > 0 && lobbyChat->userChatCB && lobbyChat->pChatWindow) {
                lobbyChat->userChatCB(lobbyChat->pChatWindow, fmt, pvt_msg, false, (pMsg->code >> 21) & 1);
            }
        }
        LobbyApiDebug(pRef, pMsg);
    }
}
