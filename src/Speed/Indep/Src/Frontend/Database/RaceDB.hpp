#ifndef RACEDB_HPP
#define RACEDB_HPP

#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

#define MAX_LAP_TIMES_PER_CAR 11             //  :44
#define FINISH_RACE_STATS_CARS 8             //  :46
#define FINISH_RACE_STATS_MAX_LAPS gMAX_LAPS //  :47

// TODO
class RacingCar;
class Race;

enum ePostRaceOptions {
    POST_RACE_OPT_NEXT_RACE = 0,
    POST_RACE_OPT_QUIT = 1,
    POST_RACE_OPT_RESTART_RACE = 2,
    POST_RACE_OPT_RESTART_EVENT = 3,
};

enum eExitRacePlaces {
    EXIT_RACE_FROM_PAUSE = 0,
    EXIT_RACE_FROM_POSTRACE = 1,
};

// total size: 0x24
// Decl: 61
class RaceSettings {
  public:
    RaceSettings() { // Decl: 98
        EventHash = 0;
        Default();
    }

    void Default(); // Decl: 104

    uint32 EventHash;       // offset 0x0, size 0x4, Decl: 106
    uint8 NumLaps;          // offset 0x4, size 0x1, Decl: 107
    uint8 TrackDirection;   // offset 0x5, size 0x1, Decl: 108
    bool IsLapKO;           // offset 0x8, size 0x1, Decl: 109
    uint8 NumOpponents;     // offset 0xC, size 0x1, Decl: 110
    uint8 AISkill;          // offset 0xD, size 0x1, Decl: 111
    uint8 CopDensity;       // offset 0xE, size 0x1, Decl: 112
    uint8 TrafficDensity;   // offset 0xF, size 0x1, Decl: 113
    bool CatchUp;           // offset 0x10, size 0x1, Decl: 114
    bool CopsOn;            // offset 0x14, size 0x1, Decl: 115
    uint8 RegionFilterBits; // offset 0x18, size 0x1, Decl: 118
    uint32 SelectedCar[2];  // offset 0x1C, size 0x8, Decl: 140
#ifdef EA_BUILD_A124
    int CarSelectFilterBits[2];
#endif
    void SetSelectedCar(uint32 car, int player_num) { // Decl: 141
        SelectedCar[player_num] = car;
    }
    uint32 GetSelectedCar(int player_num) { // Decl: 142
        return SelectedCar[player_num];
    }
};

// total size: 0x10
// Decl: 157
class OnlineRaceParameters {
  public:
    OnlineRaceParameters() {} // Decl: 159

    void Default(); // Decl: 161

    uint32 EventHash;     // offset 0x0, size 0x4, Decl: 163
    uint8 NumLaps;        // offset 0x4, size 0x1, Decl: 164
    uint8 TrackDirection; // offset 0x5, size 0x1, Decl: 165
    uint8 CopDensity;     // offset 0x6, size 0x1, Decl: 166
    float TimeOfDay;      // offset 0x8, size 0x4, Decl: 167
    bool IsLapKO;         // offset 0xC, size 0x1, Decl: 168
};

// total size: 0xC0
// Decl: 215
class FinishedRaceStatsEntry {
  public:
    int FinishPosition;          // offset 0x0, size 0x4, Decl: 217
    int DriverNumber;            // offset 0x4, size 0x4, Decl: 218
    int FinishReason;            // offset 0x8, size 0x4, Decl: 219
    Timer RaceTime;              // offset 0xC, size 0x4, Decl: 220
    Timer BestLapTime;           // offset 0x10, size 0x4, Decl: 221
    int HandlingMode;            // offset 0x14, size 0x4
    float TopSpeed;              // offset 0x18, size 0x4, Decl: 222
    float AverageSpeed;          // offset 0x1C, size 0x4, Decl: 223
    Timer LapTimes[11];          // offset 0x20, size 0x2C, Decl: 224
    Timer LapRunningTimes[11];   // offset 0x4C, size 0x2C, Decl: 225
    int NumLapsCompleted;        // offset 0x78, size 0x4, Decl: 226
    float NumLapsCompletedExact; // offset 0x7C, size 0x4, Decl: 227
    int PositionPerLap[8];       // offset 0x80, size 0x20, Decl: 228
    int NumLapsLead;             // offset 0xA0, size 0x4, Decl: 229
    float Odometer;              // offset 0xA4, size 0x4, Decl: 230
    Timer ZeroToSixtyTime;       // offset 0xA8, size 0x4, Decl: 231
    Timer QuarterMileTime;       // offset 0xAC, size 0x4, Decl: 232
    float QuarterMileSpeed;      // offset 0xB0, size 0x4, Decl: 233
    float LongestJump;           // offset 0xB4, size 0x4, Decl: 234
    float LongestPowerSlide;     // offset 0xB8, size 0x4
    int WasRecordBreaker;        // offset 0xBC, size 0x4

