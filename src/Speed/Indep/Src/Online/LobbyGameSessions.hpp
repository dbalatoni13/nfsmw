#ifndef ONLINE_LOBBY_GAME_SESSIONS_HPP
#define ONLINE_LOBBY_GAME_SESSIONS_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "LobbyCore.hpp"
#include "OnlineCfg.hpp"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"

enum YesNoAny {
    YES = 0,
    ON = 0,
    NO = 1,
    OFF = 1,
    ANY = 2
};

enum SortField {
    SORT_PING = 0,
    SORT_SESSION_NAME = 1,
    SORT_GAME_MODE = 2,
    SORT_PLAYER_COUNT = 3,
    SORT_COURSE_NAME = 4,
    SORT_INVALID = 0x80000000
};

struct FilterGameSessionParamsT {
    FilterGameSessionParamsT(uint32 count, GRace::Type gameMode, YesNoAny rankedGames, YesNoAny collisionDetection,
                             YesNoAny performanceMatching, eOnlineDisconnectPerc disconnectPerc);
    FilterGameSessionParamsT(uint32 count, const char *sessionNameSubstring);

    uint32 count;
    GRace::Type gameMode;
    YesNoAny rankedGames;
    YesNoAny collisionDetection;
    YesNoAny performanceMatching;
    char sessionNameSubstr[17];
    eOnlineDisconnectPerc disconnectPerc;
};

struct ExtraSessionData {
    int32 pingToHostInMsec;
};

typedef UTL::Std::map<int, ExtraSessionData, _type_map> ExtraSessionDataMap;

struct LobbyApiUserSetMemberT {
    int iIdent;
    char strPers[16];
    unsigned int uFlags;
    int iGame;
    char strAux[132];
};
typedef LobbyApiUserSetMemberT GameSessionMember;

struct LobbyGameSessions {
    enum SessionStatusCode {
        SESSION_DELETED = 0,
        SESSION_CHANGED = 1,
        SESSION_LIST_CHANGED = 2,
        SESSION_KICKED = 3,
        GAME_STARTED = 4
    };

    typedef void (*SessionUpdateCBFunc)(SessionStatusCode, const LobbyApiUserSetT *, void *);

    LobbyGameSessions();
    ~LobbyGameSessions() { Reset(); }
    static LobbyGameSessions &Instance();
    int32 CreateSession(const char *sessionName, CommandCBFunc createSessionCB, void *context);
    int32 JoinSession(const char *sessionName, const char *password, CommandCBFunc joinSessionCB, void *context);
    void CreateGameInSession();
    int32 LeaveSession(CommandCBFunc leaveSessionCB, void *context);
    inline int32 KickPlayer(const char *name, CommandCBFunc kickUserCB, void *context);
    void SettingsHaveChanged();
    void StartSessionChanges();
    void FinishSessionChanges();
    void SetSessionLatency(int late);
    void SetSessionRaceStatusInfo(int lap, int mapx, int mapy);
    bool IsMemberMakingChanges(int32 index);
    int GetMemberLatency(int32 index);
    void GetMemberRaceStatus(int32 index, int &lap, int &mapx, int &mapy);
    int8 GetSecsBeforeHostCanStart();
    int32 FindSessions(const FilterGameSessionParamsT &filterParams, CommandCBFunc filterSessionsCB, void *context);
    GameSession *GetMySession();
    void SendUpdateCallback(SessionStatusCode status);

  private:
    int32 Init();
    void Reset();
    void Suspend();
    void Resume();
    int32 LeaveSession_HaveMutex(CommandCBFunc leaveSessionCB, void *context);
    GameSessionMember *GetMemberByIndex_HaveMutex(int32 index);
    bool IsMemberMakingChanges_HaveMutex(int32 index);
    int GetMemberLatency_HaveMutex(int32 index);
    void UpdateSessionParams(char *params, int size);
    int32 UpdateSessionInfo(bool forceUpdate);
    void UpdateSessionFlags(uint32 &flags);
    static void SessionDispListCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    static void SessionMembersDispListCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    static void CreateSessionCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    static void JoinSessionCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    static void RecreateGameCB(LobbyApiMsgT *pMsg, void *pData);
    static void FindSessionsCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    static int SortFunc(void *sortref, int sortcon, void *recptr1, void *recptr2);
    static int FilterFunc(void *filtref, int filtcon, void *recptr);
    static void GlobalEventCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
    friend class LobbyCore;

    ExtraSessionDataMap extraSessionDataMap;
    DispListRef *sessionList;
    DispListRef *sessionMembers;
    SessionUpdateCBFunc sessionUpdateCB;
    void *updateContext;
    LobbyApiUserSetT myCurrentSession;
    uint32 currentSortParams;
    uint32 lastSearchCount;
    CommandCBFunc createSessionCB;
    void *createSessionContext;
    Timer hostStartSessionTimer;
    Timer hostHurryTimer;
    Timer hostInactiveTimer;

    static bool mHurryTimerStarted;
    static bool mHurryTimerStopped;
};

#endif
