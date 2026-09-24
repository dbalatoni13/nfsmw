#ifndef WORLD_TRACKINFO_H
#define WORLD_TRACKINFO_H

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

enum eTrackDifficulty { TRACK_DIFFICULTY_EASY = 0, TRACK_DIFFICULTY_MEDIUM = 1, TRACK_DIFFICULTY_HARD = 2 };

enum eTrackUsageTypes {
    eTRACKUSAGE_CIRCUIT = 1,
    eTRACKUSAGE_DRAG_RACE = 2,
    eTRACKUSAGE_SPRINT = 4,
    eTRACKUSAGE_DRIFT_RACE = 8,
    eTRACKUSAGE_FREE_RUN = 16,
    eTRACKUSAGE_TOURNAMENT = 32,
    eTRACKUSAGE_LAP_KNOCKOUT = 64,
    eTRACKUSAGE_SMOKESHOW = 128,
    eTRACKUSAGE_CASH_KNOCKOUT = 256,
    eTRACKUSAGE_PARTY_MODE = 512,
    eTRACKUSAGE_CARSHOW = 1024,
    eTRACKUSAGE_SHORT_TRACK = 2048,
    eTRACKUSAGE_STREET_CROSS = 4096,
    eTRACKUSAGE_RACING_LEAGUE = 8192
};

enum eTrackDirectionForTrackInfo {
    eDIRECTION_FORWARD_FOR_TRACK_INFO = 0,
    eDIRECTION_BACKWARD_FOR_TRACK_INFO = 1,
    NUM_TRACK_DIRECTIONS_FOR_TRACK_INFO = 2
};

enum eCarRaceStartConfig {
    eCarRaceStartConfig_NONE = 0,
    eCarRaceStartConfig_1_ROW_OF_4 = 1,
    eCarRaceStartConfig_2_ROWS_OF_2 = 2,
    eCarRaceStartConfig_3_ROWS_OF_2 = 3,
    eCarRaceStartConfig_2_ROWS_OF_1_FRONT_ON_LEFT = 4,
    eCarRaceStartConfig_2_ROWS_OF_1_FRONT_ON_RIGHT = 5,
    eCarRaceStartConfig_1_ROW_OF_1 = 6,
    eCarRaceStartConfig_1_UP_3_DOWN = 7
};

enum eLocationName {
    UPPER_CLASS = 0,
    CITY_CORE = 1,
    SUBURBAN_HILLS = 2,
    INDUSTRIAL_PARK = 3,
    AIRPORT = 4,
    MODE_SPECIFIC = 5,
};

enum eDriftType {
    VS_AI = 0,
    DOWNHILL = 1,
    TEAM = 2,
};

struct NeighbourhoodNameInfo {
    char *Name;
    uint32 NameHash;
};

// total size: 0x120
class TrackInfo {
  public:
    static TrackInfo *GetTrackInfo(int track_number);

    static void SetLoadedTrackInfo(int track_number) {
        LoadedTrackInfo = GetTrackInfo(track_number);
    }

    static TrackInfo *GetLoadedTrackInfo() {
        return LoadedTrackInfo;
    }

    static int LoaderTrackInfo(bChunk *chunk);
    static int UnloaderTrackInfo(bChunk *chunk);

    static int GetLoadedTrackNumber() {
        if (LoadedTrackInfo != nullptr) {
            return LoadedTrackInfo->TrackNumber;
        }
        return 0;
    }

    char Name[32];                                   // offset 0x0, size 0x20
    char TrackDirectory[32];                         // offset 0x20, size 0x20
    char RegionName[8];                              // offset 0x40, size 0x8
    char RegionDirectory[32];                        // offset 0x48, size 0x20
    int32 LocationNumber;                            // offset 0x68, size 0x4
    char LocationDirectory[16];                      // offset 0x6C, size 0x10
    eLocationName LocationName;                      // offset 0x7C, size 0x4
    eDriftType DriftType;                            // offset 0x80, size 0x4
    int8 IsValid;                                    // offset 0x84, size 0x1
    int8 Point2Point;                                // offset 0x85, size 0x1
    int8 ReverseVersionExists;                       // offset 0x86, size 0x1
    int8 Unused;                                     // offset 0x87, size 0x1
    int8 IsPerformanceTuning;                        // offset 0x88, size 0x1
    int16 TrackNumber;                               // offset 0x8A, size 0x2
    int16 SameAsTrackNumber;                         // offset 0x8C, size 0x2
    uint32 SunInfoNameHash;                          // offset 0x90, size 0x4
    uint32 UsageFlags;                               // offset 0x94, size 0x4
    uint32 TrackLength;                              // offset 0x98, size 0x4
    float TimeToBeatForwards_ms;                     // offset 0x9C, size 0x4
    float TimeToBeatReverse_ms;                      // offset 0xA0, size 0x4
    int32 ScoreToBeatForwards_DriftOnly;             // offset 0xA4, size 0x4
    int32 ScoreToBeatReverse_DriftOnly;              // offset 0xA8, size 0x4
    bVector2 TrackMapCalibrationUpperLeft;           // offset 0xAC, size 0x8
    float TrackMapCalibrationMapWidthMetres;         // offset 0xB4, size 0x4
    bAngle TrackMapCalibrationRotation;              // offset 0xB8, size 0x2
    bAngle TrackMapStartLineAngle;                   // offset 0xBA, size 0x2
    bAngle TrackMapFinishLineAngle;                  // offset 0xBC, size 0x2
    float TrackMapZoomFactor;                        // offset 0xC0, size 0x4
    eTrackDifficulty ForwardDifficulty;              // offset 0xC4, size 0x4
    eTrackDifficulty ReverseDifficulty;              // offset 0xC8, size 0x4
    int16 OverrideStartingRouteForAI[2][4];          // offset 0xCC, size 0x10
    int16 NumSecondsBeforeShortcutsAllowed;          // offset 0xDC, size 0x2
    int16 nDriftSecondsMin;                          // offset 0xDE, size 0x2
    int16 nDriftSecondsMax;                          // offset 0xE0, size 0x2
    int16 pad2[1];                                   // offset 0xE2, size 0x2
    int8 MaxTrafficCars[4][2];                       // offset 0xE4, size 0x8
    int8 TrafficAllowedNearStartLine[2];             // offset 0xEC, size 0x2
    int8 CarRaceStartConfig;                         // offset 0xEE, size 0x1
    int8 padbyte;                                    // offset 0xEF, size 0x1
    float TrafficMinInitialDistanceFromStartLine[2]; // offset 0xF0, size 0x8
    float TrafficMinInitialDistanceBetweenCars[2];   // offset 0xF8, size 0x8
    float TrafficOncomingFraction[4];                // offset 0x100, size 0x10
    bVector2 TrackMapZoomTopLeft;                    // offset 0x110, size 0x8
    float TrackMapZoomWidth;                         // offset 0x118, size 0x4
    int8 TrackMapStartZoomed;                        // offset 0x11C, size 0x1

  private:
    static TrackInfo *TrackInfoTable;
    static int NumTrackInfo;
    static int MaxTrackInfo;
    static TrackInfo *LoadedTrackInfo;
};

#endif
