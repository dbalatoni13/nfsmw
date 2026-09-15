#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

int bChunkLoaderFunctionNull(bChunk *chunk) {
    return static_cast<int>(chunk->GetID() == 0);
}

bChunkLoader bChunkLoaderNull(0, bChunkLoaderFunctionNull, bChunkLoaderFunctionNull);

bChunkLoader::bChunkLoader(unsigned int id, int (*loader)(bChunk *), int (*unloader)(bChunk *)) {
    this->LoaderFunction = loader;
    this->UnloaderFunction = unloader;

    int hash = bChunkLoader::GetHash(id);
    this->ID = id;
    this->Next = bChunkLoader::sLoaderTable[hash];
    bChunkLoader::sLoaderTable[hash] = this;
    bChunkLoader::sNumLoaders[hash]++;
}

bChunkLoader *bChunkLoader::FindLoader(unsigned int id) {
    int hash = bChunkLoader::GetHash(id);
    bChunkLoader *loader = bChunkLoader::sLoaderTable[hash];
    while (loader != nullptr) {
        if (loader->ID == id) {
            return loader;
        }
        loader = loader->Next;
    }
    return nullptr;
}

// STRIPPED
unsigned int bChunkLoader::CallLoaders(bChunk *chunks, int sizeof_chunks, bool abort_on_error) {
    for (bChunk *chunk = chunks; chunk != GetLastChunk(chunk, sizeof_chunks); chunk = chunks->GetNext()) {
        bChunkLoader *loader = FindLoader(chunk->GetID());
        if (loader != nullptr) {
            if (loader->GetLoaderFunction()(chunk)) {
                continue;
            }
        }
        if (abort_on_error) {
            return chunk->GetID();
        }
    }

    return 0;
}

// STRIPPED
unsigned int bChunkLoader::CallUnloaders(bChunk *chunks, int sizeof_chunks, bool abort_on_error) {}

void bChunkCarpHeader::PlatformEndianSwap() {
    bPlatEndianSwap(&this->mCrpSize);
    bPlatEndianSwap(&this->mSectionNumber);
    bPlatEndianSwap(&this->mFlags);
}

// STRIPPED
bool bValidateChunksRecursive(bChunk *the_chunk, int32 expected_level_size, int32 *current_offset, long max_size, bChunk *min_pointer,
                              bChunk *max_pointer) {}

// STRIPPED
bool bValidateChunks(bChunk *first_chunk, int sizeof_chunks) {}
