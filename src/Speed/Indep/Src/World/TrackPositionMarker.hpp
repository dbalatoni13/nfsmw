#ifndef WORLD_TRACKPOSITIONMARKER_H
#define WORLD_TRACKPOSITIONMARKER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x30
struct TrackPositionMarker : public bTNode<TrackPositionMarker> {
    unsigned int NameHash; // offset 0x8, size 0x4
    int Param;             // offset 0xC, size 0x4
    bVector3 Position;     // offset 0x10, size 0x10
    unsigned short Angle;  // offset 0x20, size 0x2
    short Padding0;        // offset 0x22, size 0x2
    int TrackNumber;       // offset 0x24, size 0x4
    int Padding2;          // offset 0x28, size 0x4
    int Padding3;          // offset 0x2C, size 0x4
};

int GetNumTrackPositionMarkers(int track_number, unsigned int name_hash);
TrackPositionMarker *GetTrackPositionMarker(int track_number, unsigned int name_hash, int index);
TrackPositionMarker *GetTrackPositionMarker(unsigned int name_hash, int index);

extern bTList<TrackPositionMarker> TrackPositionMarkerList;

#endif
