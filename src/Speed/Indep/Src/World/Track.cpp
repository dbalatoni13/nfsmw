#include "Track.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"

void bEndianSwap32(void *value);

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

void EstablishRemoteCaffeineConnection() {}

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
    chunk = chunk;
    NumTrackOBBs = 0;
    TrackOBBTable = 0;
    return 1;
}
