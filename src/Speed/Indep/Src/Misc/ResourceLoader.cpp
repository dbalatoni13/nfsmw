#include "ResourceLoader.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "SpeedChunks.hpp"

#define LOADER_AMOUNT (26)

bChunkLoaderFunction LoaderTable[LOADER_AMOUNT];
bChunkLoaderFunction UnloaderTable[LOADER_AMOUNT];

// UNSOLVED
int LoaderStub(bChunk *chunk) {
    // TODO magic, put those into SpeedChunks.hpp
    if (chunk->ID != BCHUNK_STYLE_MOMENTS_INFO) {
        if (chunk->ID < BCHUNK_STYLE_PARTITIONS) {
            if (chunk->ID != BCHUNK_SMOKEABLES) {
                return 0;
            }
        } else if (chunk->ID != 0x34b00) {
            return 0;
        }
    } else {
        return 1;
    }
    return 0;
}

int CallChunkLoader(bChunk *chunk) {
    if (chunk->GetID() == 0) {
        return 1;
    }
    bChunkLoader *loader = bChunkLoader::FindLoader(chunk->ID);
    if (loader) {
        int result = loader->GetLoaderFunction()(chunk);
        return result;
    }
    for (int loader_num = 0; loader_num < LOADER_AMOUNT; loader_num++) {
        bChunkLoaderFunction loader_function = LoaderTable[loader_num];
        if (loader_function(chunk)) {
            return 1;
        }
    }
    return 0;
}

int CallChunkUnloader(bChunk *chunk) {
    if (chunk->GetID() == 0) {
        return 1;
    }
    bChunkLoader *loader = bChunkLoader::FindLoader(chunk->ID);
    if (loader) {
        int result = loader->GetUnloaderFunction()(chunk);
        return result;
    }
    for (int loader_num = 0; loader_num < LOADER_AMOUNT; loader_num++) {
        bChunkLoaderFunction loader_function = UnloaderTable[loader_num];
        if (loader_function(chunk)) {
            return 1;
        }
    }
    return 0;
}

void PostLoadFixup();

void LoadChunks(bChunk *chunks, int sizeof_chunks, const char *debug_name) {
    ProfileNode profile_node(debug_name, 0);
    bChunk *last_chunk = GetLastChunk(chunks, sizeof_chunks);

    for (bChunk *chunk = chunks; chunk < last_chunk; chunk = chunk->GetNext()) {
        uint32 start_time = bGetTicker();
        if (CallChunkLoader(chunk) == 0) {
            bBreak();
        }
    }
    PostLoadFixup();
}

void UnloadChunks(bChunk *chunks, int sizeof_chunks, const char *debug_name) {
    ProfileNode profile_node(debug_name, 0);
    eWaitUntilRenderingDone();
    bChunk *first_chunk = chunks;
    bChunk *last_chunk = GetLastChunk(chunks, sizeof_chunks);

    while (first_chunk < last_chunk) {
        int num_prev_chunks = 0;
        const int max_prev_chunks = 64;
        bChunk *prev_chunk_table[64];
        for (bChunk *chunk = first_chunk; chunk < last_chunk; chunk = chunk->GetNext()) {
            prev_chunk_table[num_prev_chunks % max_prev_chunks] = chunk;
            num_prev_chunks++;
        }
        int num_chunks = num_prev_chunks;
        if (num_chunks > max_prev_chunks) {
            num_chunks = max_prev_chunks;
        }
        for (int n = 0; n < num_chunks; n++) {
            bChunk *chunk = prev_chunk_table[(num_prev_chunks - 1 - n) % max_prev_chunks];
            const char *chunkname = GetChunkName(chunk->GetID());
            uint32 start_time = bGetTicker();
            if (CallChunkLoader(chunk) == 0) {
                bBreak();
            }
            last_chunk = chunk;
        }
    }
    PostLoadFixup();
}

void ScratchPadMemCpy(void *dest, const void *src, unsigned int numbytes) {
    bOverlappedMemCpy(dest, src, numbytes);
}

float MoveChunkMemcpyTime;
float MoveChunkMemcpyAmount; // BUG (float)

void MoveChunksRange(bChunk *source_chunks, int sizeof_chunks, int movement_offset, const char *debug_name) {
    UnloadChunks(source_chunks, sizeof_chunks, debug_name);
    int start_ticks = bGetTicker();
    bChunk *dest_chunks = reinterpret_cast<bChunk *>(reinterpret_cast<char *>(source_chunks) + movement_offset);

    ScratchPadMemCpy(dest_chunks, source_chunks, sizeof_chunks);

    float time = bGetTickerDifference(start_ticks);
    MoveChunkMemcpyTime += time;
    MoveChunkMemcpyAmount += sizeof_chunks;
    LoadChunks(dest_chunks, sizeof_chunks, debug_name);
}

extern int ChunkMovementOffset;

