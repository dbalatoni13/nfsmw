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
