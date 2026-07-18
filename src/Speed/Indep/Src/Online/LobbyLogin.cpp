#include "LobbyLogin.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Online/BuddyCore.hpp"
#include "Speed/Indep/Src/Online/LobbyCore.hpp"
#include "Speed/Indep/Src/Online/LobbyGames.hpp"
#include "Speed/Indep/Src/Online/OnlineCfg.hpp"

extern LobbyLoginAlertT LobbyLogin_DefaultAlertTable[];

static OnlineMutex lobbyMutex("Lobby");

extern "C" {
LobbyLoginRefT *LobbyLoginCreate(const LobbyApiRefT *lobbyRef, const LobbyLoginAlertT *alertTable);
void LobbyLoginAbort(LobbyLoginRefT *login, LobbyLoginContextE context);
void LobbyLoginAutomaticCB(LobbyLoginRefT *login, LobbyLoginNameListT *nameList, int listType, void *callback, void *callbackData);
void LobbyLoginSetContextCB(LobbyLoginRefT *login, LobbyLoginContextE context, void *callback, void *callbackData);
void LobbyLoginDestroy(LobbyLoginRefT *login);
LobbyLoginContextE LobbyLoginGetContext(LobbyLoginRefT *login);
LobbyLoginStatusE LobbyLoginGetStatus(LobbyLoginRefT *login, LobbyLoginContextE context);
LobbyLoginAlertT *LobbyLoginAlert(LobbyLoginRefT *login, LobbyLoginContextE context);
LobbyLoginAlertE LobbyLoginGetAlertEnum(LobbyLoginRefT *login, LobbyLoginAlertT *alert);
LobbyLoginNameListT *LobbyLoginGetNameList(LobbyLoginRefT *login, int listType);
void TagFieldSetString(char *buffer, int bufferSize, const char *name, const char *value);
void TagFieldSetNumber(char *buffer, int bufferSize, const char *name, int value);
int LobbyLoginSubmitCB(LobbyLoginRefT *login, LobbyLoginContextE context, char *params,
                       void (*callback)(LobbyLoginRefT *, LobbyLoginContextE, LobbyLoginStatusE, void *), void *callbackData);
}

LobbyLogin::LobbyLogin()
    : lobbylogin(nullptr), //
      doingRetryAttempt(false) {
    bMemSet(&personas, 0, sizeof(personas));
}

LobbyLogin::~LobbyLogin() { Finish(); }

LobbyLogin &LobbyLogin::Instance() {
    static LobbyLogin the;
    return the;
}

bool LobbyLogin::Start(const LobbyApiRefT *lobbyRef, const LobbyLoginAlertT *alertTable, LobbyLoginContextE startContext,
                       LobbyLoginNameListT *autoLoginName, char *autoLoginParams) {
    bool rc;

    if (!alertTable) {
        alertTable = LobbyLogin_DefaultAlertTable;
    }
    if (IsStarted() == true) {
        LobbyLoginAbort(lobbylogin, GetContext());
    } else {
        lobbylogin = LobbyLoginCreate(lobbyRef, alertTable);
        if (!lobbylogin) {
            return false;
        }
    }

    doingRetryAttempt = false;
    bMemSet(&personas, 0, sizeof(personas));
    rc = true;
    if (autoLoginName) {
        LobbyLoginAutomaticCB(lobbylogin, autoLoginName, 1, 0, 0);
        rc = DoContext(startContext, autoLoginParams);
    } else {
        LobbyLoginSetContextCB(lobbylogin, startContext, 0, 0);
    }
    return rc;
}

void LobbyLogin::Finish() {
    if (IsStarted() == true) {
        LobbyLoginAbort(lobbylogin, GetContext());
        LobbyLoginDestroy(lobbylogin);
        lobbylogin = nullptr;
    }
}

bool LobbyLogin::IsStarted() { return lobbylogin != nullptr; }

bool LobbyLogin::DoContextConnect(const char *ipaddr, uint16 port, int timeoutMsec) {
    char params[128] = "";
    TagFieldSetString(params, sizeof(params), "NAME", ipaddr);
    TagFieldSetNumber(params, sizeof(params), "PORT", port);
    TagFieldSetNumber(params, sizeof(params), "TIME", timeoutMsec);
    return DoContext(LOBBYLOGIN_CONTEXT_CONNECT, params);
}

bool LobbyLogin::DoContextLogin(const char *username, const char *password) {
    char params[197] = "";
    TagFieldSetString(params, sizeof(params), "NAME", username);
    TagFieldSetString(params, sizeof(params), "PASS", password);
    TagFieldSetNumber(params, sizeof(params), "MULTIPERS", 1);
    return DoContext(LOBBYLOGIN_CONTEXT_LOGIN, params);
}

bool LobbyLogin::DoContextTOS(bool acceptedTOS) {
    char params[8] = "";
    TagFieldSetNumber(params, sizeof(params), "TOS", acceptedTOS);
    return DoContext(LOBBYLOGIN_CONTEXT_TOS, params);
}

