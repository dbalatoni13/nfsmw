#ifndef WORLD_TRACK_H
#define WORLD_TRACK_H

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/Src/World/TerrainType.hpp"

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
