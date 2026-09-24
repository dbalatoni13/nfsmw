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
#include "Speed/Indep/bWare/Inc/bTypes.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "SpeedChunks.hpp"

static const int PrintChunkLoading = 0; // Decl: 31

static const int PrintChunkLoadingTime = 0; // Decl: 33

static const int PrintResourceLoaderMemory = 0; // Decl: 35

int NumResourcesBeingLoaded = 0; // Decl: 37

// total size: 0x8
// Decl: 44
struct DelayedResourceCallback {
    void (*pCallback)(void *); // offset 0x0, size 0x4
    void *Param;               // offset 0x4, size 0x4
};

int NumDelayedResourceCallbacks = 0;                 // Decl: 49
DelayedResourceCallback DelayedResourceCallbacks[8]; // Decl: 50

// STRIPPED
void InitHotChunks() {}

// STRIPPED
void CloseHotChunks() {}

// STRIPPED
void HotChunkMonitor() {}

// STRIPPED
int IsChunkValid(bChunk *chunks, int sizeof_chunks, const char *debug_name) {}

// total size: 0x10
// Decl: 187
struct LoadedHotFileEntry {
    bChunk *pChunk;        // offset 0x0, size 0x4
    int OriginalSize;      // offset 0x4, size 0x4
    int MaxSize;           // offset 0x8, size 0x4
    unsigned int Checksum; // offset 0xC, size 0x4
};

static int LoaderStub(bChunk *chunk) {
    switch (chunk->ID) {
        case BCHUNK_SPEED_SMOKEABLE_INFO:
        case BCHUNK_SPEED_STYLE_MOMENT_TABLE:
        case 0x34b00:
            return 1;
        default:
            return 0;
    }
}

int ChunkMovementOffset = 0; // Decl: 215

int LoaderFEngFont(bChunk *chunk);
int LoaderEventManager(bChunk *chunk);
int LoaderVisibleSections(bChunk *chunk);
int LoaderEAGLAnimations(bChunk *chunk);
int LoaderEAGLSkeletons(bChunk *chunk);
int LoaderAnimSceneData(bChunk *chunk);
int LoaderAnimDirectoryData(bChunk *chunk);
int LoaderWorldAnimEntityData(bChunk *chunk);
int LoaderWorldAnimDirectoryData(bChunk *chunk);
int LoaderWorldAnimTreeMarker(bChunk *chunk);
int LoaderWorldAnimInstanceEntry(bChunk *chunk);
int LoaderSun(bChunk *chunk);
int LoaderCarInfo(bChunk *chunk);
int LoaderLanguage(bChunk *chunk);
int LoaderWeatherMan(bChunk *chunk);
int LoaderFEngPackage(bChunk *chunk);
int LoaderQuickSpline(bChunk *chunk);
int LoaderICECameras(bChunk *chunk);
int LoaderSoundStichs(bChunk *chunk);
int LoaderFEPresetCars(bChunk *chunk);
int LoaderParameterMaps(bChunk *chunk);
int LoaderWCollisionPack(bChunk *chunk);
int LoaderBounds(bChunk *chunk);
int LoaderTrigger(bChunk *chunk);
int LoaderColourCube(bChunk *chunk);

// Decl: 254
bChunkLoaderFunction LoaderTable[26] = {
    LoaderFEngFont,
    LoaderEventManager,
    LoaderVisibleSections,
    LoaderEAGLAnimations,
    LoaderEAGLSkeletons,
    LoaderAnimSceneData,
    LoaderAnimDirectoryData,
    LoaderWorldAnimEntityData,
    LoaderWorldAnimDirectoryData,
    LoaderWorldAnimTreeMarker,
    LoaderWorldAnimInstanceEntry,
    LoaderSun,
    LoaderCarInfo,
    LoaderLanguage,
    LoaderWeatherMan,
    LoaderFEngPackage,
    LoaderQuickSpline,
    LoaderICECameras,
    LoaderSoundStichs,
    LoaderFEPresetCars,
    LoaderParameterMaps,
    LoaderWCollisionPack,
    LoaderBounds,
    LoaderTrigger,
    LoaderColourCube,
    LoaderStub,
};

