#ifndef WORLD_TRACKPATH_H
#define WORLD_TRACKPATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

enum eTrackPathZoneType {
    NUM_TRACK_PATH_ZONES = 15,
    TRACK_PATH_ZONE_PURSUIT_START = 14,
    TRACK_PACH_ZONE_NO_COP_SPAWN = 13,
    TRACK_PATH_ZONE_JUMP_CAM = 12,
    TRACK_PATH_ZONE_NEIGHBOURHOOD = 11,
    TRACK_PATH_ZONE_DYNAMIC = 10,
    TRACK_PATH_ZONE_TRAFFIC_PATTERN = 9,
    TRACK_PATH_ZONE_HIDDEN = 8,
    TRACK_PATH_ZONE_GARAGE = 7,
    TRACK_PATH_ZONE_STREAMER_PREDICTION = 6,
    TRACK_PATH_ZONE_OVERPASS_SMALL = 5,
    TRACK_PATH_ZONE_OVERPASS = 4,
    TRACK_PATH_ZONE_TUNNEL = 3,
    TRACK_PATH_ZONE_GUIDED_RESET = 2,
    TRACK_PATH_ZONE_RESET_TO_POINT = 1,
    TRACK_PATH_ZONE_RESET = 0,
};

class TrackPathZone {
  public:
    // total size: 0x244
    eTrackPathZoneType Type; // offset 0x0, size 0x4
    bVector2 Position;       // offset 0x4, size 0x8
    bVector2 Direction;      // offset 0xC, size 0x8
    float Elevation;         // offset 0x14, size 0x4
    char ZoneSource;         // offset 0x18, size 0x1
    char CachedIndex;        // offset 0x19, size 0x1
    short VisitInfo;         // offset 0x1A, size 0x2
    void *pUserData;         // offset 0x1C, size 0x4
    bVector2 BBoxMin;        // offset 0x20, size 0x8
    bVector2 BBoxMax;        // offset 0x28, size 0x8
    int Data[4];             // offset 0x30, size 0x10
    short NumPoints;         // offset 0x40, size 0x2
    short MemoryImageSize;   // offset 0x42, size 0x2
    bVector2 Points[64];     // offset 0x44, size 0x200

    bool IsPointOnBoundary(const bVector2 *point);
    void GetOpposite(bVector2 *in0, bVector2 *in1, bVector2 *opp0, bVector2 *opp1);
    bool GetIntercept(bVector2 &InterceptPoint, const bVector2 *Start, const bVector2 *Direction);
    bool IsPointInside(const bVector2 *point);
};

class TrackPathManager {
    struct ZoneInfo {
        // total size: 0x4C
        int NumZones;
        TrackPathZone *pFirstZone;
        TrackPathZone *pLastZone;
        bVector2 CachedBBoxMin;
        bVector2 CachedBBoxMax;
        int NumCachedZones;
        int NumCacheHits;
        int NumCacheRebuilds;
        int NumFullRebuilds;
        TrackPathZone *CachedZones[8];
    };

    // total size: 0x48C
    int NumZones;                       // offset 0x0, size 0x4
    int SizeofZones;                    // offset 0x4, size 0x4
    TrackPathZone *pZones;              // offset 0x8, size 0x4
    ZoneInfo ZoneInfoTable[15];         // offset 0xC, size 0x474
    int MostCachedZones;                // offset 0x480, size 0x4
    int NumBarriers;                    // offset 0x484, size 0x4
    struct TrackPathBarrier *pBarriers; // offset 0x488, size 0x4

  public:
    void EnableBarriers(const char *group_name);
    void BuildZoneInfoTable();
    TrackPathZone *FindZone(const bVector2 *position, eTrackPathZoneType zone_type, TrackPathZone *prev_zone);
    void ResetZoneVisitInfos();
};

extern TrackPathManager TheTrackPathManager;

void TrackPathInitRemoteCaffeineConnection();

#endif
