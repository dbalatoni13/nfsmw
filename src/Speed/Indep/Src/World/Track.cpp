#include "Track.hpp"

#include "Speed/Indep/bWare/Inc/bChunk.hpp"

void bEndianSwap32(void *value);

static char *TrackOBBTable = 0;
static int NumTrackOBBs = 0;

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

    TrackOBBTable = chunk->GetAlignedData(16);
    NumTrackOBBs = chunk->GetAlignedSize(16) / 0x60;
    for (int i = 0; i < NumTrackOBBs; i++) {
        int *obb_words = reinterpret_cast<int *>(TrackOBBTable + i * 0x60);
        for (int j = 0; j < 24; j++) {
            bEndianSwap32(&obb_words[j]);
        }
    }

    return 1;
}

int UnloaderTrackOBB(bChunk *chunk) {
    chunk = chunk;
    NumTrackOBBs = 0;
    TrackOBBTable = 0;
    return 1;
}
