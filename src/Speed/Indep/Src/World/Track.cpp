#include "Track.hpp"

#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"

int NumTrackOBBs = 0;
TrackOBB *TrackOBBTable = nullptr;
bChunkLoader bChunkLoaderTrackOBB(BCHUNK_TRACK_OBJECT_BOUNDS, LoaderTrackOBB, UnloaderTrackOBB);

int GetNumTrackOBBs() {
    return NumTrackOBBs;
}

TrackOBB *GetTrackOBB(int index) {
    return &TrackOBBTable[index];
}

int LoaderTrackOBB(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_TRACK_OBJECT_BOUNDS) {
        return 0;
    }

    NumTrackOBBs = chunk->GetAlignedSize(16) / sizeof(TrackOBB);
    TrackOBBTable = reinterpret_cast<TrackOBB *>(chunk->GetAlignedData(16));
    for (int n = 0; n < NumTrackOBBs; n++) {
        TrackOBB *track_obb = &TrackOBBTable[n];
        track_obb->EndianSwap();
    }

    return 1;
}

int UnloaderTrackOBB(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_TRACK_OBJECT_BOUNDS) {
        return 0;
    }

    NumTrackOBBs = 0;
    TrackOBBTable = nullptr;
    return 1;
}

void EstablishRemoteCaffeineConnection() {}

// TODO where is this?
inline UMath::Vector3 &bConvertToBond(UMath::Vector3 &dest, const bVector3 &v) {
    return *reinterpret_cast<UMath::Vector3 *>(&bConvertToBond(*reinterpret_cast<bVector3 *>(&dest), v));
}

float TopologyCoordinate::GetElevation(const bVector3 *position, TerrainType *type, bVector3 *normal, bool *point_valid) {
    UMath::Vector3 bond_pos;

    bConvertToBond(bond_pos, *position);
    WWorldPos world_pos(0.025f);
    UMath::Vector4 dummy_normal;
    world_pos.Update(bond_pos, dummy_normal, true, nullptr, true);
    if (point_valid) {
        *point_valid = world_pos.OnValidFace();
    }
    if (world_pos.OnValidFace()) {
        return world_pos.HeightAtPoint(bond_pos);
    } else {
        return position->z;
    }
}

int TopologyCoordinate::HasTopology(const bVector2 *position) {
    float test_elevation;
    bVector3 test_position(position->x, position->y, 99999.1f);
    bool point_valid;

    test_elevation = GetElevation(&test_position, nullptr, nullptr, &point_valid);
    return point_valid;
}
