#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

int bChunkLoaderFunctionNull(bChunk *chunk) {
    return chunk->GetID() == 0;
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
    bChunkLoader *loader = bChunkLoader::sLoaderTable[bChunkLoader::GetHash(id)];
    while (loader) {
        if (loader->ID == id) {
            return loader;
        }
        loader = loader->Next;
    }
    return nullptr;
}

void bChunkCarpHeader::PlatformEndianSwap() {
    bPlatEndianSwap(&this->mCrpSize);
    bPlatEndianSwap(&this->mSectionNumber);
    bPlatEndianSwap(&this->mFlags);
}
