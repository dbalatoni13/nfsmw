#pragma once

#include "Speed/Indep/bWare/Inc/bChunk.hpp"

void EndianSwapChunkHeader(bChunk *chunk);
void EndianSwapChunkHeadersRecursive(bChunk *chunks, int sizeof_chunks);
void EndianSwapChunkHeadersRecursive(bChunk *first_chunk, bChunk *last_chunk);