void MoveChunks(bChunk *dest_chunks, bChunk *source_chunks, int sizeof_chunks, const char *debug_name) {
    int num_chunk_ranges = 0;
    bChunk *chunk_range_table[128];
    int current_chunk_class = -1;
    bChunk *last_chunk = GetLastChunk(source_chunks, sizeof_chunks);
    for (bChunk *chunk = source_chunks; chunk < last_chunk; chunk = chunk->GetNext()) {
        uint32 chunk_id = chunk->GetID();
        int chunk_class = 0;
        if (chunk_id == 0) {
            chunk_class = current_chunk_class;
        } else {
            if (chunk_id == BCHUNK_GEOMETRY_PACK) {
                chunk_class = 1;
            } else if (chunk_id == BCHUNK_TEXTURE_PACK || chunk_id == BCHUNK_TEXTURE_ANIM_PACK) {
                chunk_class = 2;
            }
        }
        if (chunk_class == -1) {
            chunk_class = 0;
        }
        if (chunk_class != current_chunk_class || chunk_id == BCHUNK_GEOMETRY_PACK || chunk_id == BCHUNK_TEXTURE_PACK) {
            chunk_range_table[num_chunk_ranges] = chunk;
            num_chunk_ranges++;
            current_chunk_class = chunk_class;
        }
    }
    int movement_offset = (uintptr_t)dest_chunks - (uintptr_t)source_chunks;
    ChunkMovementOffset = movement_offset;
    chunk_range_table[num_chunk_ranges] = last_chunk;
    if (movement_offset < 0) {
        for (int n = 0; n < num_chunk_ranges; n++) {
            int size = (uintptr_t)chunk_range_table[n + 1] - (uintptr_t)chunk_range_table[n];
            MoveChunksRange(chunk_range_table[n], size, movement_offset, debug_name);
        }
    } else {
        for (int n = num_chunk_ranges - 1; n > -1; n--) {
            int size = (uintptr_t)chunk_range_table[n + 1] - (uintptr_t)chunk_range_table[n];
            MoveChunksRange(chunk_range_table[n], size, movement_offset, debug_name);
        }
    }
    ChunkMovementOffset = 0;
}

void LoadEmbeddedChunks(bChunk *chunk, int sizeof_chunks, const char *debug_name) {
    EndianSwapChunkHeadersRecursive(chunk, sizeof_chunks);
    LoadChunks(chunk, sizeof_chunks, debug_name);
}

void EndianSwapChunkHeader(bChunk *chunk) {
    bPlatEndianSwap(&chunk->ID);
    bPlatEndianSwap(&chunk->Size);
}

void EndianSwapChunkHeadersRecursive(bChunk *chunks, int sizeof_chunks) {
    bChunk *last_chunk = GetLastChunk(chunks, sizeof_chunks);
    EndianSwapChunkHeadersRecursive(chunks, last_chunk);
}

int PrintChunks;
int PrintChunkLevel;

void EndianSwapChunkHeadersRecursive(bChunk *first_chunk, bChunk *last_chunk) {
    for (bChunk *chunk = first_chunk; chunk < last_chunk; chunk = chunk->GetNext()) {
        EndianSwapChunkHeader(chunk);
        if (chunk->IsNestedChunk()) {
            PrintChunkLevel++;
            EndianSwapChunkHeadersRecursive(chunk->GetFirstChunk(), chunk->GetLastChunk());
            PrintChunkLevel--;
        }
    }
}

bool IsTempChunk(bChunk *chunk) {
    unsigned int chunk_id = chunk->GetID();
    return false;
}

int SplitPermTempChunks(bool split_temp, bChunk *source_chunks, int source_chunks_size, uint8 *dest_buffer, int dest_position, int depth) {
    depth++;
    if (depth > 10) {
        bBreak();
    }
    const int alignment_amount = 0x80;
    const int alignment_mask = alignment_amount - 1;

    for (bChunk *source_chunk = source_chunks; source_chunk < GetLastChunk(source_chunks, source_chunks_size);
         source_chunk = source_chunk->GetNext()) {
        bool is_temp_chunk = IsTempChunk(source_chunk);
        if (is_temp_chunk == split_temp) {
            intptr_t source_position = reinterpret_cast<intptr_t>(source_chunk);
            int pad_size = source_position - dest_position & alignment_mask;
            if ((pad_size != 0) && (pad_size < 9)) {
                pad_size += alignment_amount;
            }
            if (dest_buffer && pad_size != 0) {
                bChunk *dest_chunk = reinterpret_cast<bChunk *>(dest_buffer + dest_position);
                dest_chunk->ID = 0;
                dest_chunk->Size = pad_size - sizeof(bChunk);
                bMemSet(&dest_chunk[1], 0x11, pad_size - sizeof(bChunk));
            }
            dest_position += pad_size;
        }
        if (source_chunk->IsNestedChunk() && !is_temp_chunk) {
            int new_dest_position = SplitPermTempChunks(split_temp, source_chunk->GetFirstChunk(), source_chunk->GetSize(), dest_buffer,
                                                        dest_position + sizeof(bChunk), depth);
            if (new_dest_position != dest_position + sizeof(bChunk)) {
                if (dest_buffer) {
                    bChunk *dest_chunk = reinterpret_cast<bChunk *>(dest_buffer + dest_position);
                    dest_chunk->ID = source_chunk->ID;
                    dest_chunk->Size = new_dest_position - dest_position - sizeof(bChunk);
                }
                dest_position = new_dest_position;
            }
        } else if (is_temp_chunk == split_temp) {
            if (dest_buffer) {
                bChunk *dest_chunk = reinterpret_cast<bChunk *>(dest_buffer + dest_position);
                bMemCpy(dest_chunk, source_chunk, source_chunk->GetSize() + sizeof(bChunk));
            }
            dest_position += 8 + source_chunk->GetSize();
        }
    }
    return dest_position;
}

