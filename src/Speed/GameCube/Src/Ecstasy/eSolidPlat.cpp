#include "Speed/Indep/bWare/Inc/bChunk.hpp"

int eLoadSolidListPlatChunks(bChunk *chunk) {
    bChunk *current_chunk;
    bChunk *last_chunk = chunk;

    for (current_chunk = last_chunk->GetFirstChunk(); current_chunk < last_chunk->GetLastChunk(); current_chunk = current_chunk->GetNext()) {
    }
    return 1;
}

int eUnloadSolidListPlatChunks(bChunk *chunk) {
    bChunk *current_chunk;
    bChunk *last_chunk = chunk;

    for (current_chunk = last_chunk->GetFirstChunk(); current_chunk < last_chunk->GetLastChunk(); current_chunk = current_chunk->GetNext()) {
    }
    return 1;
}
