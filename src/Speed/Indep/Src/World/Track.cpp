#include "Track.hpp"

#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"

void bEndianSwap32(void *value);

static inline UMath::Vector3 &bConvertToBond_Track(UMath::Vector3 &dest, const bVector3 &v) {
    bConvertToBond(*reinterpret_cast<bVector3 *>(&dest), v);
    return dest;
}

class WWorldPosTopologyShim_Track : public WWorldPos {
  public:
    WWorldPosTopologyShim_Track(float yOffset)
        : WWorldPos(yOffset) {
        fFace.fPt0 = UMath::Vector3::kZero;
        fFace.fPt1 = UMath::Vector3::kZero;
        fFace.fPt2 = UMath::Vector3::kZero;
        fFace.fSurface.fSurface = 0;
        fFace.fSurface.fFlags = 0;
    }
};

enum TerrainType {
    TERRAIN_TYPE_NONE = 0,
    TERRAIN_TYPE_ROAD = 1,
    TERRAIN_TYPE_ROAD_WET = 2,
    TERRAIN_TYPE_ROAD_DRIFT = 3,
    TERRAIN_TYPE_ROAD_SMOKE_1 = 4,
    TERRAIN_TYPE_ROAD_SMOKE_2 = 5,
    TERRAIN_TYPE_ROAD_SMOKE_3 = 6,
    TERRAIN_TYPE_BRIDGE = 7,
    TERRAIN_TYPE_DIRT = 8,
    TERRAIN_TYPE_GRAVEL = 9,
    TERRAIN_TYPE_ROUGH_ROAD = 10,
    TERRAIN_TYPE_COBBLESTONE = 11,
    TERRAIN_TYPE_STAIRS = 12,
    TERRAIN_TYPE_PUDDLE = 13,
    TERRAIN_TYPE_DEEP_WATER = 14,
    TERRAIN_TYPE_GRASS = 15,
    TERRAIN_TYPE_SIDEWALK = 16,
    TERRAIN_TYPE_WOOD = 17,
    TERRAIN_TYPE_PLASTIC = 18,
    TERRAIN_TYPE_GLASS = 19,
    TERRAIN_TYPE_SOLID_WALL = 20,
    TERRAIN_TYPE_SEE_THROUGH_WALL = 21,
    TERRAIN_TYPE_PLANT = 22,
    TERRAIN_TYPE_POST = 23,
    TERRAIN_TYPE_PILLAR = 24,
    TERRAIN_TYPE_METAL_GRATE = 25,
    TERRAIN_TYPE_METAL = 26,
    TERRAIN_TYPE_CHAINLINK = 27,
    TERRAIN_TYPE_CAR = 28,
    NUM_TERRAIN_TYPES = 29,
};

struct TopologyCoordinate {
    int HasTopology(const bVector2 *position);
    float GetElevation(const bVector3 *position, enum TerrainType *type, bVector3 *normal, bool *point_valid);

  private:
    int mData[2];
};

// total size: 0x60
struct TrackOBB {
    inline void EndianSwap() {
        bPlatEndianSwap(&TypeNameHash);
        bPlatEndianSwap(&Matrix);
        bPlatEndianSwap(&Dims);
    }

    unsigned int TypeNameHash;
    unsigned int Pad[3];
    bMatrix4 Matrix;
    bVector3 Dims;
};

static char *TrackOBBTable = 0;
static int NumTrackOBBs = 0;
bChunkLoader bChunkLoaderTrackOBB(0x34191, LoaderTrackOBB, UnloaderTrackOBB);

int GetNumTrackOBBs() {
    return NumTrackOBBs;
}

TrackOBB *GetTrackOBB(int index) {
    return reinterpret_cast<TrackOBB *>(TrackOBBTable + index * 0x60);
}

int LoaderTrackOBB(bChunk *chunk) {
    if (chunk->GetID() != 0x34191) {
        return 0;
    }

    NumTrackOBBs = static_cast<unsigned int>(chunk->GetAlignedSize(16)) / 0x60u;
    TrackOBBTable = chunk->GetAlignedData(16);
    for (int n = 0; n < NumTrackOBBs; n++) {
        TrackOBB *track_obb = reinterpret_cast<TrackOBB *>(TrackOBBTable + n * 0x60);
        track_obb->EndianSwap();
    }

    return 1;
}

int UnloaderTrackOBB(bChunk *chunk) {
    if (chunk->GetID() != 0x34191) {
        return 0;
    }

    NumTrackOBBs = 0;
    TrackOBBTable = 0;
    return 1;
}

void EstablishRemoteCaffeineConnection() {}

float TopologyCoordinate::GetElevation(const bVector3 *position, enum TerrainType *type, bVector3 *normal, bool *point_valid) {
    UMath::Vector3 bond_pos;
    UMath::Vector4 dummy_normal;

    (void)type;
    (void)normal;

    bConvertToBond_Track(bond_pos, *position);
    WWorldPosTopologyShim_Track world_pos(0.025f);
    world_pos.Update(bond_pos, dummy_normal, true, 0, true);
    if (point_valid) {
        *point_valid = world_pos.OnValidFace();
    }
    if (world_pos.OnValidFace()) {
        return world_pos.HeightAtPoint(bond_pos);
    }
    return position->z;
}

int TopologyCoordinate::HasTopology(const bVector2 *position) {
    float test_elevation;
    bVector3 test_position(position->x, position->y, 99999.1015625f);
    bool point_valid;

    test_elevation = GetElevation(&test_position, 0, 0, &point_valid);
    (void)test_elevation;
    return point_valid;
}