void ClobberPermChunks(bChunk *source_chunks, int source_chunks_size) {
    bChunk *source_chunk = source_chunks;
    bChunk *last_source_chunk = GetLastChunk(source_chunk, source_chunks_size);
    while (source_chunk < last_source_chunk) {
        bChunk *next_source_chunk = source_chunk->GetNext();
        bool is_temp_chunk = IsTempChunk(source_chunk);
        if (source_chunk->IsNestedChunk()) {
            if (!is_temp_chunk) {
                ClobberPermChunks(source_chunk->GetFirstChunk(), source_chunk->GetSize());
                source_chunk->Size = 0;
            }
        }
        if (!is_temp_chunk) {
            source_chunk->ID = 0;
        }
        source_chunk = next_source_chunk;
    }
}

bool LoadTempPermChunks(bChunk **ppchunks, int *psizeof_chunks, int allocation_params, const char *debug_name) {
    bChunk *chunks = *ppchunks;
    int sizeof_chunks = *psizeof_chunks;
    if (!chunks || sizeof_chunks == 0) {
        return false;
    }
    int sizeof_perm_chunks = SplitPermTempChunks(false, chunks, sizeof_chunks, nullptr, 0, 0);
    int sizeof_temp_chunks = sizeof_chunks;
    if (SplitPermTempChunks(true, chunks, sizeof_chunks, nullptr, 0, 0) == 0) {
        LoadChunks(chunks, sizeof_chunks, debug_name);
        return false;
    } else {
        bChunk *perm_chunks = (bChunk *)bMalloc(sizeof_perm_chunks, "TODO", __LINE__, allocation_params);
        bChunk *temp_chunks = chunks;

        SplitPermTempChunks(false, temp_chunks, sizeof_chunks, reinterpret_cast<uint8 *>(perm_chunks), 0, 0);
        ClobberPermChunks(temp_chunks, sizeof_chunks);
        LoadChunks(temp_chunks, sizeof_chunks, debug_name);
        LoadChunks(perm_chunks, sizeof_perm_chunks, debug_name);
        UnloadChunks(temp_chunks, sizeof_chunks, debug_name);
        bFree(temp_chunks);
        *psizeof_chunks = sizeof_perm_chunks;
        *ppchunks = perm_chunks;
        return true;
    }
}

// TODO
extern bool PostLoadFixupDisabled;
extern int eDirtyTextures;

void PostLoadFixup() {
    if (!PostLoadFixupDisabled) {
        bGetTicker();
        if (eDirtyTextures != 0) {
            eLightUpdateTextures();
        }
        eFixUpTables();
    }
}

// STRIPPED
void HotChunksPostLoadFixup() {}

SlotPool *ResourceFileSlotPool;

void InitResourceLoader() {
    ResourceFileSlotPool = bNewSlotPool(80, 80, "ResourceFileSlotPool", 0);
}

// STRIPPED
void CloseResourceLoader() {}

ResourceFile::ResourceFile(const char *filename, ResourceFileType type, int flags, int file_offset, int file_size) {
    Flags = flags;
    FileOffset = file_offset;
    FileSize = file_size;
    mEnableFreeMemory = true;
    Type = type;
    Filename = bAllocateSharedString(filename);
    HotFilename = nullptr;
    FileTransfersInProgress = 0;
    LoadingFinishedFlag = 0;
    HotFileNumber = 0;
    pLoadedHotFileEntries = nullptr;
    NumLoadedHotFileEntries = 0;
    Callback = nullptr;
    CallbackParam = nullptr;
    SetAllocationParams(0x2000, filename);
    HotFilename = nullptr;
    SizeofChunks = bFileSize(Filename);
    if (SizeofChunks < 0) {
        SizeofChunks = 0;
    }
    if (SizeofChunks != 0 && FileSize != 0) {
        SizeofChunks = FileSize;
    }
    pFirstChunk = nullptr;
}

void ResourceFile::SetAllocationParams(int allocation_params, const char *debug_name) {
    AllocationParams = allocation_params;
    AllocationName = bAllocateSharedString(debug_name);
}
