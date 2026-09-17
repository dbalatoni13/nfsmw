#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#ifdef EA_PLATFORM_WIN32
extern "C" __declspec(dllimport) int __stdcall IsBadReadPtr(const void *address, unsigned long size);
#endif

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

unsigned int bChunkLoader::CallLoaders(bChunk *chunks, int sizeof_chunks, bool abort_on_error) {
    bChunk *last_chunk = GetLastChunk(chunks, sizeof_chunks);
    for (bChunk *chunk = chunks; chunk < last_chunk;
         chunk = reinterpret_cast<bChunk *>(reinterpret_cast<char *>(chunk) + chunk->GetSize() + sizeof(*chunk))) {
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

unsigned int bChunkLoader::CallUnloaders(bChunk *chunks, int sizeof_chunks, bool abort_on_error) {
    bChunk *last_chunk = GetLastChunk(chunks, sizeof_chunks);
    for (bChunk *chunk = chunks; chunk < last_chunk;
         chunk = reinterpret_cast<bChunk *>(reinterpret_cast<char *>(chunk) + chunk->GetSize() + sizeof(*chunk))) {
        bChunkLoader *loader = FindLoader(chunk->GetID());
        if (loader != nullptr) {
            if (loader->GetUnloaderFunction()(chunk)) {
                continue;
            }
        }
        if (abort_on_error) {
            return chunk->GetID();
        }
    }

    return 0;
}

void bChunkCarpHeader::PlatformEndianSwap() {
    bPlatEndianSwap(&this->mCrpSize);
    bPlatEndianSwap(&this->mSectionNumber);
    bPlatEndianSwap(&this->mFlags);
}

bool bValidateChunksRecursive(bChunk *the_chunk, int32 expected_level_size, int32 *current_offset, long max_size, bChunk *min_pointer,
                              bChunk *max_pointer) {
#ifdef EA_PLATFORM_WIN32
    if (IsBadReadPtr(the_chunk, 1)) {
        return false;
    }
#else
    if (the_chunk == nullptr) {
        return false;
    }
#endif

    const uintptr_t chunk_address = reinterpret_cast<uintptr_t>(the_chunk);
    if (chunk_address < reinterpret_cast<uintptr_t>(min_pointer) || chunk_address >= reinterpret_cast<uintptr_t>(max_pointer)) {
        return false;
    }

    int level_offset = 0;
    while (reinterpret_cast<uintptr_t>(the_chunk) < reinterpret_cast<uintptr_t>(max_pointer)) {
        if (level_offset >= expected_level_size) {
            return false;
        }

        const int chunk_size = the_chunk->GetSize();
        if (*current_offset + chunk_size + static_cast<int>(sizeof(*the_chunk)) > max_size) {
            return false;
        }

        *current_offset += sizeof(*the_chunk);
        if (the_chunk->IsNestedChunk()) {
            if (!bValidateChunksRecursive(the_chunk->GetFirstChunk(), chunk_size, current_offset, max_size, min_pointer, max_pointer)) {
                return false;
            }
        } else {
            *current_offset += chunk_size;
        }

        level_offset += chunk_size + sizeof(*the_chunk);
        the_chunk = reinterpret_cast<bChunk *>(reinterpret_cast<char *>(the_chunk) + chunk_size + sizeof(*the_chunk));
    }

    return level_offset == expected_level_size;
}

bool bValidateChunks(bChunk *first_chunk, int sizeof_chunks) {
    int current_offset = 0;
    return bValidateChunksRecursive(first_chunk, sizeof_chunks, &current_offset, sizeof_chunks, first_chunk,
                                    GetLastChunk(first_chunk, sizeof_chunks));
}
