#include "LobbyGames.hpp"

extern "C" {
int PingManagerPingAddress(LobbyPingManagerRefT *pingManager, DirtyAddrT *address, LobbyPingManagerCallbackT *func, void *context);
}

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

int32 LobbyGames::UpdateGame(CommandCBFunc updateGameCB, void *context) {
    lobbyMutex.Lock("LobbyGames::UpdateGame");
    if (myCurrentGame.iIdent != -1 && bStrCmp(myCurrentGame.strHost, FEDatabase->OnlineSettings.GetLobbyPersona()) == 0) {
        char buf[64] = "";
        TagFieldSetString(buf, sizeof(buf), "NAME", myCurrentGame.strName);
        TagFieldSetNumber(buf, sizeof(buf), "MINSIZE", FEDatabase->OnlineSettings.MinOnlinePlayers);
        int32 rc = LobbyCore::Instance().QueueCommand('gset', buf, LobbyCore::DefaultCB, nullptr, updateGameCB, context, false);
        lobbyMutex.Unlock("LobbyGames::UpdateGame");
        return rc;
    }
    lobbyMutex.Unlock("LobbyGames::UpdateGame");
    return -1;
}

inline int32 LobbyGames::LockGame(CommandCBFunc lockGameCB, void *context) {
    lobbyMutex.Lock("LobbyGames::LockGame");
    if (myCurrentGame.iIdent != -1 && bStrCmp(myCurrentGame.strHost, FEDatabase->OnlineSettings.GetLobbyPersona()) == 0 &&
        !(myCurrentGame.uSysflags & 0x1000)) {
        bool areSomePlayersReady = false;
        for (int i = 0; i < myCurrentGame.iCount; i++) {
            if (myCurrentGame.aOpponents[i].uFlags & 0x08000000) {
                areSomePlayersReady = true;
            }
        }
        char buf[128] = "";
        TagFieldSetString(buf, sizeof(buf), "NAME", myCurrentGame.strName);
        TagFieldSetNumber(buf, sizeof(buf), "SYSFLAGS", myCurrentGame.uSysflags | 0x1000);
        TagFieldSetNumber(buf, sizeof(buf), "USERFLAGS",
                          areSomePlayersReady ? myCurrentGame.aOpponents[0].uFlags | 0x08000000
                                              : myCurrentGame.aOpponents[0].uFlags);
        int32 rc = LobbyCore::Instance().QueueCommand('gset', buf, LobbyCore::DefaultCB, nullptr, lockGameCB, context, false);
        lobbyMutex.Unlock("LobbyGames::LockGame");
        return rc;
    }
    lobbyMutex.Unlock("LobbyGames::LockGame");
    return -1;
}

int32 LobbyGames::UnlockGame(CommandCBFunc unlockGameCB, void *context) {
    lobbyMutex.Lock("LobbyGames::UnlockGame");
    if (myCurrentGame.iIdent != -1 && bStrCmp(myCurrentGame.strHost, FEDatabase->OnlineSettings.GetLobbyPersona()) == 0 &&
        (myCurrentGame.uSysflags & 0x1000)) {
        char buf[128] = "";
        TagFieldSetString(buf, sizeof(buf), "NAME", myCurrentGame.strName);
        TagFieldSetNumber(buf, sizeof(buf), "SYSFLAGS", myCurrentGame.uSysflags & ~0x1000);
        TagFieldSetNumber(buf, sizeof(buf), "USERFLAGS", myCurrentGame.aOpponents[0].uFlags & ~0x08000000);
        int32 rc = LobbyCore::Instance().QueueCommand('gset', buf, LobbyCore::DefaultCB, nullptr, unlockGameCB, context, false);
        lobbyMutex.Unlock("LobbyGames::UnlockGame");
        return rc;
    }
    lobbyMutex.Unlock("LobbyGames::UnlockGame");
    return -1;
}

LobbyApiPlayT *LobbyGames::GetMyGame() const {
    LobbyApiPlayT *game = nullptr;
    if (myCurrentGame.iIdent != -1) {
        game = const_cast<LobbyApiPlayT *>(&myCurrentGame);
    }
    return game;
}

inline int LobbyGames::PingPlayer(const char *persona, LobbyApiPlayT *game, LobbyPingManagerCallbackT *func, void *context) {
    LobbyApiPlayT *theGame = game ? game : GetMyGame();
    if (!theGame) {
        return -1;
    }
    if (!persona) {
        persona = FEDatabase->OnlineSettings.GetLobbyPersona();
    }
    for (int i = 0; i < theGame->iCount; i++) {
        if (bStrCmp(persona, theGame->aOpponents[i].strPers) == 0) {
            int rc = DoPing(i, theGame, func, context);
            return rc;
        }
    }
    return -1;
}

inline int LobbyGames::PingPlayer(int index, LobbyApiPlayT *game, LobbyPingManagerCallbackT *func, void *context) {
    int rc = -1;
    LobbyApiPlayT *theGame = game ? game : GetMyGame();
    if (theGame && index >= 0 && index < theGame->iCount) {
        rc = DoPing(index, theGame, func, context);
    }
    return rc;
}

inline void LobbyGames::AbortCommand(int32 commandID) { LobbyCore::Instance().AbortCommand(commandID); }

int32 LobbyGames::Init() {
    int32 rc = -1;
    if (LobbyCore::Instance().pLobbyRef) {
        Reset();
        if (LobbyCore::Instance().RegisterGlobalCallback(LOBBYAPI_CBTYPE_EVNT, GlobalEventCB, this) < 0) {
            Reset();
        } else {
            rc = 0;
        }
    }
    return rc;
}

void LobbyGames::Reset() {
    LobbyCore::Instance().UnregisterGlobalCallback(LOBBYAPI_CBTYPE_EVNT, GlobalEventCB, this);
    resultUpdateCB = nullptr;
    resultContext = nullptr;
    bMemSet(&myCurrentGame, 0, sizeof(myCurrentGame));
    myCurrentGame.iIdent = -1;
}

inline int LobbyGames::DoPing(int index, LobbyApiPlayT *game, LobbyPingManagerCallbackT *func, void *context) {
    LobbyApiPlayT *theGame = game ? game : GetMyGame();
    if (!theGame || index < 0 || index >= theGame->iCount) {
        return -1;
    }
    DirtyAddrT addr;
    DirtyAddrFromHostAddr(&addr, &theGame->aOpponents[index].uAddr);
    return PingManagerPingAddress(LobbyCore::Instance().pingManagerRef, &addr, func, context);
}
