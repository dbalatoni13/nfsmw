#ifndef ONLINE_LOBBY_USERS_HPP
#define ONLINE_LOBBY_USERS_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "LobbyCore.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

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
};

#endif