bool LobbyLogin::DoContextSelPers(const char *persona) {
    char params[64];

    for (int i = 0; i < personas.iNumNames; i++) {
        if (bStrCmp(persona, personas.strNames[i]) == 0) {
            break;
        }
    }
    params[0] = '\0';
    bMemSet(params + 1, 0, sizeof(params) - 1);
    TagFieldSetString(params, sizeof(params), "PERS", persona);
    return DoContext(LOBBYLOGIN_CONTEXT_SELPERS, params);
}

LobbyLoginContextE LobbyLogin::GetContext() {
    LobbyLoginContextE context = LOBBYLOGIN_CONTEXT_INVALID;
    if (IsStarted() == true) {
        context = LobbyLoginGetContext(lobbylogin);
    }
    return context;
}

LobbyLoginStatusE LobbyLogin::GetStatus() { return LobbyLoginGetStatus(lobbylogin, GetContext()); }

LobbyLoginNameListT &LobbyLogin::GetPersonaList() { return personas; }

bool LobbyLogin::IsAlertSet() { return GetStatus() == LOBBYLOGIN_STATUS_ALERT; }

LobbyLoginAlertT *LobbyLogin::GetAlert(LobbyLoginContextE context) {
    if (context == LOBBYLOGIN_NUMCONTEXTS) {
        context = GetContext();
    }
    return LobbyLoginAlert(lobbylogin, context);
}

LobbyLoginAlertE LobbyLogin::GetAlertEnum(LobbyLoginContextE context) {
    return LobbyLoginGetAlertEnum(lobbylogin, GetAlert(context));
}

static void _LoginSubmitCallback(LobbyLoginRefT *pLogin, LobbyLoginContextE context, LobbyLoginStatusE status, void *callbackData) {}

bool LobbyLogin::DoContext(LobbyLoginContextE context, char *params) {
    char emptyParams[1] = "";

    if (GetContext() != context) {
        LobbyLoginSetContextCB(lobbylogin, context, 0, 0);
    }
    if (context == LOBBYLOGIN_CONTEXT_LOGIN) {
        return LobbyLoginSubmitCB(lobbylogin, LOBBYLOGIN_CONTEXT_LOGIN, params ? params : emptyParams, ContextLoginCB, this) != 0;
    }
    return LobbyLoginSubmitCB(lobbylogin, context, params ? params : emptyParams, _LoginSubmitCallback, this) != 0;
}

void LobbyLogin::AddPersona(char *persona) {
    if (IsStarted() == true && persona && *persona && personas.iNumNames < 4) {
        bStrCpy(personas.strNames[personas.iNumNames], persona);
        personas.iNumNames++;
    }
}

void LobbyLogin::DeletePersona(char *persona) {
    if (IsStarted() == true && persona && *persona && personas.iNumNames > 0) {
        bool foundIt = false;
        int i;
        for (i = 0; i < personas.iNumNames; i++) {
            if (bStrCmp(personas.strNames[i], persona) == 0) {
                personas.strNames[i][0] = '\0';
                foundIt = true;
                break;
            }
        }
        if (foundIt) {
            for (i++; i < personas.iNumNames; i++) {
                bStrCpy(personas.strNames[i - 1], personas.strNames[i]);
                personas.strNames[i][0] = '\0';
            }
            personas.iNumNames--;
        }
    }
}

void LobbyLogin::ContextLoginCB(LobbyLoginRefT *pLogin, LobbyLoginContextE context, LobbyLoginStatusE status,
                                void *callbackData) {
    LobbyLoginNameListT *personaList;

    if (status == LOBBYLOGIN_STATUS_GOTO && (personaList = LobbyLoginGetNameList(pLogin, 0))) {
        bMemCpy(&static_cast<LobbyLogin *>(callbackData)->personas, personaList, sizeof(LobbyLoginNameListT));
    }
}

char *OLGetProductName() { return "nfs"; }

char *OLGetProductYear() { return "2006"; }

char *OLGetPlatform() { return "ps2"; }

int32 LobbyInit() {
    LobbyCore::Instance();
    lobbyMutex.Lock("LobbyInit");

    if (LobbyCore::Instance().Init() >= 0 && LobbyGames::Instance().Init() >= 0 &&
        LobbyGameSessions::Instance().Init() >= 0 && LobbyChat::Instance().Init() >= 0 &&
        LobbyRanks::Instance().Init() >= 0 && LobbyUsers::Instance().Init() >= 0 &&
        LobbyAccount::Instance().Init() >= 0 && LobbyRooms::Instance().Init() >= 0) {
        ConnectionCore::Instance().Init(cOnlineSettings::MaxOnlinePlayers, nullptr, nullptr);
        lobbyMutex.Unlock("LobbyInit");
        return 0;
    }

    lobbyMutex.Unlock("LobbyInit");
    return -1;
}

void LobbyDisconnect() {
    lobbyMutex.Lock("LobbyDisconnect");
    LobbyGameSessions::Instance().Reset();
    LobbyGames::Instance().Reset();
    LobbyRooms::Instance().Reset();
    LobbyChat::Instance().Reset();
    LobbyRanks::Instance().Reset();
    LobbyUsers::Instance().Reset();
    LobbyAccount::Instance().Reset();
    if (!FEDatabase->IsLANMode()) {
        BuddyCore::instance()->disconnect();
    }
    ConnectionCore::Instance().Reset();
    LobbyCore::Instance().Reset();
    lobbyMutex.Unlock("LobbyDisconnect");
}
