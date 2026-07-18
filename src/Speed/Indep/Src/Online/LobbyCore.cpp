#include "LobbyCore.hpp"

void *operator new(size_t size, const char *file, int line, int allocationParams);

extern "C" {
void LobbyApiUpdate(LobbyApiRefT *lobbyRef);
int LobbyApiRequestCB(LobbyApiRefT *lobbyRef, int kind, char *request, LobbyApiCallbackT *callback, void *context);
void *LobbyApiInfoPtr(LobbyApiRefT *lobbyRef, int selector);
void LobbyApiSuspend(LobbyApiRefT *lobbyRef);
void LobbyApiResume(LobbyApiRefT *lobbyRef);
LobbyPingManagerRefT *PingManagerCreate(int timeout);
void PingManagerDestroy(LobbyPingManagerRefT *pingManager);
int TagFieldPrintf(char *buffer, int bufferSize, const char *format, ...);
void TagFieldSetEpoch(char *buffer, int bufferSize, const char *name, uint32 epoch);
}

struct BuildRegion {
    static char *GetSkuCode();
};

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

int32 LobbyCore::SetAsyncServerMessages() {
    int32 rc;

    lobbyMutex.Lock("LobbyCore::SetAsyncServerMessages");
    rc = SetAsyncServerMessages_HaveMutex();
    lobbyMutex.Unlock("LobbyCore::SetAsyncServerMessages");
    return rc;
}

int32 LobbyCore::SetAsyncServerMessages_HaveMutex() {
    char buf[128] = "";
    GameSession *session;

    TagFieldPrintf(buf, sizeof(buf), "MYGAME=1 STATS=5000 ASYNC=1 MESGS=1 ROOMS=1 USERSETS=1");
    session = LobbyGameSessions::Instance().GetMySession();
    if (session) {
        TagFieldSetString(buf, sizeof(buf), "USERSET0", session->strName);
    }
    return QueueCommand('sele', buf, nullptr, nullptr, nullptr, nullptr, true);
}

void LobbyCore::SaveRaceResults(const char *results) {
    if (results) {
        lobbyMutex.Lock("LobbyCore::SaveRaceResults");
        bStrNCpy(raceResults.results, results, sizeof(raceResults.results) - 1);
        lobbyMutex.Unlock("LobbyCore::SaveRaceResults");
    }
}

int32 LobbyCore::SendSavedRaceResults(CommandCBFunc func, void *context) {
    int32 rc = 0;

    lobbyMutex.Lock("LobbyCore::SendSavedRaceResults");
    if (bStrLen(raceResults.results) > 0) {
        char results[2352] = "";

        TagFieldSetString(results, sizeof(results), "AUTH", raceResults.auth);
        TagFieldSetEpoch(results, sizeof(results), "WHEN", static_cast<uint32>(raceResults.when));
        TagFieldSetString(results, sizeof(results), "REPT", raceResults.rept);
        TagFieldSetString(results, sizeof(results), "RESU", raceResults.results);
        TagFieldSetString(results, sizeof(results), "VENUE", "NFSMW");
        TagFieldSetString(results, sizeof(results), "SKU", BuildRegion::GetSkuCode());

        for (int i = 0; i < 4 && bStrLen(raceResults.name[i]) > 0; i++) {
            char param[8];

            bSPrintf(param, "NAME%d", i);
            TagFieldSetString(results, sizeof(results), param, raceResults.name[i]);
            bSPrintf(param, "TEAM%d", i);
            TagFieldSetString(results, sizeof(results), param, "NFSMW");
            bSPrintf(param, "ROOM%d", i);
            TagFieldSetString(results, sizeof(results), param, "");
        }
        rc = QueueCommand('rank', results, RankCB, this, func, context, false);
    }
    lobbyMutex.Unlock("LobbyCore::SendSavedRaceResults");
    return rc;
}

LobbyApiServerStatT *LobbyCore::GetServerStats() {
    return static_cast<LobbyApiServerStatT *>(LobbyApiInfoPtr(pLobbyRef, 'serv'));
}

void LobbyCore::AbortCommand(int32 commandID) {
    lobbyMutex.Lock("LobbyCore::AbortCommand");
    if (currentCommand && currentCommand->commandID == commandID) {
        AbortCurrentCommand();
    } else {
        for (LobbyCommand *lc = lobbyCommandQueue.GetHead(); lc != lobbyCommandQueue.EndOfList(); lc = lc->GetNext()) {
            if (lc->commandID == commandID) {
                delete lobbyCommandQueue.Remove(lc);
                break;
            }
        }
    }
    lobbyMutex.Unlock("LobbyCore::AbortCommand");
}

void LobbyCore::AbortCommand(CommandCBFunc commandCB, bool abortAll) {
    lobbyMutex.Lock("LobbyCore::AbortCommand");
    if (currentCommand && currentCommand->commandCB == commandCB) {
        AbortCurrentCommand();
        if (!abortAll) {
            lobbyMutex.Unlock("LobbyCore::AbortCommand");
            return;
        }
    }

    for (LobbyCommand *lc = lobbyCommandQueue.GetHead(); lc != lobbyCommandQueue.EndOfList(); lc = lc->GetNext()) {
        if (lc->commandCB == commandCB) {
            lc = lc->GetPrev();
            delete lobbyCommandQueue.Remove(lc->GetNext());
            if (!abortAll) {
                break;
            }
        }
    }
    lobbyMutex.Unlock("LobbyCore::AbortCommand");
}

