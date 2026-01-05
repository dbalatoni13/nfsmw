#include "ResourceLoader.hpp"
#include "LZCompress.hpp"
#include "Platform.h"
#include "QueuedFile.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/World/WCollisionAssets.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
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

void ResourceFile::AllocateMemory(bool loading_compressed_file) {
    if (SizeofChunks == 0) {
        return;
    }
    // TODO magic flags
    int allocation_params = AllocationParams;
    if (Flags & 2) {
        allocation_params |= 0x40;
    }
    int pool_num = bMemoryGetPoolNum(allocation_params);
    if (loading_compressed_file) {
        allocation_params |= 0x40;
        if (pool_num != 0) {
            pool_num = 0;
            allocation_params &= ~0xF;
        }
    }
    if (pool_num != 0 && SizeofChunks > bLargestMalloc(allocation_params)) {
        allocation_params &= ~0xF;
    }
    pFirstChunk = reinterpret_cast<bChunk *>(bMalloc(SizeofChunks, AllocationName, 0, allocation_params));
}

// STRIPPED
bool ResourceFile::IsFreeMemoryEnabled() {
    return false;
}

// STRIPPED
void ResourceFile::SetFreeMemoryEnabled(bool enable) {}

void ResourceFile::FreeMemory() {
    if (mEnableFreeMemory && pFirstChunk) {
        bFree(pFirstChunk);
        pFirstChunk = nullptr;
    }
}

int NumResourcesBeingLoaded;

void ResourceFile::BeginLoading(void (*callback)(void *), void *callback_param) {
    Callback = callback;
    CallbackParam = callback_param;
    NumResourcesBeingLoaded++;
    if (!GetMemory()) {
        bool loading_compressed_file = (Flags >> 3) & 1;
        AllocateMemory(loading_compressed_file);
    }
    if (SizeofChunks != 0) {
        FileTransfersInProgress++;
        AddQueuedFile(pFirstChunk, Filename, FileOffset, SizeofChunks, FileTransferCallback, this, nullptr);
    }
}

void ResourceFile::ManualUnload() {
    UnloadChunks(pFirstChunk, SizeofChunks, GetFilename());
    pFirstChunk = nullptr;
}

void ResourceFile::ManualReload(bChunk *new_chunks) {
    pFirstChunk = new_chunks;
    LoadChunks(new_chunks, SizeofChunks, GetFilename());
}

ResourceFile::~ResourceFile() {
    if (pFirstChunk) {
        UnloadChunks(pFirstChunk, this->SizeofChunks, GetFilename());
        FreeMemory();
    }
    bFreeSharedString(Filename);
    bFreeSharedString(HotFilename);
    bFreeSharedString(AllocationName);
}

void ResourceFile::LoadResourceIfFileTransferFinished() {
    if (LoadingFinishedFlag || FileTransfersInProgress) {
        return;
    }
    if (Flags & 8) {
        LZHeader *header = reinterpret_cast<LZHeader *>(pFirstChunk);
        if (header) {
            bPlatEndianSwap(&header->ID);
            bPlatEndianSwap(&header->Flags);
            bPlatEndianSwap(&header->UncompressedSize);
            bPlatEndianSwap(&header->CompressedSize);
            if (LZValidHeader(header)) {
                uint8 *compressed_data = reinterpret_cast<uint8 *>(pFirstChunk);
                SizeofChunks = header->UncompressedSize;
                pFirstChunk = nullptr;
                if (SizeofChunks != 0) {
                    AllocateMemory(false);
                    LZDecompress(compressed_data, reinterpret_cast<uint8 *>(pFirstChunk));
                }
                bFree(compressed_data);
            }
            goto block_8;
        }
    } else {
    block_8:
        if (pFirstChunk) {
            EndianSwapChunkHeadersRecursive(pFirstChunk, SizeofChunks);
        }
    }
    LoadTempPermChunks(&pFirstChunk, &SizeofChunks, AllocationParams, AllocationName);
    if (Flags & 4) {
        if (pFirstChunk) {
            FreeMemory();
        }
        SizeofChunks = 0;
    }
    LoadingFinishedFlag = 1;
}

void ResourceFile::FileTransferCallback(void *param, int error_status) {
    reinterpret_cast<ResourceFile *>(param)->FileTransfersInProgress--;
}

// STRIPPED
int ResourceFile::GetSize(int chunk_id, int *pnum_chunks) {
    return 0;
}

bTList<ResourceFile> ResourceFileList;

ResourceFile *CreateResourceFile(const char *filename, ResourceFileType type, int flags, int flag_offset, int file_size) {
    ResourceFile *r = new ResourceFile(filename, type, flags, flag_offset, file_size);
    ResourceFileList.AddTail(r);
    return r;
}

