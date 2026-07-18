#include "LobbyGames.hpp"

LobbyGames::LobbyGames()
    : resultUpdateCB(nullptr), //
      resultContext(nullptr) {
    bMemSet(&myCurrentGame, 0, sizeof(myCurrentGame));
    myCurrentGame.iIdent = -1;
}

LobbyGames &LobbyGames::Instance() {
    static LobbyGames theLobbyGames;
    return theLobbyGames;
}

void LobbyGames::SetResultUpdateCB(ResultUpdateCBFunc func, void *context) {
    lobbyMutex.Lock("LobbyGames::SetResultUpdateCB");
    resultContext = context;
    resultUpdateCB = func;
    lobbyMutex.Unlock("LobbyGames::SetResultUpdateCB");
}

int32 LobbyGames::StartGame(CommandCBFunc startGameCB, void *context, bool kickUnready) {
    lobbyMutex.Lock("LobbyGames::StartGame");
    if (myCurrentGame.iIdent != -1 &&
        bStrCmp(myCurrentGame.strHost, FEDatabase->OnlineSettings.GetLobbyPersona()) == 0 &&
        LobbyCore::Instance().FindCommandID('gsta', nullptr, nullptr, nullptr, nullptr) == -1 &&
        myCurrentGame.iCount >= myCurrentGame.iMinsize) {
        char buf[512] = "";
        int32 rc = 0;
        if (myCurrentGame.iCount >= myCurrentGame.iMinsize) {
            TagFieldSetString(buf, sizeof(buf), "NAME", myCurrentGame.strName);
            rc = LobbyCore::Instance().QueueCommand('gsta', buf, LobbyCore::DefaultCB, nullptr, startGameCB, context, false);
        }
        lobbyMutex.Unlock("LobbyGames::StartGame");
        return rc;
    }
    lobbyMutex.Unlock("LobbyGames::StartGame");
    return -1;
}
