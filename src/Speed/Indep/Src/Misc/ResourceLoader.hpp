//
//
//
//
//
//
//
//
//
#ifndef RESOURCELOADER_HPP
#define RESOURCELOADER_HPP

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/file/driver.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

// Decl: 24
enum ResourceFileType {
    RESOURCE_FILE_NONE = 0,
    RESOURCE_FILE_GLOBAL = 1,
    RESOURCE_FILE_FRONTEND = 2,
    RESOURCE_FILE_INGAME = 3,
    RESOURCE_FILE_TRACK = 4,
    RESOURCE_FILE_NIS = 5,
    RESOURCE_FILE_CAR = 6,
    RESOURCE_FILE_LANGUAGE = 7,
    RESOURCE_FILE_REPLAY = 8,
};

enum ResourceFileFlags {
    RESOURCE_FILE_FLAG_HOTCHUNKABLE = 1,
    RESOURCE_FILE_FLAG_USE_TEMPORARY_MEMORY = 2,
    RESOURCE_FILE_FLAG_FREE_AFTER_LOADING = 4,
    RESOURCE_FILE_FLAG_LZCOMPRESSED = 8,
};

extern SlotPool *ResourceFileSlotPool; // Decl: 47

struct LoadedHotFileEntry;

// total size: 0x50
// Decl: 50
class ResourceFile : public bTNode<ResourceFile> {
  public:
    ResourceFile(const char *filename, ResourceFileType type, int flags, int file_offset, int file_size); // Decl: 53
    ~ResourceFile();                                                                                      // Decl: 54

    USE_SLOTALLOC(ResourceFileSlotPool); // Decl: 56

    void AllocateMemory(bool loading_compressed_file);                       // Decl: 59
    void FreeMemory();                                                       // Decl: 60
    bool IsFreeMemoryEnabled();                                              // Decl: 62
    void SetFreeMemoryEnabled(bool enable);                                  // Decl: 63
    void SetAllocationParams(int allocation_params, const char *debug_name); // Decl: 69
    // Decl: 70
    void AssignMemory(void *mem, int allocation_params, const char *debug_name) {
        pFirstChunk = reinterpret_cast<bChunk *>(mem);
        this->SetAllocationParams(allocation_params, debug_name);
    }
    // Decl: 71
    void *GetMemory() {
        return pFirstChunk;
    }
    // int GetAddress() {} // Decl: 72

    void BeginLoading(void (*callback)(void *), void *callback_param); // Decl: 74

    void BeginLoading(ASYNCFILE_CALLBACK *callback, intptr_t callback_param) {
        this->BeginLoading(reinterpret_cast<void (*)(void *)>(callback), reinterpret_cast<void *>(callback_param));
    }

    void BeginLoading() {
        BeginLoading(reinterpret_cast<void (*)(void *)>(0), reinterpret_cast<void *>(0));
    }

    // Decl: 76
    int IsFinishedLoading() {
        return LoadingFinishedFlag;
    }

    void LoadResourceIfFileTransferFinished(); // Decl: 78

    void ManualUnload();                   // Decl: 81
    void ManualReload(bChunk *new_chunks); // Decl: 82

    // Decl: 84
    void CallCallback() {
        if (Callback != nullptr) {
            Callback(CallbackParam);
        }
    }

    // Decl: 86
    const char *GetFilename() {
        return Filename;
    }
    // Decl: 87
    const char *GetHotFilename() {
        return HotFilename;
    }
    // Decl: 88
    ResourceFileType GetType() {
        return Type;
    }

    // Decl: 91
    void ChangeFilenameForHotChunking(const char *filename) {
        bFreeSharedString(Filename);
        Filename = bAllocateSharedString(filename);
    }

    // Decl: 98
    void SetHotFileNumber(int number) {
        HotFileNumber = number;
    }
    // Decl: 99
    int GetHotFileNumber() {
        return HotFileNumber;
    }

    // int IsHotChunkable() {} // Decl: 101

    int GetSize(int chunk_id, int *pnum_chunks); // Decl: 105

    // Decl: 107
    bChunk *GetFirstChunk() {
        return pFirstChunk;
    }
    // Decl: 108
    int GetSizeofChunks() {
        return SizeofChunks;
    }

  private:
    static void FileTransferCallback(void *param, int error_status); // Decl: 111

