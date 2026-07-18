#include "LobbyChat.hpp"

extern "C" {
void TagFieldSetFlags(char *buffer, int bufferSize, const char *name, int flags);
}

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
        return IERR_INVALID_PARAM;
    }
    if (bStrICmp(myPersona, toPlayer) == 0) {
        lobbyMutex.Unlock("LobbyChat::SendGameInvite");
        return IERR_CANT_SELF_INVITE;
    }
    if (myGame) {
        for (int i = 0; i < myGame->iCount; i++) {
            if (bStrICmp(toPlayer, myGame->aOpponents[i].strPers) == 0) {
                lobbyMutex.Unlock("LobbyChat::SendGameInvite");
                return IERR_PLAYER_IN_GAME;
            }
        }
    }
    for (Invite *node = sentInvites.GetHead(); node != sentInvites.EndOfList(); node = node->GetNext()) {
        if (bStrICmp(node->player, toPlayer) == 0) {
            lobbyMutex.Unlock("LobbyChat::SendGameInvite");
            return IERR_ALREADY_PENDING;
        }
    }
    if (sentInvites.CountElements() == maxActiveInvites) {
        lobbyMutex.Unlock("LobbyChat::SendGameInvite");
        return IERR_TOO_MANY_INVITES;
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
    return IERR_OUT_OF_MEMORY;
}
