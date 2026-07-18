#include "LobbyCore.hpp"
#include "LobbyGames.hpp"
#include "LobbyGameSessions.hpp"

void *operator new(size_t size, const char *file, int line, int allocationParams);
void *operator new[](size_t size, const char *file, int line, int allocationParams);

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
char *NetConnMAC();
LobbyApiRefT *LobbyApiCreate(const char *version, void *ref, void (*printfn)(void *, const char *));
int LobbyApiControl(LobbyApiRefT *lobbyRef, int selector, int value);
int LobbyApiSetCallback(LobbyApiRefT *lobbyRef, LobbyApiCBTypeE type, LobbyApiCallbackT *callback, void *context);
void LobbyApiClearCallback(LobbyApiRefT *lobbyRef, LobbyApiCBTypeE type);
void LobbyApiDisconnect(LobbyApiRefT *lobbyRef, int reason);
void LobbyApiDestroy(LobbyApiRefT *lobbyRef);
void LobbyApiCancelCB(LobbyApiRefT *lobbyRef);
void LobbyApiDebug(LobbyApiRefT *lobbyRef, LobbyApiMsgT *msg);
}

struct BuildRegion {
    static char *GetAbbreviation();
    static char *GetSkuCode();
    static char *GetSlusCode();
};

struct ISOCodes {
    static char *GetCountryISOCode();
    static char *GetLanguageISOCode();
};

extern int BuildVersionChangelistNumber;
void GetGameVersionNumberString(char *buffer, int bufferSize);

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

int LobbyCore::RegisterGlobalCallback(LobbyApiCBTypeE type, LobbyApiCallbackT *func, void *context) {
    if (type < LOBBYAPI_CBTYPE_RESP || type > LOBBYAPI_CBTYPE_IDLE || !func) {
        return -1;
    }

    for (GlobalCB *cb = globalCBList[type].GetHead(); cb != globalCBList[type].EndOfList(); cb = cb->GetNext()) {
        if (cb->cbFunc == func && cb->context == context) {
            return 0;
        }
    }

    GlobalCB *cb = new ("d:/p4_apex1666_d1001856/mw/speed/indep/src/online/LobbyCore.cpp", 0x40d, 8) GlobalCB;
    cb->cbFunc = func;
    cb->context = context;
    if (cb) {
        globalCBList[type].AddTail(cb);
        return 0;
    }
    return -1;
}

void LobbyCore::UnregisterGlobalCallback(LobbyApiCBTypeE type, LobbyApiCallbackT *func, void *context) {
    if (type < LOBBYAPI_CBTYPE_RESP || type > LOBBYAPI_CBTYPE_IDLE) {
        return;
    }

    for (GlobalCB *cb = globalCBList[type].GetHead(); cb != globalCBList[type].EndOfList(); cb = cb->GetNext()) {
        if (cb->cbFunc == func && cb->context == context) {
            delete globalCBList[type].Remove(cb);
            return;
        }
    }
}

void LobbyCore::FinishCommand(LobbyApiMsgT *msg, bool doCallback) {
    if (currentCommand) {
        if (doCallback == true && currentCommand->commandCB) {
            currentCommand->commandCB(msg, currentCommand->commandContext);
        }
        delete currentCommand;
        currentCommand = nullptr;
    }
}

void LobbyCore::LobbyPrintf(void *ref, const char *text) {
    if (text) {
        bStrLen(text);
    }
}

int32 LobbyCore::Init() {
    if (pLobbyRef) {
        return 0;
    }

    char vers[1024] = "";
    char *productName;
    char *year;
    char *platform;

    TagFieldSetString(vers, sizeof(vers), "REGN", BuildRegion::GetAbbreviation());
    TagFieldSetNumber(vers, sizeof(vers), "CLST", BuildVersionChangelistNumber);
    TagFieldSetNumber(vers, sizeof(vers), "NETV", NetworkCore::Instance().GetNetworkVersionNumber());
    if (ISOCodes::GetCountryISOCode()) {
        TagFieldSetString(vers, sizeof(vers), "FROM", ISOCodes::GetCountryISOCode());
    }
    if (ISOCodes::GetLanguageISOCode()) {
        TagFieldSetString(vers, sizeof(vers), "LANG", ISOCodes::GetLanguageISOCode());
    }
    TagFieldSetString(vers, sizeof(vers), "MID", NetConnMAC());

    productName = OLGetProductName();
    year = OLGetProductYear();
    platform = OLGetPlatform();
    char temp[64] = "";
    bSPrintf(temp, "%s-%s-%s", productName, platform, year);
    TagFieldSetString(vers, sizeof(vers), "PROD", temp);

    char version[16] = "";
    GetGameVersionNumberString(version, sizeof(version));
    bSPrintf(temp, "%s/%s-%s", platform, version, "Sep 20 2005");
    TagFieldSetString(vers, sizeof(vers), "VERS", temp);
    TagFieldSetString(vers, sizeof(vers), "SLUS", BuildRegion::GetSlusCode());
    TagFieldSetString(vers, sizeof(vers), "SKU", BuildRegion::GetSkuCode());

    pingManagerRef = PingManagerCreate(1000);
    pLobbyRef = LobbyApiCreate(vers, nullptr, LobbyPrintf);
    if (!pLobbyRef) {
        Reset();
        return -1;
    }

    LobbyApiControl(pLobbyRef, 'itim', 40000);
    LobbyApiSetCallback(pLobbyRef, LOBBYAPI_CBTYPE_CONN, GlobalConnStatusCB, this);
    LobbyApiSetCallback(pLobbyRef, LOBBYAPI_CBTYPE_RESP, GlobalResponseCB, this);
    LobbyApiSetCallback(pLobbyRef, LOBBYAPI_CBTYPE_EVNT, GlobalEventCB, this);
    return 0;
}

