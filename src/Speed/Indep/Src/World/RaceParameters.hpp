#ifndef RACEPARAMETERS_HPP
#define RACEPARAMETERS_HPP

#include "CarInfo.hpp"
#include "World.hpp"
#include "Speed/Indep/Src/AI/AIBasics.hpp"
#include "Speed/Indep/Src/Misc/Replay.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

enum PlayerNumbers {
    PLAYER_1 = 0,
    PLAYER_2 = 1,
    MAX_NUM_PLAYERS = 2,
};

enum eTransmissionType {
    TRANSMISSION_AUTOMATIC = 0,
    TRANSMISSION_MANUAL = 1,
    TRANSMISSION_SPORT = 2,
    TRANSMISSION_MANUAL_CLUTCH = 3,
    NUM_TRANSMISSION_TYPES = 4,
};

enum eHandlingMode {
    HANDLING_MODE_CLASSIC = 0,
    HANDLING_MODE_EXTREME = 1,
};

enum eTrackDirection {
    eDIRECTION_FORWARD = 0,
    eDIRECTION_BACKWARD = 1,
    NUM_TRACK_DIRECTIONS = 2,
};

enum RaceTypes {
    RACE_TYPE_NONE = 0,
    RACE_TYPE_SINGLE_RACE = 1,
    RACE_TYPE_TIME_TRIAL = 2,
    RACE_TYPE_LAP_KNOCKOUT = 3,
    RACE_TYPE_RACE_KNOCKOUT = 4,
    RACE_TYPE_TOURNAMENT = 5,
    RACE_TYPE_CAR_SHOW = 6,
    RACE_TYPE_GET_AWAY = 7,
};

// total size: 0xA0
struct RaceParameters {
    RaceParameters() {
        InitWithDefaults();
    }

    void DoSnapshot(ReplaySnapshot *snapshot);
    void InitWithDefaults();
    void Print();
    void AddDriverInfo(DriverInfo &driver_info);
    void RemoveDriverInfo(int driver_number);
    void SetDriverInfo(DriverInfo &driver_info, int index);
    DriverInfo *GetDriverInfo(int n);
    DriverInfo *GetDriverInfoByDriverNumber(int nDriverNumber);
    DriverInfo *GetDriverInfoByPlayerNumber(int player_number);
    int GetDriverNumber(int player_number);
    void ResetStartingPositions();
    DriverInfo *EliminateDriver(int nDriverNumber, int nRank);
    void ClearEliminatedDrivers();
    int GetNumEliminated();
    bool AreThereTooManyUniqueCarGeometries(RideInfo *ride_info);
    int NumRacingCars() {}
    unsigned int Checksum();
    void ChooseHudTextures();
    bool IsFreeRun();
    Timer GetTimeTrialTime() {}
    bool IsTimeTrial() {}
    bool IsCareerEventRace() {}
    bool IsDragRace() {}
    inline bool IsDriftRace() {
        return this->bDriftRaceFlag || (g_tweakIsDriftRace != 0);
    }
    bool IsBurnout() {}
    bool IsShortTrackRace() {}
    bool IsDriftPhysics() {}
    bool IsBurnoutPhysics() {}
    bool IsTestTrack() {}
    bool IsSkidPad() {}
    bool IsFreeRoam() {}
    bool IsExploreMode() {}
    bool IsGetAway() {}
    bool IsLapKnockout() {}
    bool IsRollingStart() {}
    bool IsGamebreakerOn() {}

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
    int8 PlayerStartPosition[2];                // offset 0x38, size 0x2
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
    int32 NumDriverInfo;                        // offset 0x94, size 0x4
    Timer TimeTrialTime;                        // offset 0x98, size 0x4
    bool bCareerEventRace;                      // offset 0x9C, size 0x1
};

// total size: 0x2C
struct RunTimePlayerSettings {
    void ResetToDefaults();
    void DoSnapshot(ReplaySnapshot *snapshot);
    void SetName(const char *name);
    char *GetName() {}
    void ToggleLookback();
    void CycleAlternateCamera();
    void ToggleCameraSpring();
    void ToggleHudHotkey();

    char PlayersHudState;        // offset 0x0, size 0x1
    char DriveCameraAlternate;   // offset 0x1, size 0x1
    char PreferredGender;        // offset 0x2, size 0x1
    char CameraSpring;           // offset 0x3, size 0x1
    char LookbackMode;           // offset 0x4, size 0x1
    char Pad;                    // offset 0x5, size 0x1
    char Pad2;                   // offset 0x6, size 0x1
    char Pad3;                   // offset 0x7, size 0x1
    uint32 PreferredHudFeatures; // offset 0x8, size 0x4
    char PlayerName[16];         // offset 0xC, size 0x10
    Timer BestTotalTimeRecord;   // offset 0x1C, size 0x4
    Timer BestLapTimeRecord;     // offset 0x20, size 0x4
    int HudHotkeyEnabled;        // offset 0x24, size 0x4
    float DragHandicap;          // offset 0x28, size 0x4
};

extern RaceParameters TheRaceParameters;

#endif
