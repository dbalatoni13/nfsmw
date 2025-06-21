#pragma once

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include <cstddef>

struct eStreamingEntry {
    // total size: 0x18
    unsigned int NameHash;        // offset 0x0, size 0x4
    unsigned int ChunkByteOffset; // offset 0x4, size 0x4
    int ChunkByteSize;            // offset 0x8, size 0x4
    int UncompressedSize;         // offset 0xC, size 0x4
    unsigned char UserFlags;      // offset 0x10, size 0x1
    unsigned char Flags;          // offset 0x11, size 0x1
    unsigned short RefCount;      // offset 0x12, size 0x2
    unsigned char *ChunkData;     // offset 0x14, size 0x4

    void EndianSwap() {}
};

struct eStreamingPackHeaderLoadingInfo {
    // total size: 0x10
    int HeaderChunksSize;                // offset 0x0, size 0x4
    int HeaderChunksMemCopied;           // offset 0x4, size 0x4
    void (*LoadingDoneCallback)(void *); // offset 0x8, size 0x4
    void *LoadingDoneCallbackParam;      // offset 0xC, size 0x4
};

struct eStreamingPack : public bTNode<eStreamingPack> {
    // total size: 0x44
    const char *Filename;                         // offset 0x8, size 0x4
    short HeaderMemoryPoolNum;                    // offset 0xC, size 0x2
    char DisabledFlag;                            // offset 0xE, size 0x1
    eStreamingPackHeaderLoadingInfo *LoadingInfo; // offset 0x10, size 0x4
    int HeaderLoaded;                             // offset 0x14, size 0x4
    int HeaderSize;                               // offset 0x18, size 0x4
    bChunk *HeaderChunks;                         // offset 0x1C, size 0x4
    struct ResourceFile *pResourceFile;           // offset 0x20, size 0x4
    eStreamingEntry *StreamingEntryTable;         // offset 0x24, size 0x4
    int StreamingEntryNumEntries;                 // offset 0x28, size 0x4
    int NumLoadsPending;                          // offset 0x2C, size 0x4
    int NumLoadedStreamingEntries;                // offset 0x30, size 0x4
    int NumLoadedBytes;                           // offset 0x34, size 0x4
    int RefCount;                                 // offset 0x38, size 0x4
    struct eSolidListHeader *SolidListHeader;     // offset 0x3C, size 0x4
    struct TexturePackHeader *pTexturePackHeader; // offset 0x40, size 0x4

    void *operator new(size_t size) {}
    void operator delete(void *ptr) {}

    void RegisterLoadStreamingEntry(eStreamingEntry *entry) {}

    void RegisterUnloadStreamingEntry(eStreamingEntry *entry) {}
};

struct eStreamingPackHeaderLoadingInfoPhase1 {
    // total size: 0x10
    const char *Filename;     // offset 0x0, size 0x4
    bChunk *TempHeaderChunks; // offset 0x4, size 0x4
    int NextLoadPosition;     // offset 0x8, size 0x4
    int NextLoadAmount;       // offset 0xC, size 0x4
};

struct eStreamingPackHeaderLoadingInfoPhase2 {
    // total size: 0x28
    const char *Filename;                     // offset 0x0, size 0x4
    bChunk *HeaderChunks;                     // offset 0x4, size 0x4
    int HeaderChunksMemCopied;                // offset 0x8, size 0x4
    eStreamingPack *pStreamingPack;           // offset 0xC, size 0x4
    eStreamingEntry *StreamingEntryTable;     // offset 0x10, size 0x4
    int StreamingEntryNumEntries;             // offset 0x14, size 0x4
    struct eSolidListHeader *SolidListHeader; // offset 0x18, size 0x4
    TexturePackHeader *pTexturePackHeader;    // offset 0x1C, size 0x4
    int LoadResourceFilePosition;             // offset 0x20, size 0x4
    int LoadResourceFileAmount;               // offset 0x24, size 0x4
};

struct eStreamPackLoader {
    // total size: 0x38
    int RequiredChunkAlignment;                                                            // offset 0x0, size 0x4
    void (*LoadedStreamingEntryCallback)(bChunk *, eStreamingEntry *, eStreamingPack *);   // offset 0x4, size 0x4
    void (*UnloadedStreamingEntryCallback)(bChunk *, eStreamingEntry *, eStreamingPack *); // offset 0x8, size 0x4
    void (*LoadingHeaderPhase1Callback)(eStreamingPackHeaderLoadingInfoPhase1 *);          // offset 0xC, size 0x4
    void (*LoadingHeaderPhase2Callback)(eStreamingPackHeaderLoadingInfoPhase2 *);          // offset 0x10, size 0x4
    void (*UnloadingHeaderCallback)(eStreamingPack *);                                     // offset 0x14, size 0x4
    bTList<eStreamingPack> LoadedStreamingPackList;                                        // offset 0x18, size 0x8
    int PrintLoading;                                                                      // offset 0x20, size 0x4
    int PrintInventory;                                                                    // offset 0x24, size 0x4
    const char *(*DebugGetNameFunc)(unsigned int);                                         // offset 0x28, size 0x4
    int NumLoadedStreamingPacks;                                                           // offset 0x2C, size 0x4
    int NumLoadedStreamingEntries;                                                         // offset 0x30, size 0x4
    int NumLoadedBytes;                                                                    // offset 0x34, size 0x4

    eStreamingPack *CreateStreamingPack(const char *filename, void (*callback_function)(void *), void *callback_param, int memory_pool_num);
    void LoadStreamingEntry(unsigned int *name_hash_table, int num_hashes, void (*callback)(void *), void *param0, int memory_pool_num);
    void WaitForLoadingToFinish(const char *filename);
    void UnloadStreamingEntry(unsigned int name_hash, eStreamingPack *streaming_pack);
    void UnloadStreamingEntry(unsigned int *name_hash_table, int num_hashes);
    void UnloadAllStreamingEntries(const char *filename);
    eStreamingPack *GetLoadedStreamingPack(const char *filename);
    int DeleteStreamingPack(const char *filename);

    void DisableStreamingPack(const char *filename) {}

    void EnableStreamingPack(const char *filename) {}

    void RegisterLoadStreamingEntry(eStreamingEntry *entry) {}

    void RegisterUnloadStreamingEntry(eStreamingEntry *entry) {}

    bTList<eStreamingPack> *GetLoadedStreamingPackList() {}
};

struct eStreamingPackLoadTable {
    // total size: 0x10
    char MemoryPoolNum; // offset 0x0, size 0x1
    char Locked; // offset 0x1, size 0x1
    short NumLoadsPending; // offset 0x2, size 0x2
    void (* Callback)(void *); // offset 0x4, size 0x4
    void * Param; // offset 0x8, size 0x4
    eStreamPackLoader * StreamPackLoader; // offset 0xC, size 0x4
};

extern eStreamPackLoader StreamingTexturePackLoader;
