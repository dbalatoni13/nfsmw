#ifndef WORLD_TRACK_H
#define WORLD_TRACK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"

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

// total size: 0x8
class TopologyCoordinate : public bTNode<TopologyCoordinate> {
  public:
    TopologyCoordinate() {}
    ~TopologyCoordinate() {}
    int HasTopology(const bVector2 *position);
    float GetElevation(const bVector3 *position, TerrainType *type, bVector3 *normal, bool *point_valid);
};

// total size: 0x60
class TrackOBB {
  public:
    void EndianSwap() {
        bPlatEndianSwap(&TypeNameHash);
        bPlatEndianSwap(&Matrix);
        bPlatEndianSwap(&Dims);
    }

    unsigned int TypeNameHash; // offset 0x0, size 0x4
    unsigned int Pad[3];       // offset 0x4, size 0xC
    bMatrix4 Matrix;           // offset 0x10, size 0x40
    bVector3 Dims;             // offset 0x50, size 0x10
};

void EstablishRemoteCaffeineConnection();
int GetNumTrackOBBs();
TrackOBB *GetTrackOBB(int index);
int LoaderTrackOBB(bChunk *chunk);
int UnloaderTrackOBB(bChunk *chunk);

#endif
