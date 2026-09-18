#include "LobbyGames.hpp"
#include "LobbyGameSessions.hpp"

extern "C" {
int PingManagerPingAddress(LobbyPingManagerRefT *pingManager, DirtyAddrT *address, LobbyPingManagerCallbackT *func, void *context);
void LobbyApiExtractPlayRecord(LobbyApiPlayT *game, const char *record);
int TagFieldGetStructure(const char *field, void *buffer, int bufferSize, const char *format);
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

int32 LobbyGames::CreateGame(const char *gameName, cOnlineSettings &raceOptions, CommandCBFunc createGameCB, void *context) {
    if (!gameName || !*gameName || myCurrentGame.iIdent != -1 ||
        LobbyCore::Instance().FindCommandID('gjoi', nullptr, nullptr, nullptr, nullptr) != -1 ||
        LobbyCore::Instance().FindCommandID('gcre', nullptr, nullptr, nullptr, nullptr) != -1) {
        return -1;
    }

    char buf[1024] = "";
    TagFieldSetString(buf, sizeof(buf), "NAME", gameName);
    TagFieldSetNumber(buf, sizeof(buf), "MAXSIZE", cOnlineSettings::MaxOnlinePlayers);
    TagFieldSetNumber(buf, sizeof(buf), "MINSIZE", raceOptions.MinOnlinePlayers);
    TagFieldSetNumber(buf, sizeof(buf), "SYSFLAGS", raceOptions.RankedGame == 1 ? 0x40000 : 0);
    return LobbyCore::Instance().QueueCommand('gcre', buf, GameCreatedCB, this, createGameCB, context, false);
}

int32 LobbyGames::JoinGame(const char *gameName, const char *password, CommandCBFunc joinGameCB, void *context) {
    int32 rc = -1;
    if (myCurrentGame.iIdent == -1 &&
        LobbyCore::Instance().FindCommandID('gjoi', nullptr, nullptr, nullptr, nullptr) == -1 &&
        LobbyCore::Instance().FindCommandID('gcre', nullptr, nullptr, nullptr, nullptr) == -1) {
        LobbyUsers::Instance().ClearUserOnlineRecordCache();
        OnlineIsServer = 0;
        char buf[96] = "";
        TagFieldSetString(buf, sizeof(buf), "NAME", gameName);
        if (password && *password) {
            TagFieldSetString(buf, sizeof(buf), "PASS", password);
        }
        rc = LobbyCore::Instance().QueueCommand('gjoi', buf, GameCreatedCB, this, joinGameCB, context, false);
    }
    return rc;
}

int32 LobbyGames::LeaveGame(CommandCBFunc leaveGameCB, void *context) {
    if (myCurrentGame.iIdent == -1 ||
        LobbyCore::Instance().FindCommandID('gdel', nullptr, nullptr, nullptr, nullptr) != -1 ||
        LobbyCore::Instance().FindCommandID('glea', nullptr, nullptr, nullptr, nullptr) != -1) {
        return -1;
    }

    char buf[64] = "";
    TagFieldSetString(buf, sizeof(buf), "NAME", myCurrentGame.strName);
    if (bStrCmp(FEDatabase->OnlineSettings.GetLobbyPersona(), myCurrentGame.strHost) != 0) {
        return LobbyCore::Instance().QueueCommand('glea', buf, LeaveGameCB, this, leaveGameCB, context, false);
    }
    return LobbyCore::Instance().QueueCommand('gdel', buf, LeaveGameCB, this, leaveGameCB, context, false);
}

inline int32 LobbyGames::KickPlayer(const char *name) {
    if (!name || !*name || myCurrentGame.iIdent == -1) {
        return -1;
    }
    char buf[128] = "";
    TagFieldSetString(buf, sizeof(buf), "NAME", myCurrentGame.strName);
    TagFieldSetString(buf, sizeof(buf), "PERS", name);
    return LobbyCore::Instance().QueueCommand('gkik', buf, LobbyCore::DefaultCB, nullptr, nullptr, nullptr, false);
}

inline void LobbyGames::GamesDispListCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
    LobbyApiDebug(pRef, pMsg);
}

void LobbyGames::GameCreatedCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
    LobbyGames *lobbyGames = static_cast<LobbyGames *>(pData);
    if (pMsg->code == 0) {
        LobbyApiExtractPlayRecord(&lobbyGames->myCurrentGame, pMsg->pData);
        if (bStrCmp(lobbyGames->myCurrentGame.strHost, FEDatabase->OnlineSettings.GetLobbyPersona()) == 0) {
            OnlineIsServer = 1;
            ConnectionCore::Instance().HostSession(lobbyGames->myCurrentGame.iIdent, 3);
        } else {
            OnlineIsServer = 0;
            ConnectionCore::Instance().JoinSession(lobbyGames->myCurrentGame.aOpponents[0], lobbyGames->myCurrentGame.iIdent,
                                                   lobbyGames->myCurrentGame.strSess, 3);
            for (int i = 1; i < lobbyGames->myCurrentGame.iCount; i++) {
                ConnectionCore::Instance().AddPlayer(lobbyGames->myCurrentGame.aOpponents[i]);
            }
        }
    }
    LobbyCore::Instance().FinishCommand(pMsg, true);
}

