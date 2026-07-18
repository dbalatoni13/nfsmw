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
