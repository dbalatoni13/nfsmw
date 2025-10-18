#ifndef WORLD_TRACKINFO_H
#define WORLD_TRACKINFO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

enum eLocationName {
    MODE_SPECIFIC = 5,
    AIRPORT = 4,
    INDUSTRIAL_PARK = 3,
    SUBURBAN_HILLS = 2,
    CITY_CORE = 1,
    UPPER_CLASS = 0,
};

enum eDriftType {
    TEAM = 2,
    DOWNHILL = 1,
    VS_AI = 0,
};

enum eTrackDifficulty {
    TRACK_DIFFICULTY_HARD = 2,
    TRACK_DIFFICULTY_MEDIUM = 1,
    TRACK_DIFFICULTY_EASY = 0,
};

class TrackInfo {
public:
    // total size: 0x120
    char Name[32]; // offset 0x0, size 0x20
    char TrackDirectory[32]; // offset 0x20, size 0x20
    char RegionName[8]; // offset 0x40, size 0x8
    char RegionDirectory[32]; // offset 0x48, size 0x20
    int LocationNumber; // offset 0x68, size 0x4
    char LocationDirectory[16]; // offset 0x6C, size 0x10
    eLocationName LocationName; // offset 0x7C, size 0x4
    eDriftType DriftType; // offset 0x80, size 0x4
    char IsValid; // offset 0x84, size 0x1
    char Point2Point; // offset 0x85, size 0x1
    char ReverseVersionExists; // offset 0x86, size 0x1
    char Unused; // offset 0x87, size 0x1
    char IsPerformanceTuning; // offset 0x88, size 0x1
    short TrackNumber; // offset 0x8A, size 0x2
    short SameAsTrackNumber; // offset 0x8C, size 0x2
    unsigned int SunInfoNameHash; // offset 0x90, size 0x4
    unsigned int UsageFlags; // offset 0x94, size 0x4
    unsigned int TrackLength; // offset 0x98, size 0x4
    float TimeToBeatForwards_ms; // offset 0x9C, size 0x4
    float TimeToBeatReverse_ms; // offset 0xA0, size 0x4
    int ScoreToBeatForwards_DriftOnly; // offset 0xA4, size 0x4
    int ScoreToBeatReverse_DriftOnly; // offset 0xA8, size 0x4
    bVector2 TrackMapCalibrationUpperLeft; // offset 0xAC, size 0x8
    float TrackMapCalibrationMapWidthMetres; // offset 0xB4, size 0x4
    unsigned short TrackMapCalibrationRotation; // offset 0xB8, size 0x2
    unsigned short TrackMapStartLineAngle; // offset 0xBA, size 0x2
    unsigned short TrackMapFinishLineAngle; // offset 0xBC, size 0x2
    float TrackMapZoomFactor; // offset 0xC0, size 0x4
    eTrackDifficulty ForwardDifficulty; // offset 0xC4, size 0x4
    eTrackDifficulty ReverseDifficulty; // offset 0xC8, size 0x4
    short OverrideStartingRouteForAI[2][4]; // offset 0xCC, size 0x10
    short NumSecondsBeforeShortcutsAllowed; // offset 0xDC, size 0x2
    short nDriftSecondsMin; // offset 0xDE, size 0x2
    short nDriftSecondsMax; // offset 0xE0, size 0x2
    short pad2[1]; // offset 0xE2, size 0x2
    signed char MaxTrafficCars[4][2]; // offset 0xE4, size 0x8
    signed char TrafficAllowedNearStartLine[2]; // offset 0xEC, size 0x2
    char CarRaceStartConfig; // offset 0xEE, size 0x1
    char padbyte; // offset 0xEF, size 0x1
    float TrafficMinInitialDistanceFromStartLine[2]; // offset 0xF0, size 0x8
    float TrafficMinInitialDistanceBetweenCars[2]; // offset 0xF8, size 0x8
    float TrafficOncomingFraction[4]; // offset 0x100, size 0x10
    bVector2 TrackMapZoomTopLeft; // offset 0x110, size 0x8
    float TrackMapZoomWidth; // offset 0x118, size 0x4
    char TrackMapStartZoomed; // offset 0x11C, size 0x1
};

extern TrackInfo *LoadedTrackInfo;

#endif