void LobbyGames::LeaveGameCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
    LobbyGames *lobbyGames = static_cast<LobbyGames *>(pData);
    if (pMsg->code == 0 || pMsg->code == 'ugam') {
        bMemSet(&lobbyGames->myCurrentGame, 0, sizeof(lobbyGames->myCurrentGame));
        lobbyGames->myCurrentGame.iIdent = -1;
    }
    LobbyCore::Instance().FinishCommand(pMsg, true);
}

void LobbyGames::GlobalEventCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
    LobbyGames *lobbyGames = static_cast<LobbyGames *>(pData);
    static LobbyGamesN::GameResultUpdateT results;

    if (pMsg->kind == 'game' && lobbyGames->myCurrentGame.iIdent != -1) {
        LobbyApiPlayT &myGame = lobbyGames->myCurrentGame;
        if (!TagFieldFind(pMsg->pData, "NAME")) {
            bMemSet(&myGame, 0, sizeof(myGame));
            myGame.iIdent = -1;
        } else {
            LobbyApiExtractPlayRecord(&myGame, pMsg->pData);
            LobbyUsers::Instance().ClearUserOnlineRecordCache(myGame);
            if (LobbyUsers::Instance().GetMyUserRecord()->game == 0) {
                bMemSet(&myGame, 0, sizeof(myGame));
                myGame.iIdent = -1;
                ConnectionCore::Instance().UpdatePlayers(myGame);
                return;
            }
        }
        ConnectionCore::Instance().UpdatePlayers(myGame);
        LobbyGameSessions::Instance().SendUpdateCallback(LobbyGameSessionsN::SESSION_CHANGED);
    } else if (pMsg->kind == 'play') {
        if (lobbyGames->myCurrentGame.iIdent != -1) {
            LobbyApiPlayT &myGame = lobbyGames->myCurrentGame;
            LobbyCore::Instance().AbortAllCommands_HaveMutex();
            LobbyApiExtractPlayRecord(&myGame, pMsg->pData);
            LobbyCore::raceResults.Clear();
            bStrCpy(LobbyCore::raceResults.auth, myGame.strAuth);
            LobbyCore::raceResults.when = myGame.uWhen;
            bStrCpy(LobbyCore::raceResults.rept, FEDatabase->OnlineSettings.GetLobbyPersona());
            for (int i = 0; i < myGame.iCount; i++) {
                bStrCpy(LobbyCore::raceResults.name[i], myGame.aOpponents[i].strPers);
            }
            for (int i = 0; i < myGame.iCount; i++) {
                int idx = i;
                TheOnlineGame.SetPlayerMuted(idx, VoiceCore::mInstance->IsMuted(myGame.aOpponents[i].strPers));
            }
            LobbyGameSessions::Instance().SendUpdateCallback(LobbyGameSessionsN::GAME_STARTED);
        }
    } else if (pMsg->kind == '$rup' && lobbyGames->resultUpdateCB) {
        bMemSet(&results, 0, sizeof(results));
        for (int i = 0; i < 8; i++) {
            char buf[12] = "";
            bSPrintf(buf, "OPPO%d", i);
            TagFieldGetString(TagFieldFind(pMsg->pData, buf), results.name[i], sizeof(results.name[i]), "");
            bSPrintf(buf, "STAT%d", i);
            results.status[i] = TagFieldGetNumber(TagFieldFind(pMsg->pData, buf), 0);
            results.time = TagFieldGetNumber(TagFieldFind(pMsg->pData, "TIME"), 0);
        }
        results.final = false;
        lobbyGames->resultUpdateCB(&results, lobbyGames->resultContext);
    } else if (pMsg->kind == '$rfn' && lobbyGames->resultUpdateCB) {
        struct ResultBuffer {
            int32 score;
            int32 place;
            int32 save;
            int32 reason;
            int32 points;
        };

        bMemSet(&results, 0, sizeof(results));
        for (int i = 0; i < 8; i++) {
            char buf[12] = "";
            bSPrintf(buf, "OPPO%d", i);
            if (TagFieldGetString(TagFieldFind(pMsg->pData, buf), results.name[i], sizeof(results.name[i]), "") > 0) {
                ResultBuffer rbuf;
                bSPrintf(buf, "DATA%d", i);
                TagFieldGetStructure(TagFieldFind(pMsg->pData, buf), &rbuf, sizeof(rbuf), "l*");
                results.score[i] = rbuf.score;
                results.place[i] = rbuf.place;
                results.save[i] = rbuf.save == 1;
                if (rbuf.save != 1) {
                    results.reason[i] = rbuf.reason;
                }
                results.points[i] = static_cast<unsigned char>(rbuf.points);
            }
        }
        results.final = true;
        lobbyGames->resultUpdateCB(&results, lobbyGames->resultContext);
    }
}