    bool mEnableFreeMemory;      // offset 0x8, size 0x1, Decl: 113
    ResourceFileType Type;       // offset 0xC, size 0x4, Decl: 115
    int Flags;                   // offset 0x10, size 0x4, Decl: 116
    int FileOffset;              // offset 0x14, size 0x4, Decl: 117
    int FileSize;                // offset 0x18, size 0x4, Decl: 118
    const char *Filename;        // offset 0x1C, size 0x4, Decl: 119
    const char *HotFilename;     // offset 0x20, size 0x4, Decl: 120
    const char *AllocationName;  // offset 0x24, size 0x4, Decl: 121
    int AllocationParams;        // offset 0x28, size 0x4, Decl: 122
    int FileTransfersInProgress; // offset 0x2C, size 0x4, Decl: 123
    int LoadingFinishedFlag;     // offset 0x30, size 0x4, Decl: 124

    void (*Callback)(void *); // offset 0x34, size 0x4, Decl: 126
    void *CallbackParam;      // offset 0x38, size 0x4, Decl: 127

    bChunk *pFirstChunk; // offset 0x3C, size 0x4, Decl: 129
    int SizeofChunks;    // offset 0x40, size 0x4, Decl: 130

    LoadedHotFileEntry *pLoadedHotFileEntries; // offset 0x44, size 0x4, Decl: 132
    int NumLoadedHotFileEntries;               // offset 0x48, size 0x4, Decl: 133

    int HotFileNumber; // offset 0x4C, size 0x4, Decl: 135
};

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

void InitResourceLoader();
void LoadEmbeddedChunks(struct bChunk *chunk, int32 sizeof_chunks, const char *debug_name);
void WaitForResourceLoadingComplete();
int ServiceResourceLoading();

ResourceFile *FindResourceFile(ResourceFileType type);
ResourceFile *LoadResourceFile(const char *filename, ResourceFileType type, int flags, void (*callback)(void *), void *callback_param,
                               int file_offset, int file_size);
ResourceFile *CreateResourceFile(const char *filename, ResourceFileType type, int flags, int file_offset, int file_size);
void UnloadResourceFile(ResourceFile *resource_file);
VMFile *LoadFileIntoVirtualMemory(const char *filename, bool compressed, bool use_trackstreampool_as_temp);
void UnloadFileFromVirtualMemory(VMFile *vm_file);

void EndianSwapChunkHeader(bChunk *chunk);
void EndianSwapChunkHeadersRecursive(bChunk *chunks, int32 sizeof_chunks);
void EndianSwapChunkHeadersRecursive(bChunk *first_chunk, bChunk *last_chunk);

void SetDelayedResourceCallback(void (*callback)(void *), void *param);

bool LoadTempPermChunks(bChunk **ppchunks, int *psizeof_chunks, int allocation_params, const char *debug_name);
void MoveChunks(bChunk *dest_chunks, bChunk *source_chunks, int32 sizeof_chunks, const char *debug_name);
void UnloadChunks(bChunk *chunks, int sizeof_chunks, const char *debug_name);
void PostLoadFixup();

void ScratchPadMemCpy(void *dest, const void *src, unsigned int numbytes);

extern bool PostLoadFixupDisabled; // Decl: 213

inline ResourceFile *LoadResourceFile(const char *filename, ResourceFileType type, int flags) {
    return LoadResourceFile(filename, type, flags, nullptr, nullptr, 0, 0);
}

inline ResourceFile *LoadResourceFile(const char *filename, ResourceFileType type, int flags, void (*callback)(intptr_t), intptr_t callback_param,
                                      int file_offset, int file_size) {
    return LoadResourceFile(filename, type, flags, reinterpret_cast<void (*)(void *)>(callback), reinterpret_cast<void *>(callback_param),
                            file_offset, file_size);
}

extern int ChunkMovementOffset; // size: 0x4, Decl: 223
// Decl: 224
inline int GetChunkMovementOffset() {
    return ChunkMovementOffset;
}

// Decl: 225
inline bool AreChunksBeingMoved() {
    return ChunkMovementOffset != 0;
}

inline void SetDelayedResourceCallback(void (*callback)(intptr_t), intptr_t param) {
    SetDelayedResourceCallback(reinterpret_cast<void (*)(void *)>(callback), reinterpret_cast<void *>(param));
}

bool IsCurrentlyHotChunking();

#endif