int UnloaderFEngFont(bChunk *chunk);
int UnloaderEventManager(bChunk *chunk);
int UnloaderVisibleSections(bChunk *chunk);
int UnloaderEAGLAnimations(bChunk *chunk);
int UnloaderEAGLSkeletons(bChunk *chunk);
int UnloaderAnimSceneData(bChunk *chunk);
int UnloaderAnimDirectoryData(bChunk *chunk);
int UnloaderWorldAnimEntityData(bChunk *chunk);
int UnloaderWorldAnimDirectoryData(bChunk *chunk);
int UnloaderWorldAnimTreeMarker(bChunk *chunk);
int UnloaderWorldAnimInstanceEntry(bChunk *chunk);
int UnloaderSun(bChunk *chunk);
int UnloaderCarInfo(bChunk *chunk);
int UnloaderLanguage(bChunk *chunk);
int UnloaderWeatherMan(bChunk *chunk);
int UnloaderFEngPackage(bChunk *chunk);
int UnloaderQuickSpline(bChunk *chunk);
int UnloaderICECameras(bChunk *chunk);
int UnloaderSoundStichs(bChunk *chunk);
int UnloaderFEPresetCars(bChunk *chunk);
int UnloaderParameterMaps(bChunk *chunk);
int UnloaderWCollisionPack(bChunk *chunk);
int UnloaderBounds(bChunk *chunk);
int UnloaderTrigger(bChunk *chunk);
int UnloaderColourCube(bChunk *chunk);

// Decl: 334
bChunkLoaderFunction UnloaderTable[26] = {
    UnloaderFEngFont,
    UnloaderEventManager,
    UnloaderVisibleSections,
    UnloaderEAGLAnimations,
    UnloaderEAGLSkeletons,
    UnloaderAnimSceneData,
    UnloaderAnimDirectoryData,
    UnloaderWorldAnimEntityData,
    UnloaderWorldAnimDirectoryData,
    UnloaderWorldAnimTreeMarker,
    UnloaderWorldAnimInstanceEntry,
    UnloaderSun,
    UnloaderCarInfo,
    UnloaderLanguage,
    UnloaderWeatherMan,
    UnloaderFEngPackage,
    UnloaderQuickSpline,
    UnloaderICECameras,
    UnloaderSoundStichs,
    UnloaderFEPresetCars,
    UnloaderParameterMaps,
    UnloaderWCollisionPack,
    UnloaderBounds,
    UnloaderTrigger,
    UnloaderColourCube,
    LoaderStub,
};

int CallChunkLoader(bChunk *chunk) {
    if (chunk->GetID() == 0) {
        return 1;
    }
    bChunkLoader *loader = bChunkLoader::FindLoader(chunk->ID);
    if (loader != nullptr) {
        int result = loader->GetLoaderFunction()(chunk);
        return result;
    }
    for (int loader_num = 0; loader_num < NUM_ELEMENTS(LoaderTable); loader_num++) {
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
    if (loader != nullptr) {
        int result = loader->GetUnloaderFunction()(chunk);
        return result;
    }
    for (int loader_num = 0; loader_num < NUM_ELEMENTS(LoaderTable); loader_num++) {
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
        bChunk *prev_chunk_table[max_prev_chunks];
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
            if (CallChunkUnloader(chunk) == 0) {
                bBreak();
            }
            last_chunk = chunk;
        }
    }
    PostLoadFixup();
}

float MoveChunkMemcpyTime = 0.0f; // Decl: 600
#ifdef FIX_BUGS
uint32 MoveChunkMemcpyAmount = 0.0f;
#else
float MoveChunkMemcpyAmount = 0.0f; // Decl: 601
#endif

void ScratchPadMemCpy(void *dest, const void *src, unsigned int numbytes) {
    bOverlappedMemCpy(dest, src, numbytes);
}

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

void MoveChunks(bChunk *dest_chunks, bChunk *source_chunks, int32 sizeof_chunks, const char *debug_name) {
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
    int movement_offset = (intptr_t)dest_chunks - (intptr_t)source_chunks;
    ChunkMovementOffset = movement_offset;
    chunk_range_table[num_chunk_ranges] = last_chunk;
    if (movement_offset < 0) {
        for (int n = 0; n < num_chunk_ranges; n++) {
            int size = (intptr_t)chunk_range_table[n + 1] - (intptr_t)chunk_range_table[n];
            MoveChunksRange(chunk_range_table[n], size, movement_offset, debug_name);
        }
    } else {
        for (int n = num_chunk_ranges - 1; n > -1; n--) {
            int size = (intptr_t)chunk_range_table[n + 1] - (intptr_t)chunk_range_table[n];
            MoveChunksRange(chunk_range_table[n], size, movement_offset, debug_name);
        }
    }
    ChunkMovementOffset = 0;
}

