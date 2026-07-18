#ifndef ONLINE_LOBBY_CORE_HPP
#define ONLINE_LOBBY_CORE_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"

struct LobbyApiRefT;
struct LobbyApiMsgT;
struct LobbyPingManagerRefT;
struct ConnApiRefT;
struct ConnApiCbInfoT;

typedef void ConnApiCallbackT(ConnApiRefT *, ConnApiCbInfoT *, void *);
typedef void LobbyApiCallbackT();
typedef void (*CommandCBFunc)();

int32 LobbyInit();
void LobbyDisconnect();

struct LobbyGames {
    static LobbyGames &Instance();

  private:
    int32 Init();
    void Reset();
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
};

struct LobbyGameSessions {
    static LobbyGameSessions &Instance();

  private:
    int32 Init();
    void Reset();
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
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

    typedef void (*DisconnectCBFunc)();

    LobbyCore();
    ~LobbyCore();

    static LobbyCore &Instance();
    void DoProcessing();

  private:
    int32 Init();
    void Reset();
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
