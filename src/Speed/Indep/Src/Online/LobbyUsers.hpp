#ifndef ONLINE_LOBBY_USERS_HPP
#define ONLINE_LOBBY_USERS_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "LobbyCore.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

namespace Physics {
namespace Info {
struct Performance;
}
}

struct PlayerDataT {
    struct Stats {
        struct OnlineModeStats {
            uint32 points;
            uint32 clientWins;
            uint32 clientLosses;
            uint32 hostWins;
            uint32 hostLosses;
            uint32 topSpeed;
            uint32 jumpTime;
            uint32 avgNOSused;
            uint32 totalNOSused;
            uint32 fastestFinishTime;
            uint32 winPercentage;

            OnlineModeStats() { Clear(); }
            ~OnlineModeStats() {}

            void Clear() {
                winPercentage = 0;
                fastestFinishTime = 0;
                totalNOSused = 0;
                avgNOSused = 0;
                jumpTime = 0;
                topSpeed = 0;
                hostLosses = 0;
                hostWins = 0;
                clientLosses = 0;
                clientWins = 0;
                points = 0;
            }
        };

        uint32 disqualifications;
        uint32 clientDisconnects;
        uint32 hostDisconnects;
        uint32 topSpeed;
        uint32 longestPowerSlideDistance;
        uint32 longestJumpDuration;
        uint32 fastestDragFinish;
        uint32 totalNOSusage;
        OnlineModeStats raceModeStats[3];

        Stats()
            : disqualifications(0) //
            , clientDisconnects(0) //
            , hostDisconnects(0) //
            , topSpeed(0) //
            , longestPowerSlideDistance(0) //
            , longestJumpDuration(0) //
            , fastestDragFinish(0) //
            , totalNOSusage(0) {}
        ~Stats() {}

        void Clear();
    };

    int ExtractRecord(const char *record);
    int ExtractRecord(const LobbyApiUserT &user);
    uint32 GetTotalGamesPlayedAsClient();
    uint32 GetTotalGamesPlayedAsHost();
    uint32 GetTotalGamesPlayed(int mode);
    uint32 GetTotalWinsAsClient();
    uint32 GetTotalWinsAsHost();
    uint32 GetTotalWins(int mode);
    uint32 GetTotalLossesAsClient();
    uint32 GetTotalLossesAsHost();
    uint32 GetTotalLosses(int mode);
    uint32 GetWeeklyPointTotals(int mode) {
        if (static_cast<uint32>(mode) > 2) {
            return 0;
        }
        return stats.raceModeStats[mode].points - lastWeekStats.raceModeStats[mode].points;
    }
    uint32 GetMonthlyPointTotals(int mode);
    float GetWinPercentage(int mode);
    float GetClientDisconnectPercentage() {
        uint32 gamesPlayedAsClient = GetTotalGamesPlayedAsClient();
        if (gamesPlayedAsClient != 0) {
            return (static_cast<float>(stats.clientDisconnects) /
                    static_cast<float>(gamesPlayedAsClient)) *
                   100.0f;
        }
        return 0.0f;
    }
    float GetHostDisconnectPercentage() {
        uint32 gamesPlayedAsHost = GetTotalGamesPlayedAsHost();
        if (gamesPlayedAsHost != 0) {
            return (static_cast<float>(stats.hostDisconnects) /
                    static_cast<float>(gamesPlayedAsHost)) *
                   100.0f;
        }
        return 0.0f;
    }
    float GetTotalDisconnectPercentage();
    float GetTopSpeedInMPS(int mode);
    float GetFastestDragFinish() {
        return static_cast<float>(stats.fastestDragFinish) * 0.001f;
    }
    char *GetFastestDragFinishString() {
        static char time[32];
        Timer tmptime(GetFastestDragFinish());
        tmptime.PrintToString(time, 4);
        return time;
    }
    uint32 GetTotalNOS(int mode);
    float GetLongestJumpDuration(int mode);
    char *GetLongestJumpDurationString() {
        static char time[32];
        Timer tmptime(static_cast<float>(stats.longestJumpDuration) * 0.001f);
        tmptime.PrintToString(time, 4);
        return time;
    }
    float GetLongestPowerSlideInMetres() {
        return static_cast<float>(stats.longestPowerSlideDistance) * 0.001f;
    }
    void Clear();

  private:
    bool IsFloatEqual(float x, float y, float epsilon) { return bAbs(x - y) < epsilon; }

  public:
    char persona[16];
    Stats stats;
    Stats lastWeekStats;
    Stats lastMonthStats;
    uint32 lastGameTime;

    PlayerDataT()
        : lastGameTime(0) {
        persona[0] = '\0';
    }
    ~PlayerDataT() {}
};

struct LobbyUsers {
    struct OnlineUsersData : bTNode<OnlineUsersData> {
        OnlineUsersData()
            : commandID(0) {
            bMemSet(&user, 0, sizeof(user));
        }
        ~OnlineUsersData() {}

        void Reset() {
            bMemSet(&user, 0, sizeof(user));
            commandID = 0;
        }

        LobbyApiUserT user;
        int32 commandID;
    };

    static LobbyUsers &Instance();
    void UpdateCarName();
    int32 GetUserOnlineRecord(const char *persona, CommandCBFunc func, void *context);
    LobbyApiUserT *GetUserRecord(const char *persona);
    bool GetCarNameFromUserRecord(const char *persona, uint32 &carNameHash,
                                  Physics::Info::Performance &performance);
    bool GetPointsFromUserRecord(const char *persona, uint32 &points, OnlineRaceModeE mode);
    bool GetDisqualificationsFromUserRecord(const char *persona, uint32 &disqs);
    int32 GetMyUserStats(PlayerDataT &userStats, CommandCBFunc func, void *context);
    void ClearUserStats();
    void ClearUserOnlineRecordCache();
    void ClearUserOnlineRecordCache(const LobbyApiPlayT &game);
    void SetSessionChangeFlag(bool changing);
    void SetSessionLatency(int latency);
    void SetSessionRaceStatusInfo(int lap, int mapx, int mapy);
    LobbyApiUserT *GetMyUserRecord() const;

  private:
    LobbyUsers()
        : gotMyStats(false) //
        , auxiData(nullptr) //
        , mLate(-1) //
        , mCar(-1) //
        , mLap(0) //
        , mMapX(0) //
        , mMapY(0) {}
    ~LobbyUsers() {}
    int32 Init();
    void Reset();
    void MaybeCreateAuxiBuffer();
    int32 SendAuxiData();
    static void OnlnCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    static void UserCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    friend int32 LobbyInit();
    friend void LobbyDisconnect();

    bTList<OnlineUsersData> userList;
    PlayerDataT myStats;
    bool gotMyStats;
    char *auxiData;
    int mLate;
    uint32 mCar;
    int mLap;
    int mMapX;
    int mMapY;
};

extern LobbyUsers *pLobbyUsersInstance;

#endif