ResourceFile *LoadResourceFile(const char *filename, enum ResourceFileType type, int flags, void (*callback)(void *), void *callback_param,
                               int file_offset, int file_size) {
    ResourceFile *r = CreateResourceFile(filename, type, flags, file_offset, file_size);
    r->BeginLoading(callback, callback_param);
    return r;
}

void UnloadResourceFile(ResourceFile *resource_file) {
    if (!resource_file) {
        return;
    }
    while (!resource_file->IsFinishedLoading()) {
        ServiceResourceLoading();
    }
    ResourceFileList.Remove(resource_file);
    delete resource_file;
}

// total size: 0x8
struct DelayedResourceCallback {
    void (*pCallback)(void *); // offset 0x0, size 0x4
    void *Param;               // offset 0x4, size 0x4
};

int NumDelayedResourceCallbacks;
DelayedResourceCallback DelayedResourceCallbacks[8];

int ServiceResourceLoading() {
    ProfileNode profile_node("TODO", 0);

    while (NumDelayedResourceCallbacks != 0) {
        ProfileNode profile_node("TODO2", 0);
        // TODO registers instead of stack
        DelayedResourceCallback drc = DelayedResourceCallbacks[0];
        // drc.pCallback = DelayedResourceCallbacks[0].pCallback;
        // drc.Param = DelayedResourceCallbacks[0].Param;
        if (NumDelayedResourceCallbacks > 1) {
            bOverlappedMemCpy(&DelayedResourceCallbacks[0], &DelayedResourceCallbacks[1],
                              NumDelayedResourceCallbacks * sizeof(DelayedResourceCallback));
        }
        NumDelayedResourceCallbacks--;
        drc.pCallback(drc.Param);
    }
    ServiceQueuedFiles();
    if (NumResourcesBeingLoaded != 0) {
        for (ResourceFile *resource_file = ResourceFileList.GetHead(); resource_file != ResourceFileList.EndOfList();
             resource_file = resource_file->GetNext()) {
            if (!resource_file->IsFinishedLoading()) {
                resource_file->LoadResourceIfFileTransferFinished();
                if (resource_file->IsFinishedLoading()) {
                    NumResourcesBeingLoaded--;
                    resource_file->CallCallback();
                }
                return NumResourcesBeingLoaded;
            }
        }
    }
    return false;
}

int IsResourceLoadingComplete() {
    return NumResourcesBeingLoaded == 0;
}

void WaitForResourceLoadingComplete() {
    ServiceResourceLoading();
    while (!IsResourceLoadingComplete()) {
        DVDErrorTask(nullptr, 0);
        bThreadYield(8);
        ServiceResourceLoading();
    }
}

void SetDelayedResourceCallback(void (*callback)(void *), void *param) {
    DelayedResourceCallback *drc = &DelayedResourceCallbacks[NumDelayedResourceCallbacks];
    drc->pCallback = callback;
    drc->Param = param;
    NumDelayedResourceCallbacks++;
}

// STRIPPED
ResourceFile *FindResourceFile(const char *filename) {
    return nullptr;
}

ResourceFile *FindResourceFile(ResourceFileType type) {
    for (ResourceFile *resource_file = ResourceFileList.GetTail(); resource_file != ResourceFileList.EndOfList();
         resource_file = resource_file->GetPrev()) {
        if (resource_file->GetType() == type) {
            return resource_file;
        }
    }
    return nullptr;
}

int CurrentlyHotChunking;

bool IsCurrentlyHotChunking() {
    return CurrentlyHotChunking;
}

int LoaderWCollisionPack(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_W_COLLISION_ASSETS) {
        WCollisionAssets::Get().LoadCollisionPack(chunk);
        return true;
    }
    return false;
}

int UnloaderWCollisionPack(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_W_COLLISION_ASSETS) {
        WCollisionAssets::Get().UnLoadCollisionPack(chunk);
        return true;
    }
    return false;
}

int LoaderColourCube(bChunk *chunk) {
    return chunk->GetID() == BCHUNK_COLOUR_CUBE;
}

int UnloaderColourCube(bChunk *chunk) {
    return chunk->GetID() == BCHUNK_FENG_FONT;
}

// total size: 0x4C
struct VMFile {
    VMFile();

    bool mInit;          // offset 0x0, size 0x1
    char mFilename[48];  // offset 0x4, size 0x30
    bool mCompressed;    // offset 0x34, size 0x1
    int mSize;           // offset 0x38, size 0x4
    int mSizeOfChunks;   // offset 0x3C, size 0x4
    void *mMainMemAddr;  // offset 0x40, size 0x4
    void *mVirtMemAddr;  // offset 0x44, size 0x4
    bool mUsedTrackPool; // offset 0x48, size 0x1
};