void LoadEmbeddedChunks(bChunk *chunk, int32 sizeof_chunks, const char *debug_name) {
    EndianSwapChunkHeadersRecursive(chunk, sizeof_chunks);
    LoadChunks(chunk, sizeof_chunks, debug_name);
}

void EndianSwapChunkHeader(bChunk *chunk) {
    bPlatEndianSwap(&chunk->ID); // TODO using the int override, huh?
    bPlatEndianSwap(&chunk->Size);
}

void EndianSwapChunkHeadersRecursive(bChunk *chunks, int32 sizeof_chunks) {
    bChunk *last_chunk = GetLastChunk(chunks, sizeof_chunks);
    EndianSwapChunkHeadersRecursive(chunks, last_chunk);
}

int PrintChunks = 0;     // Decl: 944
int PrintChunkLevel = 0; // Decl: 945

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
            if ((dest_buffer != nullptr) && (pad_size != 0)) {
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
                if (dest_buffer != nullptr) {
                    bChunk *dest_chunk = reinterpret_cast<bChunk *>(dest_buffer + dest_position);
                    dest_chunk->ID = source_chunk->ID;
                    dest_chunk->Size = new_dest_position - dest_position - sizeof(bChunk);
                }
                dest_position = new_dest_position;
            }
        } else if (is_temp_chunk == split_temp) {
            if (dest_buffer != nullptr) {
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
    if ((chunks == nullptr) || (sizeof_chunks == 0)) {
        return false;
    }
    int sizeof_perm_chunks = SplitPermTempChunks(false, chunks, sizeof_chunks, nullptr, 0, 0);
    int sizeof_temp_chunks = sizeof_chunks;
    if (SplitPermTempChunks(true, chunks, sizeof_chunks, nullptr, 0, 0) == 0) {
        LoadChunks(chunks, sizeof_chunks, debug_name);
        return false;
    } else {
        bChunk *perm_chunks = static_cast<bChunk *>(bMalloc(sizeof_perm_chunks, debug_name, 0, allocation_params));
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

bool PostLoadFixupDisabled = false; // Decl: 213

// TODO
extern int32 eDirtyTextures;

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

SlotPool *ResourceFileSlotPool = nullptr;

void InitResourceLoader() {
    ResourceFileSlotPool = bNewSlotPool(80, 80, "ResourceFileSlotPool", 0);
}

// STRIPPED
void CloseResourceLoader() {}

ResourceFile::ResourceFile(const char *filename, ResourceFileType type, int flags, int file_offset, int file_size) {
    this->Flags = flags;
    this->FileOffset = file_offset;
    this->FileSize = file_size;
    this->mEnableFreeMemory = true;
    this->Type = type;
    this->Filename = bAllocateSharedString(filename);
    this->HotFilename = nullptr;
    this->FileTransfersInProgress = 0;
    this->LoadingFinishedFlag = 0;
    this->HotFileNumber = 0;
    this->pLoadedHotFileEntries = nullptr;
    this->NumLoadedHotFileEntries = 0;
    this->Callback = nullptr;
    this->CallbackParam = nullptr;
    this->SetAllocationParams(0x2000, filename);
    this->HotFilename = nullptr;
    this->SizeofChunks = bFileSize(Filename);
    if (this->SizeofChunks < 0) {
        this->SizeofChunks = 0;
    }
    if (this->SizeofChunks != 0 && this->FileSize != 0) {
        this->SizeofChunks = this->FileSize;
    }
    this->pFirstChunk = nullptr;
}

void ResourceFile::SetAllocationParams(int allocation_params, const char *debug_name) {
    this->AllocationParams = allocation_params;
    this->AllocationName = bAllocateSharedString(debug_name);
}

void ResourceFile::AllocateMemory(bool loading_compressed_file) {
    if (this->SizeofChunks == 0) {
        return;
    }
    // TODO magic flags
    int allocation_params = this->AllocationParams;
    if (this->Flags & 2) {
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
    if (pool_num != 0 && this->SizeofChunks > bLargestMalloc(allocation_params)) {
        allocation_params &= ~0xF;
    }
    this->pFirstChunk = reinterpret_cast<bChunk *>(bMalloc(this->SizeofChunks, this->AllocationName, 0, allocation_params));
}

// STRIPPED
bool ResourceFile::IsFreeMemoryEnabled() {
    return false;
}

// STRIPPED
void ResourceFile::SetFreeMemoryEnabled(bool enable) {}

void ResourceFile::FreeMemory() {
    if (this->mEnableFreeMemory && (this->pFirstChunk != nullptr)) {
        bFree(this->pFirstChunk);
        this->pFirstChunk = nullptr;
    }
}

void ResourceFile::BeginLoading(void (*callback)(void *), void *callback_param) {
    this->Callback = callback;
    this->CallbackParam = callback_param;
    NumResourcesBeingLoaded++;
    if (this->GetMemory() == nullptr) {
        bool loading_compressed_file = ((Flags >> 3) & 1) != 0;
        this->AllocateMemory(loading_compressed_file);
    }
    if (SizeofChunks != 0) {
        this->FileTransfersInProgress++;
        AddQueuedFile(pFirstChunk, this->Filename, this->FileOffset, this->SizeofChunks, ResourceFile::FileTransferCallback, this, nullptr);
    }
}

void ResourceFile::ManualUnload() {
    UnloadChunks(this->pFirstChunk, this->SizeofChunks, this->GetFilename());
    this->pFirstChunk = nullptr;
}

void ResourceFile::ManualReload(bChunk *new_chunks) {
    this->pFirstChunk = new_chunks;
    LoadChunks(new_chunks, this->SizeofChunks, this->GetFilename());
}

ResourceFile::~ResourceFile() {
    if (this->pFirstChunk != nullptr) {
        UnloadChunks(this->pFirstChunk, this->SizeofChunks, this->GetFilename());
        this->FreeMemory();
    }
    bFreeSharedString(this->Filename);
    bFreeSharedString(this->HotFilename);
    bFreeSharedString(this->AllocationName);
}

// TODO get rid of goto
void ResourceFile::LoadResourceIfFileTransferFinished() {
    if (this->LoadingFinishedFlag || this->FileTransfersInProgress) {
        return;
    }
    if (this->Flags & 8) {
        LZHeader *header = reinterpret_cast<LZHeader *>(this->pFirstChunk);
        if (header != nullptr) {
            bPlatEndianSwap(&header->ID);
            bPlatEndianSwap(&header->Flags);
            bPlatEndianSwap(&header->UncompressedSize);
            bPlatEndianSwap(&header->CompressedSize);
            if (LZValidHeader(header)) {
                uint8 *compressed_data = reinterpret_cast<uint8 *>(this->pFirstChunk);
                this->SizeofChunks = header->UncompressedSize;
                this->pFirstChunk = nullptr;
                if (this->SizeofChunks != 0) {
                    this->AllocateMemory(false);
                    LZDecompress(compressed_data, reinterpret_cast<uint8 *>(this->pFirstChunk));
                }
                bFree(compressed_data);
            }
            goto block_8;
        }
    } else {
    block_8:
        if (this->pFirstChunk != nullptr) {
            EndianSwapChunkHeadersRecursive(this->pFirstChunk, this->SizeofChunks);
        }
    }
    LoadTempPermChunks(&this->pFirstChunk, &this->SizeofChunks, this->AllocationParams, this->AllocationName);
    if (this->Flags & 4) {
        if (this->pFirstChunk != nullptr) {
            this->FreeMemory();
        }
        this->SizeofChunks = 0;
    }
    this->LoadingFinishedFlag = 1;
}

void ResourceFile::FileTransferCallback(void *param, int error_status) {
    reinterpret_cast<ResourceFile *>(param)->FileTransfersInProgress--;
}

// STRIPPED
int ResourceFile::GetSize(int chunk_id, int *pnum_chunks) {
    return 0;
}

bTList<ResourceFile> ResourceFileList;

ResourceFile *CreateResourceFile(const char *filename, ResourceFileType type, int flags, int file_offset, int file_size) {
    ResourceFile *r = ResourceFileList.AddTail(new ResourceFile(filename, type, flags, file_offset, file_size));
    return r;
}

ResourceFile *LoadResourceFile(const char *filename, enum ResourceFileType type, int flags, void (*callback)(void *), void *callback_param,
                               int file_offset, int file_size) {
    ResourceFile *r = CreateResourceFile(filename, type, flags, file_offset, file_size);
    r->BeginLoading(callback, callback_param);
    return r;
}

void UnloadResourceFile(ResourceFile *resource_file) {
    if (resource_file == nullptr) {
        return;
    }
    while (!resource_file->IsFinishedLoading()) {
        ServiceResourceLoading();
    }
    ResourceFileList.Remove(resource_file);
    delete resource_file;
}

int ServiceResourceLoading() {
    ProfileNode profile_node("TODO", 0);

    while (NumDelayedResourceCallbacks != 0) {
        ProfileNode profile_node("TODO", 0);
        DelayedResourceCallback drc = DelayedResourceCallbacks[0];

        if (NumDelayedResourceCallbacks > 1) {
            bOverlappedMemCpy(&DelayedResourceCallbacks[0], &DelayedResourceCallbacks[1],
                              NumDelayedResourceCallbacks * sizeof(DelayedResourceCallback));
        }
        (NumDelayedResourceCallbacks--, drc.pCallback)(drc.Param);
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
    return 0;
}

int IsResourceLoadingComplete() {
    return static_cast<int>(NumResourcesBeingLoaded == 0 && NumDelayedResourceCallbacks == 0);
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

// STRIPPED
int GetResourceFileSize(ResourceFileType type, int chunk_id, int *pnum_chunks) {}

// STRIPPED
int GetResourceFileSize(int chunk_id, int *pnum_chunks) {}

// STRIPPED
void PrintResourceFiles(ResourceFileType type) {}

// STRIPPED
void PrintResourceFiles(ResourceFileType type, bFile *file) {}

int CurrentlyHotChunking = 0; // Decl: 1826

bool IsCurrentlyHotChunking() {
    return CurrentlyHotChunking != 0;
}

int LoaderWCollisionPack(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_W_COLLISION_ASSETS) {
        WCollisionAssets::Get().LoadCollisionPack(chunk);
        return 1;
    }
    return 0;
}

int UnloaderWCollisionPack(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_W_COLLISION_ASSETS) {
        WCollisionAssets::Get().UnLoadCollisionPack(chunk);
        return 1;
    }
    return 0;
}

int LoaderColourCube(bChunk *chunk) {
    return static_cast<int>(chunk->GetID() == BCHUNK_COLOUR_CUBE);
}

int UnloaderColourCube(bChunk *chunk) {
    return static_cast<int>(chunk->GetID() == BCHUNK_FENG_FONT);
}

VMFile::VMFile() {
    this->mInit = false;
    this->mCompressed = false;
    this->mSize = 0;
    this->mSizeOfChunks = 0;
    this->mVirtMemAddr = nullptr;
    this->mUsedTrackPool = false;
    bMemSet(this->mFilename, 0, sizeof(this->mFilename));
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

void MoveFileIntoVirtualMemoryThenLoadChunks(intptr_t param, int err) {
    VMFile *vm_file = reinterpret_cast<VMFile *>(param);
    if (!vm_file->mInit) {
        return;
    }

    void *old_memory = vm_file->mMainMemAddr;
    int vm_file_size = vm_file->mSize;
    unsigned int sizeofchunks = vm_file_size;

    if (vm_file->mCompressed) {
        LZHeader *header = reinterpret_cast<LZHeader *>(old_memory);
        bPlatEndianSwap(&header->ID);
        bPlatEndianSwap(&header->Flags);
        bPlatEndianSwap(&header->UncompressedSize);
        bPlatEndianSwap(&header->CompressedSize);

        if (LZValidHeader(header)) {
            sizeofchunks = header->UncompressedSize;
            uint8 *compressed_data = reinterpret_cast<uint8 *>(header);
            old_memory = nullptr;

            if (sizeofchunks != 0) {
                int allocation_params = GetVirtualMemoryAllocParams();
                void *realloc = bMalloc(sizeofchunks, "TODO2", 0, allocation_params);
                LZDecompress(compressed_data, static_cast<uint8 *>(realloc));
                old_memory = realloc;
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

    if (new_mem != nullptr) {
        EndianSwapChunkHeadersRecursive(static_cast<bChunk *>(new_mem), sizeofchunks);
    }

    LoadChunks(static_cast<bChunk *>(new_mem), sizeofchunks, vm_file->mFilename);
    vm_file->mSizeOfChunks = sizeofchunks;
}

void UnloadFileFromVirtualMemory(VMFile *vm_file) {
    UnloadChunks(static_cast<bChunk *>(vm_file->mVirtMemAddr), vm_file->mSizeOfChunks, vm_file->mFilename);
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
    if (vm_file == nullptr) {
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
