#ifndef ONLINE_LOBBY_LOGIN_HPP
#define ONLINE_LOBBY_LOGIN_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct LobbyApiRefT;
struct LobbyLoginRefT;
struct LobbyAccount;

enum LobbyLoginAlertE {
    LOBBYLOGIN_ALERT_ERROR = 0,
    LOBBYLOGIN_ALERT_DB_ERROR = 1,
    LOBBYLOGIN_ALERT_FILTER = 2,
    LOBBYLOGIN_ALERT_INV_MASTER = 3,
    LOBBYLOGIN_ALERT_MISSING_PARAM = 4,
    LOBBYLOGIN_ALERT_ALREADY_LOGGED_IN = 5,
    LOBBYLOGIN_ALERT_LOCKED = 6,
    LOBBYLOGIN_ALERT_RESERVED = 7,
    LOBBYLOGIN_ALERT_CONNTIMEOUT = 8,
    LOBBYLOGIN_ALERT_SERVERDOWN = 9,
    LOBBYLOGIN_ALERT_DISC = 10,
    LOBBYLOGIN_ALERT_CHEAT_DEVICE = 11,
    LOBBYLOGIN_ALERT_DASHBOARD = 12,
    LOBBYLOGIN_ALERT_PERSONA_SET = 13,
    LOBBYLOGIN_ALERT_INV_PASS = 14,
    LOBBYLOGIN_ALERT_INV_REGKEY = 15,
    LOBBYLOGIN_NUMALERTS = 16,
};

enum LobbyLoginContextE {
    LOBBYLOGIN_CONTEXT_INVALID = -1,
    LOBBYLOGIN_CONTEXT_OFFLINE = 0,
    LOBBYLOGIN_CONTEXT_CONNECT = 1,
    LOBBYLOGIN_CONTEXT_LOGIN = 2,
    LOBBYLOGIN_CONTEXT_TOS = 3,
    LOBBYLOGIN_CONTEXT_SHARE = 4,
    LOBBYLOGIN_CONTEXT_SELPERS = 5,
    LOBBYLOGIN_CONTEXT_DONE = 6,
    LOBBYLOGIN_NUMCONTEXTS = 7,
    LOBBYLOGIN_CONTEXT_PREVIOUS = 4096,
};

enum LobbyLoginStatusE {
    LOBBYLOGIN_STATUS_IDLE = 0,
    LOBBYLOGIN_STATUS_NETWORK = 1,
    LOBBYLOGIN_STATUS_ALERT = 2,
    LOBBYLOGIN_STATUS_GOTO = 3,
    LOBBYLOGIN_STATUS_OFFLINE = 4,
    LOBBYLOGIN_NUMSTATUS = 5,
};

struct LobbyLoginNameListT {
    char strNames[4][32];
    int iNumNames;
};

struct LobbyLoginAlertT {
    int uFlags;
    char *pTitle;
    char *pBody;
    char *pFooter;
};

class LobbyLogin {
  public:
    LobbyLogin();
    ~LobbyLogin();

    static LobbyLogin &Instance();
    bool Start(const LobbyApiRefT *lobbyRef, const LobbyLoginAlertT *alertTable, LobbyLoginContextE startContext,
               LobbyLoginNameListT *autoLoginName, char *autoLoginParams);
    void Finish();
    bool IsStarted();
    bool IsFinished();
    bool DoContextConnect(const char *ipaddr, uint16 port, int timeoutMsec);
    bool DoContextLogin(const char *username, const char *password);
    bool DoContextTOS(bool acceptedTOS);
    bool DoContextShare(bool wantsEASpam, bool wantsEmailShared);
    bool DoContextSelPers(const char *persona);
    LobbyLoginContextE GetContext();
    LobbyLoginStatusE GetStatus();
    LobbyLoginNameListT &GetPersonaList();
    bool IsAlertSet();
    LobbyLoginAlertT *GetAlert(LobbyLoginContextE context);
    LobbyLoginAlertE GetAlertEnum(LobbyLoginContextE context);
    bool IsRetryAttempt();

  private:
    bool DoContext(LobbyLoginContextE context, char *params);
    void AddPersona(char *persona);
    void DeletePersona(char *persona);
    static void ContextLoginCB(LobbyLoginRefT *pLogin, LobbyLoginContextE context, LobbyLoginStatusE status, void *callbackData);
    friend struct LobbyAccount;

    LobbyLoginRefT *lobbylogin;
    bool doingRetryAttempt;
    LobbyLoginNameListT personas;
};

#endif
