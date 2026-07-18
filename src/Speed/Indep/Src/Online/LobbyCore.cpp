#include "LobbyCore.hpp"

extern "C" {
void LobbyApiUpdate(LobbyApiRefT *lobbyRef);
int LobbyApiRequestCB(LobbyApiRefT *lobbyRef, int kind, char *request, LobbyApiCallbackT *callback, void *context);
}

LobbyCore::LobbyCore()
    : pLobbyRef(nullptr),             //
      pingManagerRef(nullptr),        //
      currentCommand(nullptr),        //
      disconnectCB(nullptr),          //
      disconnectContext(nullptr),     //
      groupStarted(false),            //
      raceResultsAccepted(false),     //
      cheatResponseActive(false),     //
      sendCDKey(false),               //
      mIsSuspended(false) {}

LobbyCore::~LobbyCore() { Reset(); }

LobbyCore &LobbyCore::Instance() {
    static LobbyCore theLobby;
    return theLobby;
}

void LobbyCore::DoProcessing() {
    lobbyMutex.Lock("LobbyCore::DoProcessing");
    if (pLobbyRef) {
        LobbyApiUpdate(pLobbyRef);
        if (!currentCommand && !lobbyCommandQueue.IsEmpty()) {
            currentCommand = lobbyCommandQueue.RemoveHead();
            currentCommand->lobbyCallbackID = LobbyApiRequestCB(pLobbyRef, currentCommand->kind, currentCommand->req,
                                                                currentCommand->lobbyCB, currentCommand->lobbyContext);
            if (!currentCommand->lobbyCB) {
                FinishCommand(nullptr, true);
            }
        }
    }
    lobbyMutex.Unlock("LobbyCore::DoProcessing");
}
