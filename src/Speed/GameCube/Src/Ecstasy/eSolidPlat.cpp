#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

bTList<eSolid> SolidList; // size: 0x8, address: 0x8045ED18, Decl: speed/ps3/src/ecstasy/eSolidPlat.cpp:23

// UNSOLVED
int eLoadSolidListPlatChunks(bChunk *chunk) {
    bChunk *current_chunk;
    bChunk *last_chunk = chunk;

    for (current_chunk = last_chunk->GetFirstChunk(); current_chunk < last_chunk->GetLastChunk(); current_chunk = current_chunk->GetNext()) {
    }
    return 1;
}

// UNSOLVED
int eUnloadSolidListPlatChunks(bChunk *chunk) {
    bChunk *current_chunk;
    bChunk *last_chunk = chunk;

    for (current_chunk = last_chunk->GetFirstChunk(); current_chunk < last_chunk->GetLastChunk(); current_chunk = current_chunk->GetNext()) {
    }
    return 1;
}
