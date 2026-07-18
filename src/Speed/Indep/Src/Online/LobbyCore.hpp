#ifndef ONLINE_LOBBY_CORE_HPP
#define ONLINE_LOBBY_CORE_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"

struct LobbyApiRefT;
struct LobbyApiMsgT;
struct LobbyApiServerStatT;
struct LobbyPingManagerRefT;
struct ConnApiRefT;
struct ConnApiCbInfoT;

typedef void ConnApiCallbackT(ConnApiRefT *, ConnApiCbInfoT *, void *);
typedef void LobbyApiCallbackT(LobbyApiRefT *, LobbyApiMsgT *, void *);
typedef void (*CommandCBFunc)(LobbyApiMsgT *, void *);

namespace LobbyCoreN {
enum OnlineDisconnectReason {
    DISC_DISCONNECTED = 0,
    DISC_SERVER_DOWN = 1,
    DISC_IDLE_TIMEOUT = 2,
    DISC_MAX_TIMEOUT = 3,
    DISC_NEED_UPDATE = 4
};
}

struct LobbyApiUserSetT {
    int iIdent;
    int iType;
    uint32 uSysFlags;
    uint32 uCustFlags;
    char strOwner[16];
    int iSize;
    int iCount;
    char strName[36];
    char strDesc[68];
    char strParams[68];
};

typedef LobbyApiUserSetT GameSession;

int32 LobbyInit();
void LobbyDisconnect();

struct LobbyGames {
    static LobbyGames &Instance();

  private:
    int32 Init();
    void Reset();
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
    friend class LobbyCore;
};

struct LobbyGameSessions {
    static LobbyGameSessions &Instance();
    GameSession *GetMySession();

  private:
    int32 Init();
    void Reset();
    void Suspend();
    void Resume();
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
    friend class LobbyCore;
};

struct LobbyChat {
    static LobbyChat &Instance();

  private:
    int32 Init();
    void Reset();
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
};

struct LobbyRanks {
    static LobbyRanks &Instance();

  private:
    int32 Init();
    void Reset();
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
};

struct LobbyUsers {
    static LobbyUsers &Instance();

  private:
    int32 Init();
    void Reset();
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
};

struct LobbyAccount {
    static LobbyAccount &Instance();

  private:
    int32 Init();
    void Reset();
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
};

struct LobbyRooms {
    static LobbyRooms &Instance();

  private:
    int32 Init();
    void Reset();
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
    friend class LobbyCore;
};

struct ConnectionCore {
    static ConnectionCore &Instance();
    void Init(int maxNumPlayers, ConnApiCallbackT *cbfunc, void *context);
    void Reset();
};

class LobbyCore {
  public:
    struct GlobalCB : bTNode<GlobalCB> {
        LobbyApiCallbackT *cbFunc;
        void *context;
    };

    struct LobbyCommand : bTNode<LobbyCommand> {
        LobbyCommand(int commandKind, const char *request, bool commandGroup, LobbyApiCallbackT *callback, void *callbackContext,
                     CommandCBFunc completionCallback, void *completionContext);

        int kind;
        char *req;
        LobbyApiCallbackT *lobbyCB;
        void *lobbyContext;
        CommandCBFunc commandCB;
        void *commandContext;
        int32 commandID;
        int32 groupID;
        int lobbyCallbackID;
    };

    struct RaceResults {
        char auth[64];
        uint64 when;
        char rept[16];
        char name[4][16];
        char results[1024];
    };

    typedef void (*DisconnectCBFunc)(LobbyCoreN::OnlineDisconnectReason, LobbyApiMsgT *, void *);

    LobbyCore();
    ~LobbyCore();

    static LobbyCore &Instance();
    void DoProcessing();
    int32 SetAsyncServerMessages();
    void SaveRaceResults(const char *results);
    int32 SendSavedRaceResults(CommandCBFunc func, void *context);
    LobbyApiServerStatT *GetServerStats();
    void AbortCommand(int32 commandID);
    void AbortCommand(CommandCBFunc commandCB, bool abortAll);
    void AbortGroup(int32 commandID);
    void AbortAllCommands();
    void SetDisconnectCallback(DisconnectCBFunc func, void *context);
    char *GetServerConfig();
    LobbyApiRefT *GetLobbyApiRef();
    void Suspend();
    void Resume(bool fullResume);

  private:
    int32 Init();
    void Reset();
    int32 SetAsyncServerMessages_HaveMutex();
    int32 QueueCommand(int kind, const char *request, LobbyApiCallbackT *lobbyCB, void *lobbyContext,
                       CommandCBFunc commandCB, void *commandContext, bool makeNextCommand);
    int32 FindCommandID(int kind, LobbyApiCallbackT *lobbyCB, void *lobbyContext, CommandCBFunc commandCB, void *commandContext);
    static void RankCB(LobbyApiRefT *lobbyRef, LobbyApiMsgT *msg, void *context);
    void AbortCurrentCommand();
    void AbortAllCommands_HaveMutex();
    void FinishCommand(LobbyApiMsgT *msg, bool doCallback);
    friend int32 LobbyInit();
    friend void LobbyDisconnect();

    bTList<GlobalCB> globalCBList[6];
    bTList<LobbyCommand> lobbyCommandQueue;
    LobbyApiRefT *pLobbyRef;
    LobbyPingManagerRefT *pingManagerRef;
    LobbyCommand *currentCommand;
    static RaceResults raceResults;
    DisconnectCBFunc disconnectCB;
    void *disconnectContext;
    bool groupStarted;
    bool raceResultsAccepted;
    bool cheatResponseActive;
    char cheatResponse[33];
    bool sendCDKey;
    bool mIsSuspended;
};

#endif
