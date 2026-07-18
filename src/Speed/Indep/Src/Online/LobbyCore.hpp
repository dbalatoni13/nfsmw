#ifndef ONLINE_LOBBY_CORE_HPP
#define ONLINE_LOBBY_CORE_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"

struct LobbyApiRefT;
struct LobbyPingManagerRefT;
struct ConnApiRefT;
struct ConnApiCbInfoT;

typedef void ConnApiCallbackT(ConnApiRefT *, ConnApiCbInfoT *, void *);

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
    struct GlobalCB;
    struct LobbyCommand;
    struct RaceResults;

    typedef void (*DisconnectCBFunc)();

    LobbyCore();
    ~LobbyCore();

    static LobbyCore &Instance();
    void DoProcessing();

  private:
    int32 Init();
    void Reset();
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
