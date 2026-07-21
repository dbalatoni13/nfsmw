#ifndef ONLINE_LOBBY_USERS_HPP
#define ONLINE_LOBBY_USERS_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "LobbyCore.hpp"

struct PlayerDataT {
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

        void Clear();
    };

    struct Stats {
        uint32 disqualifications;
        uint32 clientDisconnects;
        uint32 hostDisconnects;
        uint32 topSpeed;
        uint32 longestPowerSlideDistance;
        uint32 longestJumpDuration;
        uint32 fastestDragFinish;
        uint32 totalNOSusage;
        OnlineModeStats raceModeStats[3];

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
    uint32 GetWeeklyPointTotals(int mode);
    uint32 GetMonthlyPointTotals(int mode);
    float GetWinPercentage(int mode);
    float GetClientDisconnectPercentage();
    float GetHostDisconnectPercentage();
    float GetTotalDisconnectPercentage();
    float GetTopSpeedInMPS(int mode);
    float GetFastestDragFinish();
    char *GetFastestDragFinishString();
    uint32 GetTotalNOS(int mode);
    float GetLongestJumpDuration(int mode);
    char *GetLongestJumpDurationString();
    float GetLongestPowerSlideInMetres();
    void Clear();

  private:
    bool IsFloatEqual(float x, float y, float epsilon);

  public:
    char persona[16];
    Stats stats;
    Stats lastWeekStats;
    Stats lastMonthStats;
    uint32 lastGameTime;
};

#endif