VMFile::VMFile() {
    mInit = false;
    mCompressed = false;
    mSize = 0;
    mSizeOfChunks = 0;
    mVirtMemAddr = nullptr;
    mUsedTrackPool = false;
    bMemSet(mFilename, 0, sizeof(mFilename));
}

VMFile queued_vm_files[5];

VMFile *GetVMFile() {
    for (int i = 0; i < 5; i++) {
        if (!queued_vm_files[i].mInit) {
            return &queued_vm_files[i];
        }
    }
    return nullptr;
}

// UNSOLVED regswap
void MoveFileIntoVirtualMemoryThenLoadChunks(int param, int err) {
    VMFile *vm_file = reinterpret_cast<VMFile *>(param);
    if (!vm_file->mInit) {
        return;
    }
    void *old_memory = vm_file->mMainMemAddr;
    int vm_file_size = vm_file->mSize;
    unsigned int sizeofchunks = vm_file_size;
    if (vm_file->mCompressed != 0) {
        LZHeader *header = reinterpret_cast<LZHeader *>(old_memory);
        bPlatEndianSwap(&header->ID);
        bPlatEndianSwap(&header->Flags);
        bPlatEndianSwap(&header->UncompressedSize);
        bPlatEndianSwap(&header->CompressedSize);
        if (LZValidHeader(header)) {
            sizeofchunks = header->UncompressedSize;
            uint8 *compressed_data = (uint8_t *)old_memory;
            old_memory = nullptr;
            if (sizeofchunks != 0) {
                int allocation_params = GetVirtualMemoryAllocParams();
                void *realloc = bMalloc(sizeofchunks, "TODO2", 0, allocation_params);
                old_memory = realloc;
                LZDecompress(compressed_data, (uint8 *)old_memory);
            }
            bFree(compressed_data);
        }
    }
    void *new_mem = bMalloc(sizeofchunks, "TODO", 0, GetVirtualMemoryAllocParams());
    bMemCpy(new_mem, old_memory, sizeofchunks);
    vm_file->mVirtMemAddr = new_mem;
    if (vm_file->mUsedTrackPool) {
        TheTrackStreamer.FreeUserMemory(old_memory);
    } else {
        bFree(old_memory);
    }
    if (new_mem) {
        EndianSwapChunkHeadersRecursive((bChunk *)new_mem, sizeofchunks);
    }
    LoadChunks((bChunk *)new_mem, sizeofchunks, vm_file->mFilename);
    vm_file->mSizeOfChunks = sizeofchunks;
}

void UnloadFileFromVirtualMemory(VMFile *vm_file) {
    UnloadChunks((bChunk *)vm_file->mVirtMemAddr, vm_file->mSizeOfChunks, vm_file->mFilename);
    bFree(vm_file->mVirtMemAddr);
    vm_file->mFilename[0] = '\0';
    vm_file->mSize = 0;
    vm_file->mCompressed = false;
    vm_file->mMainMemAddr = nullptr;
    vm_file->mInit = false;
    vm_file->mUsedTrackPool = false;
    vm_file->mVirtMemAddr = nullptr;
    vm_file->mSizeOfChunks = 0;
}

VMFile *LoadFileIntoVirtualMemory(const char *filename, bool compressed, bool use_trackstreampool_as_temp) {
    VMFile *vm_file = GetVMFile();
    if (!vm_file) {
        return nullptr;
    }

    vm_file->mInit = true;
    bStrNCpy(vm_file->mFilename, filename, sizeof(vm_file->mFilename));
    char temp_name[128];
    bSPrintf(temp_name, "TEMP[%s]", filename);
    int vm_file_size = GetQueuedFileSize(filename);
    void *memory_file;
    if (use_trackstreampool_as_temp) {
        memory_file = TheTrackStreamer.AllocateUserMemory(vm_file_size, temp_name, 0);
    } else {
        memory_file = bMalloc(vm_file_size, "TODO", 0, 0x2040);
    }
    vm_file->mCompressed = compressed;
    vm_file->mUsedTrackPool = use_trackstreampool_as_temp;
    vm_file->mSize = vm_file_size;
    vm_file->mMainMemAddr = memory_file;
    AddQueuedFile(memory_file, filename, 0, vm_file_size, MoveFileIntoVirtualMemoryThenLoadChunks, reinterpret_cast<int>(vm_file), nullptr);

    return vm_file;
}
