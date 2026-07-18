#ifndef ONLINE_LOBBY_RANKS_HPP
#define ONLINE_LOBBY_RANKS_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct DispListRef;
struct LobbyApiRefT;
struct LobbyApiMsgT;
struct LobbyApiRankT {
    int pts;
    int rank;
    unsigned int uFlags;
    char name[16];
    char stat[132];
};
typedef void (*CommandCBFunc)(LobbyApiMsgT *, void *);

enum RankListTypeE {
    RLT_INVALID = -1,
    RLT_TOP100 = 0,
    RLT_PERSONAL = 1,
    RLT_CURRENT_WEEK = 2,
    RLT_PERSONAL_WEEK = 3,
    RLT_CURRENT_MONTH = 4,
    RLT_PERSONAL_MONTH = 5
};

enum _RankListE {
    RL_INVALID = -1,
    RL_BOGUS = 0,
    RL_FIRST = 1,
    RL_FIRST_TIERED_LIST = 1,
    RL_CIRCUIT_BY_RATING = 1,
    RL_CIRCUIT_BY_WINS = 2,
    RL_CIRCUIT_BY_TOP_SPEED = 3,
    RL_CIRCUIT_BY_WIN_PERCENTAGE = 4,
    RL_SPRINT_BY_RATING = 5,
    RL_SPRINT_BY_WINS = 6,
    RL_SPRINT_BY_TOP_SPEED = 7,
    RL_SPRINT_BY_WIN_PERCENTAGE = 8,
    RL_DRAG_BY_RATING = 9,
    RL_DRAG_BY_WINS = 10,
    RL_DRAG_BY_TOP_SPEED = 11,
    RL_DRAG_BY_WIN_PERCENTAGE = 12,
    RL_DRAG_BY_FASTEST_FINISH = 13,
    RL_LAST_TIERED_LIST = 13,
    RL_FIRST_WEEK_LIST = 14,
    RL_CIRCUIT_WEEK_BEST_BY_RATING = 14,
    RL_CIRCUIT_WEEK_BEST_BY_WINS = 15,
    RL_CIRCUIT_WEEK_BEST_BY_TOP_SPEED = 16,
    RL_CIRCUIT_WEEK_BEST_BY_WIN_PERCENTAGE = 17,
    RL_SPRINT_WEEK_BEST_BY_RATING = 18,
    RL_SPRINT_WEEK_BEST_BY_WINS = 19,
    RL_SPRINT_WEEK_BEST_BY_TOP_SPEED = 20,
    RL_SPRINT_WEEK_BEST_BY_WIN_PERCENTAGE = 21,
    RL_DRAG_WEEK_BEST_BY_RATING = 22,
    RL_DRAG_WEEK_BEST_BY_WINS = 23,
    RL_DRAG_WEEK_BEST_BY_TOP_SPEED = 24,
    RL_DRAG_WEEK_BEST_BY_WIN_PERCENTAGE = 25,
    RL_DRAG_WEEK_BY_FASTEST_FINISH = 26,
    RL_LAST_WEEK_LIST = 26,
    RL_FIRST_MONTH_LIST = 27,
    RL_CIRCUIT_MONTH_BEST_BY_RATING = 27,
    RL_CIRCUIT_MONTH_BEST_BY_WINS = 28,
    RL_CIRCUIT_MONTH_BEST_BY_TOP_SPEED = 29,
    RL_CIRCUIT_MONTH_BEST_BY_WIN_PERCENTAGE = 30,
    RL_SPRINT_MONTH_BEST_BY_RATING = 31,
    RL_SPRINT_MONTH_BEST_BY_WINS = 32,
    RL_SPRINT_MONTH_BEST_BY_TOP_SPEED = 33,
    RL_SPRINT_MONTH_BEST_BY_WIN_PERCENTAGE = 34,
    RL_DRAG_MONTH_BEST_BY_RATING = 35,
    RL_DRAG_MONTH_BEST_BY_WINS = 36,
    RL_DRAG_MONTH_BEST_BY_TOP_SPEED = 37,
    RL_DRAG_MONTH_BEST_BY_WIN_PERCENTAGE = 38,
    RL_DRAG_MONTH_BY_FASTEST_FINISH = 39,
    RL_LAST_MONTH_LIST = 39,
    NUM_RANK_LISTS = 40
};
typedef _RankListE RankListE;

enum _OnlineRaceModeE {
    OL_RACE_MODE_INVALID = -1,
    OL_RACE_MODE_CIRCUIT = 0,
    OL_RACE_MODE_SPRINT = 1,
    OL_RACE_MODE_DRAG = 2,
    NUM_OL_RANKED_RACE_MODES = 3,
    NUM_OL_RACE_MODES = 3
};
typedef _OnlineRaceModeE OnlineRaceModeE;

enum _RankListSubcategoryE {
    RANKLIST_BY_RATING = 0,
    RANKLIST_BY_WINS = 1,
    RANKLIST_BY_TOP_SPEED = 2,
    RANKLIST_BY_WIN_PERCENTAGE = 3
};
typedef _RankListSubcategoryE RankListSubcategoryE;

struct LobbyRanks {
  public:
    enum RankListMapping {
        RLM_INVALID = 255
    };

    LobbyRanks();
    ~LobbyRanks() { Reset(); }
    static LobbyRanks &Instance();
    int32 GetRanks(RankListE rankList, CommandCBFunc func, void *context);
    inline int32 GetAllMonthRanks(CommandCBFunc func, void *context);
    inline int32 GetMonthRanks(OnlineRaceModeE mode, RankListTypeE type, CommandCBFunc func, void *context);
    inline int32 GetAllWeeksRanks(CommandCBFunc func, void *context);
    inline int32 GetWeekRanks(OnlineRaceModeE mode, RankListTypeE type, CommandCBFunc func, void *context);
    LobbyApiRankT *GetRankData(RankListE rankList, RankListTypeE type, int index);
    inline void AbortCommand(int32 commandID);
    void ClearCache();
    int GetRankListSize(RankListE rankList, RankListTypeE type);
    DispListRef *GetDisplayListReference(RankListE index);

  private:
    int32 Init();
    void Reset();
    static void SnapsDispListCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    static void SnapCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    static int RankSort(void *sortref, int sortcon, void *recptr1, void *recptr2);
    static RankListMapping RankList2SnapIndex(RankListE rankList, RankListTypeE type);
    static int RankList2DispListIndex(RankListE rankList, RankListTypeE type);
    static int RankListMapping2DispListIndex(RankListMapping mapping);
    static inline RankListTypeE RankListMapping2RankListType(RankListMapping mapping);
    static inline int InitTierArray();
    static RankListE GetRankListFromMode(OnlineRaceModeE raceMode, RankListSubcategoryE category, int weekOffset);
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
    friend class LobbyCore;

    DispListRef *rankDispLists[33];
    bool gotRankList[33];
    unsigned int snapsTotal[33];
    static unsigned int tierPoints[3][10];
};

#endif
