#ifndef ONLINE_LOBBY_GAMES_HPP
#define ONLINE_LOBBY_GAMES_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "LobbyCore.hpp"

struct cOnlineSettings;

namespace LobbyGamesN {
struct _GameResultUpdateT {
    char name[8][20];
    bool final;
    int status[8];
    uint32 time;
    int score[8];
    int place[8];
    bool save[8];
    unsigned int reason[8];
    unsigned char points[8];
};
typedef _GameResultUpdateT GameResultUpdateT;
}

typedef void (*ResultUpdateCBFunc)(const LobbyGamesN::_GameResultUpdateT *, void *);
typedef void LobbyPingManagerCallbackT(DirtyAddrT *, unsigned int, void *);

struct OnlineGame {
  private:
    bool m_isPassworded;
    char m_password[20];
    bool m_playermuted[8];

  public:
    void SetPlayerMuted(int index, bool muted) { m_playermuted[index] = muted; }
};

extern OnlineGame TheOnlineGame;

struct LobbyGames {
    LobbyGames();
    ~LobbyGames() { Reset(); }

    static LobbyGames &Instance();
    void SetResultUpdateCB(ResultUpdateCBFunc func, void *context);
    int32 StartGame(CommandCBFunc startGameCB, void *context, bool kickUnready);
    int32 UpdateGame(CommandCBFunc updateGameCB, void *context);
    inline int32 LockGame(CommandCBFunc lockGameCB, void *context);
    int32 UnlockGame(CommandCBFunc unlockGameCB, void *context);
    LobbyApiPlayT *GetMyGame() const;
    inline int PingPlayer(const char *persona, LobbyApiPlayT *game, LobbyPingManagerCallbackT *func, void *context);
    inline int PingPlayer(int index, LobbyApiPlayT *game, LobbyPingManagerCallbackT *func, void *context);
    inline void AbortCommand(int32 commandID);

  private:
    int32 Init();
    void Reset();
    inline int DoPing(int index, LobbyApiPlayT *game, LobbyPingManagerCallbackT *func, void *context);
    int32 CreateGame(const char *gameName, cOnlineSettings &raceOptions, CommandCBFunc createGameCB, void *context);
    int32 JoinGame(const char *gameName, const char *password, CommandCBFunc joinGameCB, void *context);
    int32 LeaveGame(CommandCBFunc leaveGameCB, void *context);
    inline int32 KickPlayer(const char *name);
    static inline void GamesDispListCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    static void GameCreatedCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    static void LeaveGameCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    static void GlobalEventCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
    friend class LobbyCore;
    friend struct LobbyGameSessions;

    ResultUpdateCBFunc resultUpdateCB;
    void *resultContext;
    LobbyApiPlayT myCurrentGame;
};

#endif
