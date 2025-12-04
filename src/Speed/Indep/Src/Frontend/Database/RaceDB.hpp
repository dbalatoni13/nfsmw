#ifndef FRONTEND_DATABASE_RACEDB_H
#define FRONTEND_DATABASE_RACEDB_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Timer.hpp"

// total size: 0x8
struct TrackHighScore {
    short TrackNumber; // offset 0x0, size 0x2
    char NumLaps;      // offset 0x2, size 0x1
    char Direction;    // offset 0x3, size 0x1
    int BestLapTime;   // offset 0x4, size 0x4
};

// total size: 0x8
struct RaceTypeHighScores {
    int TotalStarts; // offset 0x0, size 0x4
    int TotalWins;   // offset 0x4, size 0x4
};

// total size: 0x38
struct TopEvadedPursuitDetail {
    char PursuitName[12];     // offset 0x0, size 0xC
    unsigned int CarFEKey;    // offset 0xC, size 0x4
    int Bounty;               // offset 0x10, size 0x4
    int Length;               // offset 0x14, size 0x4
    int NumCops;              // offset 0x18, size 0x4
    int NumCopsDamaged;       // offset 0x1C, size 0x4
    int NumCopsDestroyed;     // offset 0x20, size 0x4
    int NumRoadblocksDodged;  // offset 0x24, size 0x4
    int NumSpikeStripsDodged; // offset 0x28, size 0x4
    int TotalCostToState;     // offset 0x2C, size 0x4
    int NumInfractions;       // offset 0x30, size 0x4
    int NumHelicopters;       // offset 0x34, size 0x4
};

// total size: 0x20
struct CareerPursuitScores {
    int Value[8]; // offset 0x0, size 0x20
};

// total size: 0x8
struct PursuitScore {
    unsigned int CarFEKey; // offset 0x0, size 0x4
    int Value;             // offset 0x4, size 0x4
};

// total size: 0x20
struct CostToStateScores {
    int mNumRoadblocksDeployed;      // offset 0x0, size 0x4
    int mNumTrafficCarsHit;          // offset 0x4, size 0x4
    int mNumSpikeStripsDeployed;     // offset 0x8, size 0x4
    int mNumHeliSpikeStripsDeployed; // offset 0xC, size 0x4
    int mNumCopCarsDeployed;         // offset 0x10, size 0x4
    int mNumSupportVehiclesDeployed; // offset 0x14, size 0x4
    int mPropertyDamageValue;        // offset 0x18, size 0x4
    int mNumPropertiesDamaged;       // offset 0x1C, size 0x4
};

// total size: 0xBD8
class HighScoresDatabase {
  public:
    TrackHighScore TrackHighScoreTable[320];          // offset 0x0, size 0xA00
    float TotalOdometer;                              // offset 0xA00, size 0x4
    int TotalStarts;                                  // offset 0xA04, size 0x4
    int TotalWins;                                    // offset 0xA08, size 0x4
    int TotalLosses;                                  // offset 0xA0C, size 0x4
    RaceTypeHighScores RaceTypeScores[2];             // offset 0xA10, size 0x10
    RaceTypeHighScores SplitScreenScores;             // offset 0xA20, size 0x8
    int TotalDragTotalled;                            // offset 0xA28, size 0x4
    TopEvadedPursuitDetail TopEvadedPursuitScores[5]; // offset 0xA2C, size 0x118
    CareerPursuitScores CareerPursuitDetails;         // offset 0xB44, size 0x20
    PursuitScore BestPursuitRankings[10];             // offset 0xB64, size 0x50
    CostToStateScores CostToStateDetails;             // offset 0xBB4, size 0x20
    unsigned int PreviouslyPursuedCarFEKey;           // offset 0xBD4, size 0x4
};

// total size: 0xC0
struct FinishedRaceStatsEntry {
    int FinishPosition;          // offset 0x0, size 0x4
    int DriverNumber;            // offset 0x4, size 0x4
    int FinishReason;            // offset 0x8, size 0x4
    Timer RaceTime;              // offset 0xC, size 0x4
    Timer BestLapTime;           // offset 0x10, size 0x4
    int HandlingMode;            // offset 0x14, size 0x4
    float TopSpeed;              // offset 0x18, size 0x4
    float AverageSpeed;          // offset 0x1C, size 0x4
    Timer LapTimes[11];          // offset 0x20, size 0x2C
    Timer LapRunningTimes[11];   // offset 0x4C, size 0x2C
    int NumLapsCompleted;        // offset 0x78, size 0x4
    float NumLapsCompletedExact; // offset 0x7C, size 0x4
    int PositionPerLap[8];       // offset 0x80, size 0x20
    int NumLapsLead;             // offset 0xA0, size 0x4
    float Odometer;              // offset 0xA4, size 0x4
    Timer ZeroToSixtyTime;       // offset 0xA8, size 0x4
    Timer QuarterMileTime;       // offset 0xAC, size 0x4
    float QuarterMileSpeed;      // offset 0xB0, size 0x4
    float LongestJump;           // offset 0xB4, size 0x4
    float LongestPowerSlide;     // offset 0xB8, size 0x4
    int WasRecordBreaker;        // offset 0xBC, size 0x4
};

// total size: 0x604
struct cFinishedRaceStats {
    FinishedRaceStatsEntry RaceStats[8]; // offset 0x0, size 0x600
    int NumStats;                        // offset 0x600, size 0x4
};

#endif