void LobbyCore::AbortGroup(int32 commandID) {
    int32 group = 0;

    if (currentCommand && currentCommand->commandID == commandID) {
        group = currentCommand->groupID;
        AbortCurrentCommand();
    } else {
        for (LobbyCommand *lc = lobbyCommandQueue.GetHead(); lc != lobbyCommandQueue.EndOfList(); lc = lc->GetNext()) {
            if (lc->commandID == commandID) {
                group = lc->groupID;
                delete lobbyCommandQueue.Remove(lc);
                break;
            }
        }
    }

    if (group) {
        for (LobbyCommand *lc = lobbyCommandQueue.GetHead(); lc != lobbyCommandQueue.EndOfList(); lc = lc->GetNext()) {
            if (lc->groupID == group) {
                lc = lc->GetPrev();
                delete lobbyCommandQueue.Remove(lc->GetNext());
            }
        }
    }
}

void LobbyCore::AbortAllCommands() {
    lobbyMutex.Lock("LobbyCore::AbortAllCommands");
    AbortAllCommands_HaveMutex();
    lobbyMutex.Unlock("LobbyCore::AbortAllCommands");
}

void LobbyCore::AbortAllCommands_HaveMutex() {
    AbortCurrentCommand();
    while (!lobbyCommandQueue.IsEmpty()) {
        delete lobbyCommandQueue.RemoveHead();
    }
}

void LobbyCore::SetDisconnectCallback(DisconnectCBFunc func, void *context) {
    disconnectContext = context;
    disconnectCB = func;
}

char *LobbyCore::GetServerConfig() {
    if (!pLobbyRef) {
        return nullptr;
    }
    return static_cast<char *>(LobbyApiInfoPtr(pLobbyRef, 'conf'));
}

LobbyApiRefT *LobbyCore::GetLobbyApiRef() { return pLobbyRef; }

void LobbyCore::Suspend() {
    lobbyMutex.Lock("LobbyCore::Suspend");
    if (!mIsSuspended) {
        if (pLobbyRef) {
            LobbyApiSuspend(pLobbyRef);
        }
        if (pingManagerRef) {
            PingManagerDestroy(pingManagerRef);
            pingManagerRef = nullptr;
        }
        LobbyGames::Instance().Reset();
        LobbyRooms::Instance().Reset();
        LobbyGameSessions::Instance().Suspend();
        if (!FEDatabase->IsLANMode()) {
            BuddyCore::instance()->pause();
        }
        mIsSuspended = true;
    }
    lobbyMutex.Unlock("LobbyCore::Suspend");
}

void LobbyCore::Resume(bool fullResume) {
    lobbyMutex.Lock("LobbyCore::Resume");
    if (mIsSuspended) {
        if (pLobbyRef) {
            LobbyApiResume(pLobbyRef);
            SetAsyncServerMessages_HaveMutex();
        }
        if (fullResume) {
            pingManagerRef = PingManagerCreate(1000);
            LobbyGames::Instance().Init();
            LobbyRooms::Instance().Init();
            LobbyGameSessions::Instance().Resume();
            if (!FEDatabase->IsLANMode()) {
                BuddyCore::instance()->resume();
            }
        }
        mIsSuspended = false;
    }
    SendSavedRaceResults(nullptr, nullptr);
    lobbyMutex.Unlock("LobbyCore::Resume");
}

int32 LobbyCore::QueueCommand(int kind, const char *request, LobbyApiCallbackT *lobbyCB, void *lobbyContext,
                              CommandCBFunc commandCB, void *commandContext, bool makeNextCommand) {
    LobbyCommand *lc = new ("d:/p4_apex1666_d1001856/mw/speed/indep/src/online/LobbyCore.cpp", 0x385, 8)
        LobbyCommand(kind, request, groupStarted, lobbyCB, lobbyContext, commandCB, commandContext);
    if (!lc) {
        return -2;
    }

    if (makeNextCommand) {
        lobbyCommandQueue.AddHead(lc);
    } else {
        lobbyCommandQueue.AddTail(lc);
    }
    return lc->commandID;
}

int32 LobbyCore::FindCommandID(int kind, LobbyApiCallbackT *lobbyCB, void *lobbyContext, CommandCBFunc commandCB,
                               void *commandContext) {
    LobbyCommand *lc = currentCommand;
    if (lc && kind == lc->kind && (!lobbyCB || lobbyCB == lc->lobbyCB) && (!lobbyContext || lobbyContext == lc->lobbyContext) &&
        (!commandCB || commandCB == lc->commandCB) && (!commandContext || commandContext == lc->commandContext)) {
        return lc->commandID;
    }

    for (lc = lobbyCommandQueue.GetHead(); lc != lobbyCommandQueue.EndOfList(); lc = lc->GetNext()) {
        if (kind == lc->kind && (!lobbyCB || lobbyCB == lc->lobbyCB) && (!lobbyContext || lobbyContext == lc->lobbyContext) &&
            (!commandCB || commandCB == lc->commandCB) && (!commandContext || commandContext == lc->commandContext)) {
            return lc->commandID;
        }
    }
    return -1;
}