void LobbyCore::Reset() {
    int type;
    for (type = 0; type < LOBBYAPI_CBTYPE_TOTAL; type++) {
        while (!globalCBList[type].IsEmpty()) {
            GlobalCB *cb = globalCBList[type].RemoveHead();
            delete cb;
        }
    }

    if (pLobbyRef) {
        AbortAllCommands_HaveMutex();
        for (LobbyApiCBTypeE cb = LOBBYAPI_CBTYPE_RESP; cb < LOBBYAPI_CBTYPE_TOTAL;
             cb = static_cast<LobbyApiCBTypeE>(cb + 1)) {
            LobbyApiClearCallback(pLobbyRef, cb);
        }
        LobbyApiDisconnect(pLobbyRef, 0);
        if (pingManagerRef) {
            PingManagerDestroy(pingManagerRef);
            pingManagerRef = nullptr;
        }
        LobbyApiDestroy(pLobbyRef);
        pLobbyRef = nullptr;
    }

    currentCommand = nullptr;
    disconnectCB = nullptr;
    disconnectContext = nullptr;
    groupStarted = false;
    raceResultsAccepted = false;
    sendCDKey = false;
}

void LobbyCore::AbortCurrentCommand() {
    if (currentCommand) {
        if (currentCommand->lobbyCallbackID != -1) {
            LobbyApiCancelCB(pLobbyRef);
        }
        delete currentCommand;
        currentCommand = nullptr;
    }
}

void LobbyCore::GlobalConnStatusCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
    bool boMutexLocked = false;
    if (!lobbyMutex.IsLocked()) {
        lobbyMutex.Lock(nullptr);
        boMutexLocked = true;
    }
    if (pMsg->kind == 'disc') {
        static_cast<LobbyCore *>(pData)->AbortAllCommands_HaveMutex();
        if (static_cast<LobbyCore *>(pData)->disconnectCB) {
            static_cast<LobbyCore *>(pData)->disconnectCB(LobbyCoreN::DISC_DISCONNECTED, pMsg,
                                                          static_cast<LobbyCore *>(pData)->disconnectContext);
        }
    }
    if (boMutexLocked) {
        lobbyMutex.Unlock(nullptr);
    }
}

void LobbyCore::GlobalResponseCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
    LobbyCore *lobbyCore = static_cast<LobbyCore *>(pData);
    for (GlobalCB *cb = lobbyCore->globalCBList[LOBBYAPI_CBTYPE_RESP].GetHead();
         cb != lobbyCore->globalCBList[LOBBYAPI_CBTYPE_RESP].EndOfList(); cb = cb->GetNext()) {
        cb->cbFunc(pRef, pMsg, cb->context);
    }
    LobbyApiDebug(pRef, pMsg);
}

void LobbyCore::GlobalEventCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
    LobbyCore *lobbyCore = static_cast<LobbyCore *>(pData);
    for (GlobalCB *cb = lobbyCore->globalCBList[LOBBYAPI_CBTYPE_EVNT].GetHead();
         cb != lobbyCore->globalCBList[LOBBYAPI_CBTYPE_EVNT].EndOfList(); cb = cb->GetNext()) {
        cb->cbFunc(pRef, pMsg, cb->context);
    }
    LobbyApiDebug(pRef, pMsg);
}

void LobbyCore::RankCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
    LobbyCore *lobbyCore = static_cast<LobbyCore *>(pData);
    lobbyCore->raceResultsAccepted = true;
    if (pMsg->code != 0) {
        lobbyCore->raceResultsAccepted = false;
    }
    raceResults.Clear();
    lobbyCore->FinishCommand(pMsg, true);
}

void LobbyCore::DefaultCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
    Instance().FinishCommand(pMsg, true);
}

LobbyCore::LobbyCommand::LobbyCommand(int commandKind, const char *request, bool commandGroup, LobbyApiCallbackT *callback,
                                      void *callbackContext, CommandCBFunc completionCallback, void *completionContext) {
    static unsigned int cmdID = 0;
    static unsigned int grpID = 0;

    kind = commandKind;
    lobbyCallbackID = -1;
    cmdID++;
    lobbyCB = callback;
    lobbyContext = callbackContext;
    commandCB = completionCallback;
    commandContext = completionContext;
    req = nullptr;
    commandID = cmdID;
    if (commandID < 1) {
        commandID = 1;
    }
    if (commandGroup == true) {
        if (grpID == 0) {
            grpID = commandID;
        }
        groupID = grpID;
    } else if (grpID != 0) {
        grpID = 0;
    }
    if (request && *request) {
        req = new ("d:/p4_apex1666_d1001856/mw/speed/indep/src/online/LobbyCore.cpp", 0x5bf, 8) char[bStrLen(request) + 1];
        bStrCpy(req, request);
    }
}

void LobbyCore::RaceResults::Clear() {
    raceResults.auth[0] = '\0';
    raceResults.when = 0;
    raceResults.rept[0] = '\0';
    for (int i = 3; i >= 0; i--) {
        raceResults.name[i][0] = '\0';
    }
    raceResults.results[0] = '\0';
}