    FinishedRaceStatsEntry() : RaceTime(), BestLapTime(), LapTimes(), LapRunningTimes(), ZeroToSixtyTime(), QuarterMileTime() {}

    void Construct(RacingCar *racing_car, bool ignore_online_info); // Decl: 247

    Timer &GetLapTime(int lap) { // Decl: 250
        return LapTimes[lap];
    }
    Timer &GetLapRunningTime(int lap) { // Decl: 251
        return LapRunningTimes[lap];
    }
};

// total size: 0x604
// Decl: 256
class cFinishedRaceStats {
  public:
    FinishedRaceStatsEntry RaceStats[8]; // offset 0x0, size 0x600, Decl: 258
    int NumStats;                        // offset 0x600, size 0x4, Decl: 259

    cFinishedRaceStats() : RaceStats() {}

    void CalculatePositionsForEachLap(); // Decl: 261

    void Construct(Race *race); // Decl: 263

    FinishedRaceStatsEntry *GetStatsForDriver(int driver_number); // Decl: 265
    FinishedRaceStatsEntry *GetIndex(int index) {                 // Decl: 266
        return &RaceStats[index];
    }
    FinishedRaceStatsEntry *GetFinisher(int position); // Decl: 267
    int GetNumStats() {                                // Decl: 268
        return NumStats;
    }
};

enum ePursuitDetailTypes {
    PD_PURUSIT_LENGTH = 0,
    PD_COPS_INVOLVED = 1,
    PD_COPS_DAMAGED = 2,
    PD_COPS_DESTROYED = 3,
    PD_SPIKESTRIPS_DODGED = 4,
    PD_ROADBLOCKS_DODGED = 5,
    PD_HELICOPTERS_INVOLVED = 6,
    PD_COST_TO_STATE = 7,
    PD_NUM_CAREER_TO_STORE = 8,
    PD_NUM_INFRACTIONS = 8,
    PD_BOUNTY = 9,
    PD_NUM_SINGLE_PURSUIT_TO_STORE = 10,
    PD_NUM_PD_TYPES = 10
};

enum eHighScoresRaceTypes {
    HS_RACE_TYPE_UNKNOWN = -1,
    HS_RACE_TYPE_SPRINT = 0,
    HS_RACE_TYPE_CIRCUIT = 1,
    NUM_HS_RACE_TYPES = 2,
};

// total size: 0x8
struct RaceTypeHighScores {
    int32 TotalStarts; // offset 0x0, size 0x4
    int32 TotalWins;   // offset 0x4, size 0x4
};

// total size: 0x38
class TopEvadedPursuitDetail {
  public:
    TopEvadedPursuitDetail() {
        bMemSet(this, 0, sizeof(TopEvadedPursuitDetail));
    }

    void GeneratePursuitID();

    char PursuitName[12];       // offset 0x0, size 0xC
    uint32 CarFEKey;            // offset 0xC, size 0x4
    int32 Bounty;               // offset 0x10, size 0x4
    int32 Length;               // offset 0x14, size 0x4
    int32 NumCops;              // offset 0x18, size 0x4
    int32 NumCopsDamaged;       // offset 0x1C, size 0x4
    int32 NumCopsDestroyed;     // offset 0x20, size 0x4
    int32 NumRoadblocksDodged;  // offset 0x24, size 0x4
    int32 NumSpikeStripsDodged; // offset 0x28, size 0x4
    int32 TotalCostToState;     // offset 0x2C, size 0x4
    int32 NumInfractions;       // offset 0x30, size 0x4
    int32 NumHelicopters;       // offset 0x34, size 0x4
};

// total size: 0x20
class CareerPursuitScores {
  public:
    CareerPursuitScores() {
        bMemSet(this, 0, sizeof(CareerPursuitScores));
    }
    void IncValue(ePursuitDetailTypes type, int32 amount);
    int32 GetValue(ePursuitDetailTypes type) const;

  private:
    int32 Value[8]; // offset 0x0, size 0x20
};

// total size: 0x8
class PursuitScore {
  public:
    PursuitScore() {
        bMemSet(this, 0, sizeof(PursuitScore));
    }

    uint32 CarFEKey; // offset 0x0, size 0x4
    int32 Value;     // offset 0x4, size 0x4
};

enum RAP_CTS_ITEM {
    RAP_CTS_HELI_SPAWN = 0,
    RAP_CTS_SUPPORT_VEHICLE_DEPLOYED = 1,
    RAP_CTS_COP_CAR_DEPLOYED = 2,
    RAP_CTS_COP_DESTROYED = 3,
    RAP_CTS_COP_DAMAGED = 4,
    RAP_CTS_ROADBLOCK_DEPLOYED = 5,
    RAP_CTS_SPIKE_STRIP_DEPLOYED = 6,
    RAP_CTS_HELI_SPIKE_STRIP_DEPLOYED = 7,
    RAP_CTS_TRAFFIC_CAR_HIT = 8,
    RAP_CTS_PROPERTY_DAMAGE = 9,
};

