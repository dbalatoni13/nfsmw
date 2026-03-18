#include "Track.hpp"

#include "Speed/Indep/bWare/Inc/bChunk.hpp"

static char *TrackOBBTable = 0;
static int NumTrackOBBs = 0;

void EstablishRemoteCaffeineConnection() {}

int GetNumTrackOBBs() {
    return NumTrackOBBs;
}

TrackOBB *GetTrackOBB(int index) {
    if (index < 0 || index >= NumTrackOBBs || !TrackOBBTable) {
        return 0;
    }

    return reinterpret_cast<TrackOBB *>(TrackOBBTable + index * 0x60);
}

int LoaderTrackOBB(bChunk *chunk) {
    NumTrackOBBs = chunk->Size / 0x60;
    TrackOBBTable = chunk->GetData();
    return 1;
}

int UnloaderTrackOBB(bChunk *chunk) {
    chunk = chunk;
    NumTrackOBBs = 0;
    TrackOBBTable = 0;
    return 1;
}
