#ifndef WORLD_TRACKPATH_H
#define WORLD_TRACKPATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

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

// total size: 0x18
class TrackPathBarrier {
  public:
    void EndianSwap() {
        bPlatEndianSwap(&Points[0]);
        bPlatEndianSwap(&Points[1]);
        bPlatEndianSwap(&GroupHash);
    }

    bool HasGroup(unsigned int group_hash) {
        return GroupHash == group_hash;
    }

    bVector2 Points[2];     // offset 0x0, size 0x10
    char Enabled;           // offset 0x10, size 0x1
    char Pad;               // offset 0x11, size 0x1
    char PlayerBarrier;     // offset 0x12, size 0x1
    char LeftHanded;        // offset 0x13, size 0x1
    unsigned int GroupHash; // offset 0x14, size 0x4
};

// total size: 0x244
class TrackPathZone {
  public:
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
    float GetSegmentNextTo(bVector2 *point, bVector2 *segment_point_a, bVector2 *segment_point_b);

    float GetElevation() {
        return Elevation;
    }

    eTrackPathZoneType GetType() {
        return Type;
    }

    int GetMemoryImageSize() {
        return MemoryImageSize;
    }

    void SetVisitInfo(int v) {
        VisitInfo = v;
    }

    TrackPathZone *GetMemoryImageNext() {
        return reinterpret_cast<TrackPathZone *>(reinterpret_cast<char *>(this) + GetMemoryImageSize());
    }

    int GetData(int index) {
        return Data[index];
    }
};

// total size: 0x48C
class TrackPathManager {
  public:
    // total size: 0x4C
    struct ZoneInfo {
        int NumZones;                  // offset 0x0, size 0x4
        TrackPathZone *pFirstZone;     // offset 0x4, size 0x4
        TrackPathZone *pLastZone;      // offset 0x8, size 0x4
        bVector2 CachedBBoxMin;        // offset 0xC, size 0x8
        bVector2 CachedBBoxMax;        // offset 0x14, size 0x8
        int NumCachedZones;            // offset 0x1C, size 0x4
        int NumCacheHits;              // offset 0x20, size 0x4
        int NumCacheRebuilds;          // offset 0x24, size 0x4
        int NumFullRebuilds;           // offset 0x28, size 0x4
        TrackPathZone *CachedZones[8]; // offset 0x2C, size 0x20
    };

  public:
    TrackPathManager() {
        Clear();
    }

    int Loader(bChunk *chunk);
    int Unloader(bChunk *chunk);
    void Clear();
    void EnableBarriers(const char *group_name);
    void DisableAllBarriers();
    void BuildZoneInfoTable();
    TrackPathZone *FindZone(const bVector2 *position, eTrackPathZoneType zone_type, TrackPathZone *prev_zone);
    void ResetZoneVisitInfos();

    void Close() {}

    TrackPathBarrier *GetBarrier(int n) {
        return &pBarriers[n];
    }

  private:
    TrackPathZone *GetLastZone() {
        return reinterpret_cast<TrackPathZone *>(reinterpret_cast<char *>(pZones) + SizeofZones);
    }

    int NumZones;                // offset 0x0, size 0x4
    int SizeofZones;             // offset 0x4, size 0x4
    TrackPathZone *pZones;       // offset 0x8, size 0x4
    ZoneInfo ZoneInfoTable[15];  // offset 0xC, size 0x474
    int MostCachedZones;         // offset 0x480, size 0x4
    int NumBarriers;             // offset 0x484, size 0x4
    TrackPathBarrier *pBarriers; // offset 0x488, size 0x4
};

extern TrackPathManager TheTrackPathManager;

bool DoLinesIntersect(const bVector2 &line1_start, const bVector2 &line1_end, const bVector2 &line2_start, const bVector2 &line2_end);
void TrackPathInitRemoteCaffeineConnection();
int LoaderTrackPath(bChunk *chunk);
int UnloaderTrackPath(bChunk *chunk);

#endif