// total size: 0x20
class CostToStateScores {
  public:
    CostToStateScores() {
        bMemSet(this, 0, sizeof(CostToStateScores));
    }

    int mNumRoadblocksDeployed;      // offset 0x0, size 0x4
    int mNumTrafficCarsHit;          // offset 0x4, size 0x4
    int mNumSpikeStripsDeployed;     // offset 0x8, size 0x4
    int mNumHeliSpikeStripsDeployed; // offset 0xC, size 0x4
    int mNumCopCarsDeployed;         // offset 0x10, size 0x4
    int mNumSupportVehiclesDeployed; // offset 0x14, size 0x4
    int mPropertyDamageValue;        // offset 0x18, size 0x4
    int mNumPropertiesDamaged;       // offset 0x1C, size 0x4
};

// total size: 0x8
struct TrackHighScore {
    int16 TrackNumber; // offset 0x0, size 0x2
    int8 NumLaps;      // offset 0x2, size 0x1
    int8 Direction;    // offset 0x3, size 0x1
    int32 BestLapTime; // offset 0x4, size 0x4
};

// total size: 0xBD8
// Decl: 1001
class HighScoresDatabase {
  public:
    HighScoresDatabase() : TopEvadedPursuitScores(), CareerPursuitDetails(), BestPursuitRankings(), CostToStateDetails() {}

  private:
    bool MaybeAddTrackHighScore(eHighScoresRaceTypes race_type, int track, int direction, int laps, FinishedRaceStatsEntry *stats);
    TrackHighScore *GetEmptyHighScore();
    bool DidPlayerBreakRecord(eHighScoresRaceTypes race_type, TrackHighScore *ths, FinishedRaceStatsEntry *stats);

  public:
    float GetWinPercentage();
    void Default();
    TrackHighScore *FindHighScore(int track, int direction, int num_laps); // RaceDB.hpp:1029
    bool IsAHighScore(eHighScoresRaceTypes race_type, int track, int direction, int laps, FinishedRaceStatsEntry *stats);
    void DebugPrint();
    void CommitHighScoresPreRace(eHighScoresRaceTypes race_type, bool is_splPD_screen);
    void CommitHighScoresPostRace(eHighScoresRaceTypes race_type, int track, int direction, int laps, int is_splPD_screen,
                                  FinishedRaceStatsEntry *stats);
    void CommitHighScoresPauseQuit();
    void CommitPursuitInfo(IPursuit *iPursuit, uint32 car_FEKey, int32 bounty, unsigned int num_infractions);
    int CalcPursuitRank(ePursuitDetailTypes type, bool career_rank);
    const TopEvadedPursuitDetail &GetTopEvadedPursuitScores(uint16 index) const {
        return TopEvadedPursuitScores[index];
    }
    int32 GetCareerPursuitScore(ePursuitDetailTypes type) const {
        return CareerPursuitDetails.GetValue(type);
    }
    const PursuitScore &GetBestPursuitScore(ePursuitDetailTypes type) const {
        return BestPursuitRankings[type];
    }
    uint32 GetPreviouslyPursuedCarNameHash() const;
    void GetCareerCST(RAP_CTS_ITEM item, int32 &quantity, uint32 &value) const;

    TrackHighScore TrackHighScoreTable[320]; // offset 0x0, size 0xA00
    float TotalOdometer;                     // offset 0xA00, size 0x4
    int32 TotalStarts;                       // offset 0xA04, size 0x4
    int32 TotalWins;                         // offset 0xA08, size 0x4
    int32 TotalLosses;                       // offset 0xA0C, size 0x4
    RaceTypeHighScores RaceTypeScores[2];    // offset 0xA10, size 0x10
    RaceTypeHighScores SplitScreenScores;    // offset 0xA20, size 0x8
    int32 TotalDragTotalled;                 // offset 0xA28, size 0x4
  private:
    TopEvadedPursuitDetail TopEvadedPursuitScores[5];                 // offset 0xA2C, size 0x118
    CareerPursuitScores CareerPursuitDetails;                         // offset 0xB44, size 0x20
    PursuitScore BestPursuitRankings[PD_NUM_SINGLE_PURSUIT_TO_STORE]; // offset 0xB64, size 0x50
    CostToStateScores CostToStateDetails;                             // offset 0xBB4, size 0x20
    uint32 PreviouslyPursuedCarFEKey;                                 // offset 0xBD4, size 0x4
};

uint32 CalcLanguageHash(const char *prefix /* r30 */, GRaceParameters *pRaceParams /* r4 */);
void FixDot(char *buf /* r3 */, int size /* r4 */);

#endif
