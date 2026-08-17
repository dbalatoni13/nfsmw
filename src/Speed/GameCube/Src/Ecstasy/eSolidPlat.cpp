#include "Speed/Indep/bWare/Inc/bChunk.hpp"

// UNSOLVED
int eLoadSolidListPlatChunks(bChunk *chunk) {
    bChunk *current_chunk;
    bChunk *last_chunk = chunk;

    current_chunk = last_chunk->GetFirstChunk();
    if (current_chunk < last_chunk->GetLastChunk()) {
        do {
            current_chunk = current_chunk->GetNext();
        } while (current_chunk < last_chunk->GetLastChunk());
    }
    return 1;
}

// UNSOLVED
int eUnloadSolidListPlatChunks(bChunk *chunk) {
    bChunk *current_chunk;
    bChunk *last_chunk = chunk;

    current_chunk = last_chunk->GetFirstChunk();
    if (current_chunk < last_chunk->GetLastChunk()) {
        do {
            current_chunk = current_chunk->GetNext();
        } while (current_chunk < last_chunk->GetLastChunk());
    }
    return 1;
}
