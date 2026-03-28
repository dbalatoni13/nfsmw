#ifndef WORLD_TRACKPOSITIONMARKER_H
#define WORLD_TRACKPOSITIONMARKER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x30
class TrackPositionMarker : public bTNode<TrackPositionMarker> {
  public:
    uint32 NameHash;      // offset 0x8, size 0x4
    int32 Param;          // offset 0xC, size 0x4
    bVector3 Position;    // offset 0x10, size 0x10
    unsigned short Angle; // offset 0x20, size 0x2
    int16 Padding0;       // offset 0x22, size 0x2
    int32 TrackNumber;    // offset 0x24, size 0x4
    int32 Padding2;       // offset 0x28, size 0x4
    int32 Padding3;       // offset 0x2C, size 0x4
};

typedef bool (*TrackPositionMarkerCallback)(TrackPositionMarker *, unsigned int);

int LoaderTrackPositionMarkers(bChunk *chunk);
int UnloaderTrackPositionMarkers(bChunk *chunk);
void ForEachTrackPositionMarker(TrackPositionMarkerCallback callback, uint32 tag);
int GetNumTrackPositionMarkers(int track_number, unsigned int name_hash);
TrackPositionMarker *GetTrackPositionMarker(int track_number, unsigned int name_hash, int index);
TrackPositionMarker *GetTrackPositionMarker(unsigned int name_hash, int index);

extern bTList<TrackPositionMarker> TrackPositionMarkerList;

#endif
