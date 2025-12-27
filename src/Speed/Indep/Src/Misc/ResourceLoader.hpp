#ifndef MISC_RESOURCE_LOADER_H
#define MISC_RESOURCE_LOADER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

enum ResourceFileType {
    RESOURCE_FILE_REPLAY = 8,
    RESOURCE_FILE_LANGUAGE = 7,
    RESOURCE_FILE_CAR = 6,
    RESOURCE_FILE_NIS = 5,
    RESOURCE_FILE_TRACK = 4,
    RESOURCE_FILE_INGAME = 3,
    RESOURCE_FILE_FRONTEND = 2,
    RESOURCE_FILE_GLOBAL = 1,
    RESOURCE_FILE_NONE = 0,
};

// total size: 0x50
class ResourceFile : public bTNode<ResourceFile> {
  public:
    void *GetMemory() {
        return this->pFirstChunk;
    }

    void SetAllocationParams(int allocation_params, const char *debug_name);
    void BeginLoading(void (*callback)(void *), void *callback_param);

  private:
    bool mEnableFreeMemory;                           // offset 0x8, size 0x1
    enum ResourceFileType Type;                       // offset 0xC, size 0x4
    int Flags;                                        // offset 0x10, size 0x4
    int FileOffset;                                   // offset 0x14, size 0x4
    int FileSize;                                     // offset 0x18, size 0x4
    const char *Filename;                             // offset 0x1C, size 0x4
    const char *HotFilename;                          // offset 0x20, size 0x4
    const char *AllocationName;                       // offset 0x24, size 0x4
    int AllocationParams;                             // offset 0x28, size 0x4
    int FileTransfersInProgress;                      // offset 0x2C, size 0x4
    int LoadingFinishedFlag;                          // offset 0x30, size 0x4
    void (*Callback)(void *);                         // offset 0x34, size 0x4
    void *CallbackParam;                              // offset 0x38, size 0x4
    struct bChunk *pFirstChunk;                       // offset 0x3C, size 0x4
    int SizeofChunks;                                 // offset 0x40, size 0x4
    struct LoadedHotFileEntry *pLoadedHotFileEntries; // offset 0x44, size 0x4
    int NumLoadedHotFileEntries;                      // offset 0x48, size 0x4
    int HotFileNumber;                                // offset 0x4C, size 0x4
};

void InitResourceLoader();

void EndianSwapChunkHeader(bChunk *chunk);
void EndianSwapChunkHeadersRecursive(bChunk *chunks, int sizeof_chunks);
void EndianSwapChunkHeadersRecursive(bChunk *first_chunk, bChunk *last_chunk);

int ServiceResourceLoading();
ResourceFile *CreateResourceFile(const char *filename, ResourceFileType type, int flags, int flag_offset, int file_size);
void UnloadResourceFile(ResourceFile *resource_file);

extern int ChunkMovementOffset; // size: 0x4

inline bool AreChunksBeingMoved() {
    return ChunkMovementOffset;
}

inline int GetChunkMovementOffset() {
    return ChunkMovementOffset;
}

#endif
