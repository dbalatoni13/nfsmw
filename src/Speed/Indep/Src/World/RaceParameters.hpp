#ifndef WORLD_RACEPARAMETERS_H
#define WORLD_RACEPARAMETERS_H

#include "CarInfo.hpp"
#include "Speed/Indep/Src/Misc/Replay.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "./World.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

enum eTrackDirection {
    eDIRECTION_FORWARD,
    eDIRECTION_BACKWARD,
    NUM_TRACK_DIRECTIONS,
};

enum eTrafficDensity {
    eTRAFFICDENSITY_OFF,
    eTRAFFICDENSITY_LOW,
    eTRAFFICDENSITY_MEDIUM,
    eTRAFFICDENSITY_HIGH,
    NUM_TRAFFIC_DENSITIES,
};

enum RaceTypes {
    RACE_TYPE_NONE,
    RACE_TYPE_SINGLE_RACE,
    RACE_TYPE_TIME_TRIAL,
    RACE_TYPE_LAP_KNOCKOUT,
    RACE_TYPE_RACE_KNOCKOUT,
    RACE_TYPE_TOURNAMENT,
    RACE_TYPE_CAR_SHOW,
    RACE_TYPE_GET_AWAY,
};

enum eHandlingMode {
    HANDLING_MODE_CLASSIC,
    HANDLING_MODE_EXTREME,
};

enum eOpponentStrength {
    eOPPONENTSTRENGTH_LOW,
    eOPPONENTSTRENGTH_MEDIUM,
    eOPPONENTSTRENGTH_HIGH,
    NUM_OPPONENT_STRENGTHS,
};

enum eAIDifficultyModifier {
    eAI_DIFFICULTY_MODIFIER_EASY,
    eAI_DIFFICULTY_MODIFIER_SOMEWHAT_EASY,
    eAI_DIFFICULTY_MODIFIER_NORMAL,
    NUM_AI_DIFFICULTY_MODIFIERS,
};

// total size: 0xA0
struct RaceParameters {
    void InitWithDefaults();
    void DoSnapshot(ReplaySnapshot *snapshot);
    void AddDriverInfo(DriverInfo &driver_info);
    void RemoveDriverInfo(int driver_number);
    void SetDriverInfo(DriverInfo &driver_info);
    DriverInfo *GetDriverInfo(int n);
    DriverInfo *GetDriverInfoByDriverNumber(int nDriverNumber);
    DriverInfo *GetDriverInfoByPlayerNumber(int player_number);
    int GetDriverNumber(int player_number);
    DriverInfo *EliminateDriver(int nDriverNumber, int nRank);
    void ClearEliminatedDrivers();
    int GetNumEliminated();
    void ResetStartingPositions();
    bool AreThereTooManyUniqueCarGeometries(RideInfo *ride_info);
    void Print();

    inline bool IsDriftRace() {
        return ((this->bDriftRaceFlag) || (g_tweakIsDriftRace));
    }

    int TrackNumber;                            // offset 0x0, size 0x4
    eTrackDirection TrackDirection;             // offset 0x4, size 0x4
    eTrafficDensity TrafficDensity;             // offset 0x8, size 0x4
    float TrafficOncoming;                      // offset 0xC, size 0x4
    bool AIDemoMode;                            // offset 0x10, size 0x1
    bool ReplayDemoMode;                        // offset 0x14, size 0x1
    RaceTypes RaceType;                         // offset 0x18, size 0x4
    int Point2Point;                            // offset 0x1C, size 0x4
    float RollingStartSpeed;                    // offset 0x20, size 0x4
    int NumLapsInRace;                          // offset 0x24, size 0x4
    int NumPlayerCars;                          // offset 0x28, size 0x4
    int NumAICars;                              // offset 0x2C, size 0x4
    int NumOnlinePlayerCars;                    // offset 0x30, size 0x4
    int NumOnlineAICars;                        // offset 0x34, size 0x4
    signed char PlayerStartPosition[2];         // offset 0x38, size 0x2
    bool DamageEnabled;                         // offset 0x3C, size 0x1
    eHandlingMode HandlingMode;                 // offset 0x40, size 0x4
    int FinishLineNumber;                       // offset 0x44, size 0x4
    bool bDragRaceFlag;                         // offset 0x48, size 0x1
    bool bDriftRaceFlag;                        // offset 0x4C, size 0x1
    bool bBurnoutFlag;                          // offset 0x50, size 0x1
    bool bShortRaceFlag;                        // offset 0x54, size 0x1
    bool bOnlineRace;                           // offset 0x58, size 0x1
    bool bCarShowFlag;                          // offset 0x5C, size 0x1
    bool bGamebreakerOn;                        // offset 0x60, size 0x1
    int PlayerJoyports[2];                      // offset 0x64, size 0x8
    int nMaxCops;                               // offset 0x6C, size 0x4
    eOpponentStrength CopStrength;              // offset 0x70, size 0x4
    float DriftOpponentScoreMultiplier;         // offset 0x74, size 0x4
    eOpponentStrength OpponentStrength;         // offset 0x78, size 0x4
    int Boost;                                  // offset 0x7C, size 0x4
    float BoostScale[2];                        // offset 0x80, size 0x8
    eAIDifficultyModifier AIDifficultyModifier; // offset 0x88, size 0x4
    int PlayerDriverNumber[2];                  // offset 0x8C, size 0x8
    int NumDriverInfo;                          // offset 0x94, size 0x4
    Timer TimeTrialTime;                        // offset 0x98, size 0x4
    bool bCareerEventRace;                      // offset 0x9C, size 0x1
};

#endif
